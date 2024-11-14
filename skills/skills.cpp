#define IMGUI_DEFINE_MATH_OPERATORS
#include <map>
#include <algorithm>
#include <iostream>
#include <utility>

#include "skills.h"

#include <set>

void Skills::OnStart() {
    ed::Config config;

    config.SettingsFile = "skills.json";

    config.UserPointer = this;

    config.LoadNodeSettings = [](ed::NodeId nodeId, char *data, void *userPointer) -> size_t {
        const auto self = static_cast<Skills *>(userPointer);

        const auto node = self->FindNode(nodeId);
        if (!node)
            return 0;

        if (data != nullptr)
            memcpy(data, node->State.data(), node->State.size());
        return node->State.size();
    };

    config.SaveNodeSettings = [](ed::NodeId nodeId, const char *data, size_t size, ed::SaveReasonFlags reason,
                                 void *userPointer) -> bool {
        auto self = static_cast<Skills *>(userPointer);

        auto node = self->FindNode(nodeId);
        if (!node)
            return false;

        node->State.assign(data, size);

        self->TouchNode(nodeId);

        return true;
    };

    m_Editor = ed::CreateEditor(&config);
    ed::SetCurrentEditor(m_Editor);


    m_HeaderBackground = LoadTexture("data/BlueprintBackground.png");

    CreateNodes();
}

void Skills::OnStop() {
    SaveToJson("graph.json");
    auto releaseTexture = [this](ImTextureID &id) {
        if (id) {
            DestroyTexture(id);
            id = nullptr;
        }
    };

    releaseTexture(m_HeaderBackground);

    if (m_Editor) {
        ed::DestroyEditor(m_Editor);
        m_Editor = nullptr;
    }
}

void Skills::OnFrame(float deltaTime) {
    UpdateTouch();

    const auto &io = ImGui::GetIO();

    ed::SetCurrentEditor(m_Editor);

    ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate != 0 ? 1000.0f / io.Framerate : 0.0f); // type: ignore

    static bool createNewNode = false;
    static Pin *newNodeLinkPin = nullptr;
    static Pin *newLinkPin = nullptr;

    static float leftPaneWidth = 400.0f;
    static float rightPaneWidth = 800.0f;
    Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

    ShowLeftPane(leftPaneWidth - 4.0f);

    ImGui::SameLine(0.0f, 12.0f);

    ed::Begin("Node editor"); {
        auto cursorTopLeft = ImGui::GetCursorScreenPos();

        util::BlueprintNodeBuilder builder(m_HeaderBackground, GetTextureWidth(m_HeaderBackground),
                                           GetTextureHeight(m_HeaderBackground));

        for (auto &node: m_Nodes) {
            draw_node(node, builder, newLinkPin);
        }

        for (auto &link: m_Links)
            ed::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);

        if (!createNewNode) {
            newNodeLinkPin = handle_creation_and_destruction(newLinkPin, createNewNode);
        }

        ImGui::SetCursorScreenPos(cursorTopLeft);
    }

    PopupHandler(newNodeLinkPin, createNewNode);
}

void Skills::PopupHandler(Pin* newNodeLinkPin, bool &createNewNode) {
    static ed::NodeId contextNodeId = 0;
    static ed::LinkId contextLinkId = 0;
    static ed::PinId contextPinId = 0;
    static bool can_show = true;

    auto openPopupPosition = ImGui::GetMousePos();
    ed::Suspend();
    if (ed::ShowNodeContextMenu(&contextNodeId))
        ImGui::OpenPopup("Node Context Menu");
    else if (ed::ShowPinContextMenu(&contextPinId))
        ImGui::OpenPopup("Pin Context Menu");
    else if (ed::ShowLinkContextMenu(&contextLinkId))
        ImGui::OpenPopup("Link Context Menu");
    else if (ed::ShowBackgroundContextMenu() ||( ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Space)) && can_show)) {
        can_show = false;
        ImGui::OpenPopup("Create New Node");
        newNodeLinkPin = nullptr;
        can_show = true;
    }
    ed::Resume();

    ed::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("Node Context Menu")) {
        auto node = FindNode(contextNodeId);

        if (node->Type == NodeType::Comment) {
            ImGui::TextUnformatted(node->Name.c_str());
        } else {
            ImGui::TextUnformatted("Node Context Menu");
            ImGui::Separator();

            ImGui::Text("ID: %p", node->ID.AsPointer());
            ImGui::Text("Type: %s", node->Type == NodeType::Blueprint
                                        ? "Blueprint"
                                        : "Tree");
            ImGui::Text("Inputs: %d", static_cast<int>(node->Inputs.size()));
            ImGui::Text("Outputs: %d", static_cast<int>(node->Outputs.size()));
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteNode(contextNodeId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Pin Context Menu")) {
        auto pin = FindPin(contextPinId);

        ImGui::TextUnformatted("Pin Context Menu");
        ImGui::Separator();
        if (pin) {
            ImGui::Text("ID: %p", pin->ID.AsPointer());
            if (pin->Node)
                ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
            else
                ImGui::Text("Node: %s", "<none>");
        } else
            ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Link Context Menu")) {
        auto link = FindLink(contextLinkId);

        ImGui::TextUnformatted("Link Context Menu");
        ImGui::Separator();
        if (link) {
            ImGui::Text("ID: %p", link->ID.AsPointer());
            ImGui::Text("From: %p", link->StartPinID.AsPointer());
            ImGui::Text("To: %p", link->EndPinID.AsPointer());
        } else
            ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteLink(contextLinkId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Create New Node")) {
        Node *node = choice();

        create_node(node, openPopupPosition, createNewNode, newNodeLinkPin);

        ImGui::EndPopup();
    } else
        createNewNode = false;
    ImGui::PopStyleVar();
    ed::Resume();

    ed::End();
}

void Nodes::draw_node(Node &node, util::BlueprintNodeBuilder &builder, const Pin *newLinkPin) {
    static bool do_event_popup = false;
    static bool do_tooltip = false;
    static bool do_tooltip_in_out = false;
    static std::string do_tooltip_in_out_desc;


    if (node.Type == NodeType::Blueprint || node.Type == NodeType::Simple) {
        const auto isSimple = node.Type == NodeType::Simple;

        builder.Begin(node.ID);
        if (!isSimple) {
            builder.Header(node.Color);
            ImGui::Spring(0);
            ImGui::TextUnformatted(node.Name.c_str());
            do_tooltip = ImGui::IsItemHovered();
            ImGui::Spring(1);
            ImGui::Dummy(ImVec2(0, 28));
            builder.EndHeader();
        }

        for (auto &input: node.Inputs) {
            auto alpha = ImGui::GetStyle().Alpha;
            if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
                alpha = alpha * (48.0f / 255.0f);

            builder.Input(input.ID);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            DrawPinIcon(input, IsPinLinked(input.ID), static_cast<int>(alpha * 255));
            ImGui::Spring(0);
            if (!input.Name.empty()) {
                ImGui::Spring(0);
                ImGui::TextUnformatted(input.Name.c_str());

            }
            if (input.can_ask_on_device) {
                if (input.Type == PinType::String) {
                    static char buffer[128] = "Edit Me";
                    static bool wasActive = false;

                    ImGui::PushItemWidth(100.0f);
                    ImGui::InputText("##edit", buffer, 127);
                    ImGui::PopItemWidth();
                    if (ImGui::IsItemActive() && !wasActive) {
                        ed::EnableShortcuts(false);
                        wasActive = true;
                    } else if (!ImGui::IsItemActive() && wasActive) {
                        ed::EnableShortcuts(true);
                        wasActive = false;
                    }
                    ImGui::Spring(0);
                }
                if (input.Type == PinType::Bool) {
                    static bool check = true;
                    ImGui::Checkbox("", &check);
                }
                if (input.Type == PinType::Float) {
                    static float f0 = 0.001f;
                    ImGui::InputFloat("Float", &f0, 0.01f, 1.0f, "%.3f");
                }
                if (input.Type == PinType::Int) {
                    // Arrow buttons with Repeater
                    static int counter = 0;
                    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                    ImGui::PushButtonRepeat(true);
                    if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { counter--; }
                    ImGui::SameLine(0.0f, spacing);
                    if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { counter++; }
                    ImGui::PopButtonRepeat();
                    ImGui::SameLine();
                    ImGui::Text("%d", counter);
                }
            }

            ImGui::PopStyleVar();
            builder.EndInput();
            do_tooltip_in_out = ImGui::IsItemHovered();
            if (do_tooltip_in_out)
                do_tooltip_in_out_desc = input.Name;
        }

        if (isSimple) {
            builder.Middle();

            ImGui::Spring(1, 0);
            ImGui::TextUnformatted(node.Name.c_str());
            ImGui::Spring(1, 0);
        }

        for (auto &output: node.Outputs) {
            if (!isSimple && output.Type == PinType::Delegate)
                continue;

            auto alpha = ImGui::GetStyle().Alpha;
            if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
                alpha = alpha * (48.0f / 255.0f);

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            builder.Output(output.ID);
            if (output.can_ask_on_device) {
                if (output.Type == PinType::String) {
                    static char buffer[128] = "Edit Me";
                    static bool wasActive = false;

                    ImGui::PushItemWidth(100.0f);
                    ImGui::InputText("##edit", buffer, 127);
                    ImGui::PopItemWidth();
                    if (ImGui::IsItemActive() && !wasActive) {
                        ed::EnableShortcuts(false);
                        wasActive = true;
                    } else if (!ImGui::IsItemActive() && wasActive) {
                        ed::EnableShortcuts(true);
                        wasActive = false;
                    }
                    ImGui::Spring(0);
                }
                if (output.Type == PinType::Bool) {
                    static bool check = true;
                    ImGui::Checkbox("True", &check);
                }
                if (output.Type == PinType::Float) {
                    static float f0 = 0.001f;
                    ImGui::InputFloat("Float", &f0, 0.01f, 1.0f, "%.3f");
                }
                if (output.Type == PinType::Int) {
                    // Arrow buttons with Repeater
                    static int counter = 0;
                    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                    ImGui::PushButtonRepeat(true);
                    if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { counter--; }
                    ImGui::SameLine(0.0f, spacing);
                    if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { counter++; }
                    ImGui::PopButtonRepeat();
                    ImGui::SameLine();
                    ImGui::Text("%d", counter);
                }
                if (output.Type == PinType::Event) {
                    if (ImGui::Button("Chose Event")) {
                        do_event_popup = true;
                    }
                }
            }
            if (!output.Name.empty()) {
                ImGui::Spring(0);
                ImGui::TextUnformatted(output.Name.c_str());

            }
            ImGui::Spring(0);
            DrawPinIcon(output, IsPinLinked(output.ID), static_cast<int>(alpha * 255));
            ImGui::PopStyleVar();
            builder.EndOutput();
            do_tooltip_in_out = ImGui::IsItemHovered();
            if (do_tooltip_in_out)
                do_tooltip_in_out_desc = output.Name;
        }

        builder.End();
    }
    else if (node.Type == NodeType::Tree) {
        constexpr float rounding = 5.0f;
        constexpr float padding = 12.0f;

        const auto pinBackground = ed::GetStyle().Colors[ed::StyleColor_NodeBg];

        ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
        ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
        ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));
        ed::PushStyleColor(ed::StyleColor_PinRectBorder, ImColor(60, 180, 255, 150));

        ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
        ed::PushStyleVar(ed::StyleVar_NodeRounding, rounding);
        ed::PushStyleVar(ed::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
        ed::PushStyleVar(ed::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
        ed::PushStyleVar(ed::StyleVar_LinkStrength, 0.0f);
        ed::PushStyleVar(ed::StyleVar_PinBorderWidth, 1.0f);
        ed::PushStyleVar(ed::StyleVar_PinRadius, 5.0f);
        ed::BeginNode(node.ID);

        ImGui::BeginVertical(node.ID.AsPointer());
        ImGui::BeginHorizontal("inputs");
        ImGui::Spring(0, padding * 2);

        ImRect inputsRect;
        int inputAlpha = 200;
        if (!node.Inputs.empty()) {
            const auto &pin = node.Inputs[0];
            ImGui::Dummy(ImVec2(0, padding));
            ImGui::Spring(1, 0);
            inputsRect = ImGui_GetItemRect();

            ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
            ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
#if IMGUI_VERSION_NUM > 18101
            ed::PushStyleVar(ed::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);
#else
                        ed::PushStyleVar(ed::StyleVar_PinCorners, 12);
#endif
            ed::BeginPin(pin.ID, ed::PinKind::Input);
            ed::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
            ed::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
            ed::EndPin();
            ed::PopStyleVar(3);

            if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
                inputAlpha = static_cast<int>(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        } else
            ImGui::Dummy(ImVec2(0, padding));

        ImGui::Spring(0, padding * 2);
        ImGui::EndHorizontal();

        ImGui::BeginHorizontal("content_frame");
        ImGui::Spring(1, padding);

        ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
        ImGui::Dummy(ImVec2(160, 0));
        ImGui::Spring(1);
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::Spring(1);
        ImGui::EndVertical();
        auto contentRect = ImGui_GetItemRect();

        ImGui::Spring(1, padding);
        ImGui::EndHorizontal();

        ImGui::BeginHorizontal("outputs");
        ImGui::Spring(0, padding * 2);

        ImRect outputsRect;
        int outputAlpha = 200;
        if (!node.Outputs.empty()) {
            auto &pin = node.Outputs[0];
            ImGui::Dummy(ImVec2(0, padding));
            ImGui::Spring(1, 0);
            outputsRect = ImGui_GetItemRect();

#if IMGUI_VERSION_NUM > 18101
            ed::PushStyleVar(ed::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);
#else
                    ed::PushStyleVar(ed::StyleVar_PinCorners, 3);
#endif
            ed::BeginPin(pin.ID, ed::PinKind::Output);
            ed::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
            ed::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
            ed::EndPin();
            ed::PopStyleVar();

            if (newLinkPin && !CanCreateLink(newLinkPin, &pin) && &pin != newLinkPin)
                outputAlpha = static_cast<int>(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        } else
            ImGui::Dummy(ImVec2(0, padding));

        ImGui::Spring(0, padding * 2);
        ImGui::EndHorizontal();

        ImGui::EndVertical();

        ed::EndNode();
        ed::PopStyleVar(7);
        ed::PopStyleColor(4);

        const auto drawList = ed::GetNodeBackgroundDrawList(node.ID);

#if IMGUI_VERSION_NUM > 18101
        constexpr auto topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
        constexpr auto bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;
#else
                const auto    topRoundCornersFlags = 1 | 2;
                const auto bottomRoundCornersFlags = 4 | 8;
#endif

        drawList->AddRectFilled(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
                                IM_COL32(static_cast<int>(255 * pinBackground.x),
                                         static_cast<int>(255 * pinBackground.y),
                                         static_cast<int>(255 * pinBackground.z), inputAlpha), 4.0f,
                                bottomRoundCornersFlags);
        //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
        drawList->AddRect(inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
                          IM_COL32(static_cast<int>(255 * pinBackground.x), static_cast<int>(255 * pinBackground.y),
                                   static_cast<int>(255 * pinBackground.z), inputAlpha), 4.0f,
                          bottomRoundCornersFlags);
        //ImGui::PopStyleVar();
        drawList->AddRectFilled(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
                                IM_COL32(static_cast<int>(255 * pinBackground.x), static_cast<int>(255 * pinBackground.y),
                                         static_cast<int>(255 * pinBackground.z), outputAlpha), 4.0f,
                                topRoundCornersFlags);
        //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
        drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
                          IM_COL32(static_cast<int>(255 * pinBackground.x), static_cast<int>(255 * pinBackground.y),
                                   static_cast<int>(255 * pinBackground.z), outputAlpha), 4.0f,
                          topRoundCornersFlags);
        //ImGui::PopStyleVar();
        drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(), IM_COL32(24, 64, 128, 200), 0.0f);
        //ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
        drawList->AddRect(
            contentRect.GetTL(),
            contentRect.GetBR(),
            IM_COL32(48, 128, 255, 100), 0.0f);
        //ImGui::PopStyleVar();
    }
    else if (node.Type == NodeType::Comment) {
        constexpr float commentAlpha = 0.75f;

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
        ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
        ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
        ed::BeginNode(node.ID);
        ImGui::PushID(node.ID.AsPointer());
        ImGui::BeginVertical("content");
        ImGui::BeginHorizontal("horizontal");
        ImGui::Spring(1);
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::Spring(1);
        ImGui::EndHorizontal();
        ed::Group(node.Size);
        ImGui::EndVertical();
        ImGui::PopID();
        ed::EndNode();
        ed::PopStyleColor(2);
        ImGui::PopStyleVar();

        if (ed::BeginGroupHint(node.ID)) {
            //auto alpha   = static_cast<int>(commentAlpha * ImGui::GetStyle().Alpha * 255);
            auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

            //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

            const auto min = ed::GetGroupMin();
            //auto max = ed::GetGroupMax();

            ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
            ImGui::BeginGroup();
            ImGui::TextUnformatted(node.Name.c_str());
            ImGui::EndGroup();

            const auto drawList = ed::GetHintBackgroundDrawList();

            auto hintBounds = ImGui_GetItemRect();
            auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

            drawList->AddRectFilled(
                hintFrameBounds.GetTL(),
                hintFrameBounds.GetBR(),
                IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

            drawList->AddRect(
                hintFrameBounds.GetTL(),
                hintFrameBounds.GetBR(),
                IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

            //ImGui::PopStyleVar();
        }
        ed::EndGroupHint();
    }

    ed::Suspend();
    if (do_event_popup) {
        ImGui::OpenPopup("popup_button");
        do_event_popup = false;
    }

    // This is the actual popup Gui drawing section.
    if (ImGui::BeginPopup("popup_button")) {
        // Note: if it weren't for the child window, we would have to PushItemWidth() here to avoid a crash!
        ImGui::TextDisabled("Pick One:");
        ImGui::BeginChild("popup_scroller", ImVec2(100, 100), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        if (ImGui::Button("Wake Up")) {
            ImGui::CloseCurrentPopup();  // These calls revoke the popup open state, which was set by OpenPopup above.
        }
        if (ImGui::Button("Internet Request")) {
            ImGui::CloseCurrentPopup();  // These calls revoke the popup open state, which was set by OpenPopup above.
        }
        ImGui::EndChild();
        ImGui::EndPopup(); // Note this does not do anything to the popup open/close state. It just terminates the content declaration.
    }

    if (do_tooltip)
        ImGui::SetTooltip(node.Description.c_str());

    if (do_tooltip_in_out && !do_tooltip_in_out_desc.empty())
        ImGui::SetTooltip(do_tooltip_in_out_desc.c_str());
    ed::Resume();
}

Pin* Nodes::handle_creation_and_destruction(const Pin* newLinkPin, bool &createNewNode) {
    Pin* newNodeLinkPin = nullptr;
    if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f)) {
        auto showLabel = [](const char *label, ImColor color) {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
            const auto size = ImGui::CalcTextSize(label);
            const auto padding = ImGui::GetStyle().FramePadding;
            const auto spacing = ImGui::GetStyle().ItemSpacing;
            ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));
            const auto rectMin = ImGui::GetCursorScreenPos() - padding;
            const auto rectMax = ImGui::GetCursorScreenPos() + size + padding;
            const auto drawList = ImGui::GetWindowDrawList();
            drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
            ImGui::TextUnformatted(label);
        };

        ed::PinId startPinId = 0, endPinId = 0;
        if (ed::QueryNewLink(&startPinId, &endPinId)) {
            auto startPin = FindPin(startPinId);
            auto endPin = FindPin(endPinId);

            newLinkPin = startPin ? startPin : endPin;

            if (startPin->Kind == PinKind::Input) {
                std::swap(startPin, endPin);
                std::swap(startPinId, endPinId);
            }

            if (startPin && endPin) {
                if (endPin == startPin) {
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (endPin->Kind == startPin->Kind) {
                    showLabel(endPin->Kind == PinKind::Output ?
                        "Cannot connect two output pins together." :
                        "Cannot connect two input pins together.",
                        ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                } else if (endPin->Node == startPin->Node) {
                    showLabel("Cannot connect to self", ImColor(45, 32, 32, 180));
                    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                } else {
                    std::set<const Node*> visited;
                    if (hasPathToNode(endPin->Node, startPin->Node, visited)) {
                        showLabel("Cannot create loop", ImColor(45, 32, 32, 180));
                        ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                    } else if (endPin->Type != startPin->Type && endPin->Type != PinType::Object) {
                        showLabel("Incompatible Pin Type", ImColor(45, 32, 32, 180));
                        ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                    } else if (IsPinLinked(endPin->ID)) {
                        if (ed::AcceptNewItem(ImColor(255, 255, 0), 4.0f)) {
                            ed::DeleteLink(*PinLinkId(endPinId));
                            m_Links.emplace_back(GetNextId(), startPinId, endPinId);
                            m_Links.back().Color = GetIconColor(startPin->Type);
                        }
                        showLabel("Already connected", ImColor(45, 140, 140, 180));
                    } else {
                        showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                        if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
                            m_Links.emplace_back(GetNextId(), startPinId, endPinId);
                        }
                    }
                }
            }
        }

        ed::PinId pinId = 0;
        if (ed::QueryNewNode(&pinId)) {
            newLinkPin = FindPin(pinId);
            if (newLinkPin)
                showLabel("+ Create Node", ImColor(32, 45, 32, 180));

            if (ed::AcceptNewItem()) {
                createNewNode = true;
                newNodeLinkPin = FindPin(pinId);
                newLinkPin = nullptr;
                ed::Suspend();
                ImGui::OpenPopup("Create New Node");
                ed::Resume();
            }
        }
    } else
        newLinkPin = nullptr;

    ed::EndCreate();

    if (ed::BeginDelete()) {
        ed::NodeId nodeId = 0;
        while (ed::QueryDeletedNode(&nodeId)) {
            if (ed::AcceptDeletedItem()) {
                auto id = std::find_if(m_Nodes.begin(), m_Nodes.end(),
                                       [nodeId](auto &node) { return node.ID == nodeId; });
                if (id != m_Nodes.end())
                    m_Nodes.erase(id);
            }
        }

        ed::LinkId linkId = 0;
        while (ed::QueryDeletedLink(&linkId)) {
            if (ed::AcceptDeletedItem()) {
                auto id = std::find_if(m_Links.begin(), m_Links.end(),
                                       [linkId](auto &link) { return link.ID == linkId; });
                if (id != m_Links.end())
                    m_Links.erase(id);
            }
        }
    }
    ed::EndDelete();
    return newNodeLinkPin;
}

auto Nodes::hasPathToNode(const Node* start, const Node* target, std::set<const Node*>& visited) -> bool {
    if (start == target) return true;
    if (visited.find(start) != visited.end()) return false;
    visited.insert(start);

    for (const auto& output : start->Outputs) {
        for (const auto& link : m_Links) {
            if (link.StartPinID == output.ID) {
                const Pin* endPin = FindPin(link.EndPinID);
                if (endPin && hasPathToNode(endPin->Node, target, visited)) {
                    return true;
                }
            }
        }
    }
    return false;
}

int Main(const int argc, char **argv) {
    Skills app("Skills", argc, argv);

    if (app.Create())
        return app.Run();

    return 0;
}
