#include "nodes.h"

#include <iostream>

unsigned long Nodes::GetNextId() {
    return m_NextId++;
}

ed::LinkId Nodes::GetNextLinkId() {
    return {GetNextId()};
}

void Nodes::TouchNode(const ed::NodeId id) {
    m_NodeTouchTime[id] = m_TouchTime;
}

float Nodes::GetTouchProgress(const ed::NodeId id) {
    auto it = m_NodeTouchTime.find(id);
    if (it != m_NodeTouchTime.end() && it->second > 0.0f)
        return (m_TouchTime - it->second) / m_TouchTime;
    else
        return 0.0f;
}

void Nodes::UpdateTouch() {
    const auto deltaTime = ImGui::GetIO().DeltaTime;
    for (auto &entry: m_NodeTouchTime) {
        if (entry.second > 0.0f)
            entry.second -= deltaTime;
    }
}

Node *Nodes::FindNode(const ed::NodeId id) {
    for (auto &node: m_Nodes)
        if (node.ID == id)
            return &node;

    return nullptr;
}

Link *Nodes::FindLink(const ed::LinkId id) {
    for (auto &link: m_Links)
        if (link.ID == id)
            return &link;

    return nullptr;
}

Pin *Nodes::FindPin(const ed::PinId id) {
    if (!id)
        return nullptr;

    for (auto &node: m_Nodes) {
        for (auto &pin: node.Inputs)
            if (pin.ID == id)
                return &pin;

        for (auto &pin: node.Outputs)
            if (pin.ID == id)
                return &pin;
    }

    return nullptr;
}

bool Nodes::IsPinLinked(const ed::PinId id) {
    if (!id)
        return false;

    for (auto &link: m_Links)
        if (link.StartPinID == id || link.EndPinID == id)
            return true;

    return false;
}

ed::LinkId *Nodes::PinLinkId(const ed::PinId id) {
    if (!id)
        return nullptr;

    for (auto &link: m_Links) {
        if (link.StartPinID == id)
            return &link.ID;
        if (link.EndPinID == id)
            return &link.ID;
    }

    return nullptr;
}

bool Nodes::CanCreateLink(const Pin *a, const Pin *b) {
    if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
        return false;

    return true;
}

void Nodes::BuildNode(Node *node) {
    for (auto &input: node->Inputs) {
        input.Node = node;
        input.Kind = PinKind::Input;
    }

    for (auto &output: node->Outputs) {
        output.Node = node;
        output.Kind = PinKind::Output;
    }
}

void Nodes::BuildNodes() {
    for (auto &node: m_Nodes)
        BuildNode(&node);
}

ImColor Nodes::GetIconColor(PinType type) {
    switch (type) {
        default:
        case PinType::None: return ImColor(70, 70, 70);
        case PinType::Flow: return ImColor(255, 255, 255);
        case PinType::Bool: return ImColor(220, 48, 48);
        case PinType::Int: return ImColor(68, 201, 156);
        case PinType::Float: return ImColor(156, 226, 68);
        case PinType::String: return ImColor(124, 21, 153);
        case PinType::Dictionary: return ImColor(32, 200, 255);
        case PinType::List: return ImColor(196, 255, 150);
        case PinType::Object: return ImColor(51, 150, 215);
        case PinType::Function: return ImColor(255, 48, 48);
        case PinType::Delegate: return ImColor(218, 0, 183);
        case PinType::UserResponse: return ImColor(5, 255, 255);
        case PinType::Vector: return ImColor(128, 195, 255);
        case PinType::Rotator: return ImColor(255, 156, 0);
        case PinType::Transform: return ImColor(180, 120, 255);
        case PinType::Enum: return ImColor(185, 185, 0);
        case PinType::Struct: return ImColor(220, 150, 50);
        case PinType::Set: return ImColor(0, 175, 175);
        case PinType::Event: return ImColor(255, 100, 100);
    }
};

void Nodes::DrawPinIcon(const Pin &pin, const bool connected, const int alpha) const {
    IconType iconType;
    ImColor color = GetIconColor(pin.Type);
    color.Value.w = alpha / 255.0f;
    switch (pin.Type) {
        case PinType::None: return;
        case PinType::Flow: iconType = IconType::Flow; break;
        case PinType::Bool: iconType = IconType::Circle; break;
        case PinType::Int: iconType = IconType::Circle; break;
        case PinType::Float: iconType = IconType::Circle; break;
        case PinType::String: iconType = IconType::Circle; break;
        case PinType::List: iconType = IconType::Diamond; break;
        case PinType::Dictionary: iconType = IconType::RoundSquare; break;
        case PinType::UserResponse: iconType = IconType::RoundSquare; break;
        case PinType::Event: iconType = IconType::Arrow; break;
        case PinType::Object: iconType = IconType::Grid; break;
        case PinType::Function: iconType = IconType::Square; break;
        case PinType::Delegate: iconType = IconType::Grid; break;
        case PinType::Vector: iconType = IconType::Square; break;
        case PinType::Rotator: iconType = IconType::Circle; break;
        case PinType::Transform: iconType = IconType::Diamond; break;
        case PinType::Enum: iconType = IconType::Square; break;
        case PinType::Struct: iconType = IconType::RoundSquare; break;
        case PinType::Set: iconType = IconType::Diamond; break;
        default: return;
    }


    ax::Widgets::Icon(ImVec2(static_cast<float>(m_PinIconSize), static_cast<float>(m_PinIconSize)), iconType, connected,
                      color, ImColor(32, 32, 32, alpha));
};

Node *Nodes::generate_node_context_menu_generate(const std::string& label, const NodeCategory category) {
    Node* node = nullptr;
    if (!m_NodeCategory[category].empty() && ImGui::BeginMenu(label.c_str())) {
        for (auto &name: m_NodeCategory[category])
            if (ImGui::MenuItem(name.c_str()))
            {
                const std::function<Node*()> creator = m_NodeCreators[name];
                node = creator();
            }
        ImGui::EndMenu();
    }
    return node;
}

Node* Nodes::choice() {
    static char searchBuffer[256] = "";
    Node* node = nullptr;
    ImGui::PushID("CreateNodeMenu");

    // Search bar at the top
    ImGui::PushItemWidth(-1);
    bool searchChanged = ImGui::InputText("##NodeSearch", searchBuffer, sizeof(searchBuffer),
        ImGuiInputTextFlags_AutoSelectAll);
    ImGui::PopItemWidth();

    ImGui::Separator();

    auto searchStr = std::string(searchBuffer);
    std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);

    // Helper function to check if a string matches the search
    auto matchesSearch = [&searchStr](const std::string& text) {
        if (searchStr.empty()) return true;
        std::string lower = text;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return lower.find(searchStr) != std::string::npos;
    };

    // Calculate available content region height
    float remainingHeight = ImGui::GetContentRegionAvail().y;
    ImGui::BeginChild("NodeCategories", ImVec2(600, 300), false);

    // Display categories and their nodes
    std::map<NodeCategory, bool> hasVisibleNodes;

    // First pass: determine which categories have visible nodes
    for (const auto& categoryPair : m_NodeCategory) {
        bool categoryHasVisibleNodes = false;
        for (const auto& nodeName : categoryPair.second) {
            if (matchesSearch(nodeName) || searchStr.empty()) {
                categoryHasVisibleNodes = true;
                break;
            }
        }
        hasVisibleNodes[categoryPair.first] = categoryHasVisibleNodes;
    }

    // Second pass: display categories with visible nodes
    for (const auto& categoryPair : m_NodeCategory) {
        if (!hasVisibleNodes[categoryPair.first]) continue;

        // Get category name for display
        std::string categoryName = category_to_string(categoryPair.first);

        if (ImGui::CollapsingHeader(categoryName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent(10.0f);
            for (const auto& nodeName : categoryPair.second) {
                if (matchesSearch(nodeName)) {
                    if (ImGui::Selectable(nodeName.c_str())) {
                        const std::function<Node*()> creator = m_NodeCreators[nodeName];
                        node = creator();
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
            ImGui::Unindent(10.0f);
        }
    }

    // Variables section
    if (!m_Variables.empty()) {
        bool hasVisibleVars = false;
        for (const auto& var : m_Variables) {
            if (matchesSearch(var.name)) {
                hasVisibleVars = true;
                break;
            }
        }

        if (hasVisibleVars && ImGui::CollapsingHeader("Variables", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent(10.0f);
            for (const auto& var : m_Variables) {
                if (matchesSearch(var.name)) {
                    if (ImGui::Selectable(("Get Value of " + var.name).c_str())) {
                        node = new Node(GetNextId(), "Get " + var.name, ImColor(255, 10, 0));
                        node->Outputs.emplace_back(GetNextId(), "Value", var.type);
                        ImGui::CloseCurrentPopup();
                    }
                    if (ImGui::Selectable(("Set Value of " + var.name).c_str())) {
                        node = new Node(GetNextId(), "Set " + var.name, ImColor(0, 10, 255));
                        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
                        node->Inputs.emplace_back(GetNextId(), "Value", var.type, true);
                        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
                        ImGui::CloseCurrentPopup();
                    }

                }
            }
            ImGui::Unindent(10.0f);
        }
    }

    // Functions section
    if (!m_Functions.empty()) {
        bool hasVisibleFuncs = false;
        for (const auto& func : m_Functions) {
            if (matchesSearch(func.name)) {
                hasVisibleFuncs = true;
                break;
            }
        }

        if (hasVisibleFuncs && ImGui::CollapsingHeader("Functions", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent(10.0f);
            for (const auto& func : m_Functions) {
                if (matchesSearch(func.name)) {
                    if (ImGui::Selectable(("Execute " + func.name).c_str())) {
                        node = new Node(GetNextId(), "Execute " + func.name, ImColor(255, 10, 0));
                        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
                        for (Pin arg: func.parameters) {
                            arg.ID = GetNextId();
                            node->Inputs.emplace_back(arg);
                        }
                        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
                        if (func.returnType != PinType::None)
                            node->Outputs.emplace_back(GetNextId(), "Return Value", func.returnType);
                        ImGui::CloseCurrentPopup();
                    }

                   bool can_func = true;
                   for (Node nd: m_Nodes)
                       if (nd.Name == "Function " + func.name)
                           can_func = false;

                   if (can_func && ImGui::Selectable(("Define " + func.name).c_str())) {
                       node = new Node(GetNextId(), "Function " + func.name, ImColor(0, 10, 255));
                       node->Outputs.emplace_back(GetNextId(), "Function Obj", PinType::Function);
                       node->Outputs.emplace_back(GetNextId(), "Entry", PinType::Flow);
                       for (Pin arg: func.parameters) {
                           arg.Kind = PinKind::Output;
                           arg.ID = GetNextId();
                           node->Outputs.emplace_back(arg);
                       }
                       ImGui::CloseCurrentPopup();
                   }

                }
            }
            ImGui::Unindent(10.0f);
        }
    }

    ImGui::EndChild();
    ImGui::PopID();

    return node;
}

void Nodes::create_node(Node* node, const ImVec2 newNodePosition, bool& createNewNode, Pin* newNodeLinkPin) {
    if (node) {
        m_Nodes.emplace_back(*node);
        BuildNodes();

        createNewNode = false;

        ed::SetNodePosition(node->ID, newNodePosition);

        if (auto startPin = newNodeLinkPin) {
            auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

            for (auto& pin : pins) {
                if (CanCreateLink(startPin, &pin)) {
                    auto endPin = &pin;
                    if (startPin->Kind == PinKind::Input)
                        std::swap(startPin, endPin);

                    m_Links.emplace_back(GetNextId(), startPin->ID, endPin->ID);

                    break;
                }
            }
        }
    }
}

void Nodes::RegisterNode(const char* name, const NodeCategory category, const std::function<Node*()>& createFunction) {
    m_NodeCreators[name] = [createFunction, category] {
        Node* node = createFunction();
        node->Category = category;
        node->Color = category_color(category);
        return node;
    };
    if (strcmp(name, "Comment") == 0) {
        return;
    }
    m_NodeCategory[category].emplace_back(name);

}