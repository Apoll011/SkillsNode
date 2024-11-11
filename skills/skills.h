//
// Created by pegasus on 11/4/24.
//
#pragma once

#include <application.h>
#include "utilities/builders.h"
#include "utilities/widgets.h"

#include <imgui_node_editor.h>
#include <imgui_internal.h>

#include "nodes.h"

#ifndef SKILLS_H
#define SKILLS_H

namespace ax::NodeEditor {
    struct EditorContext;
}

static inline ImRect ImGui_GetItemRect() {
    return {ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
}

static inline ImRect ImRect_Expanded(const ImRect &rect, float x, float y) {
    auto result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
}

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

static ed::EditorContext *m_Editor = nullptr;


static bool Splitter(const bool split_vertically, const float thickness, float *size1, float *size2, const float min_size1,
                     const float min_size2, const float splitter_long_axis_size = -1.0f) {
    using namespace ImGui;
    const ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    const ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + CalcItemSize(split_vertically
                                       ? ImVec2(thickness, splitter_long_axis_size)
                                       : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2,
                            0.0f);
}

struct Skills final :
        public Application, public Nodes {
    using Application::Application;

    void OnStart() override;

    void OnStop() override;

    static void ShowStyleEditor(bool *show = nullptr);

    void ShowLeftPane(float paneWidth);

    void KeyBoardHandler();

    void OnFrame(float deltaTime) override;

    void PopupHandler(Pin* newNodeLinkPin, bool &createNewNode);

    ImTextureID m_HeaderBackground = nullptr;
};

inline void Skills::ShowStyleEditor(bool *show) {
    if (!ImGui::Begin("Style", show)) {
        ImGui::End();
        return;
    }

    const auto paneWidth = ImGui::GetContentRegionAvail().x;

    auto &editorStyle = ed::GetStyle();
    ImGui::BeginHorizontal("Style buttons", ImVec2(paneWidth, 0), 1.0f);
    ImGui::TextUnformatted("Values");
    ImGui::Spring();
    if (ImGui::Button("Reset to defaults"))
        editorStyle = ed::Style();
    ImGui::EndHorizontal();
    ImGui::Spacing();
    ImGui::DragFloat4("Node Padding", &editorStyle.NodePadding.x, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Node Rounding", &editorStyle.NodeRounding, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Node Border Width", &editorStyle.NodeBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Hovered Node Border Width", &editorStyle.HoveredNodeBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Hovered Node Border Offset", &editorStyle.HoverNodeBorderOffset, 0.1f, -40.0f, 40.0f);
    ImGui::DragFloat("Selected Node Border Width", &editorStyle.SelectedNodeBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Selected Node Border Offset", &editorStyle.SelectedNodeBorderOffset, 0.1f, -40.0f, 40.0f);
    ImGui::DragFloat("Pin Rounding", &editorStyle.PinRounding, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Pin Border Width", &editorStyle.PinBorderWidth, 0.1f, 0.0f, 15.0f);
    ImGui::DragFloat("Link Strength", &editorStyle.LinkStrength, 1.0f, 0.0f, 500.0f);
    //ImVec2  SourceDirection;
    //ImVec2  TargetDirection;
    ImGui::DragFloat("Scroll Duration", &editorStyle.ScrollDuration, 0.001f, 0.0f, 2.0f);
    ImGui::DragFloat("Flow Marker Distance", &editorStyle.FlowMarkerDistance, 1.0f, 1.0f, 200.0f);
    ImGui::DragFloat("Flow Speed", &editorStyle.FlowSpeed, 1.0f, 1.0f, 2000.0f);
    ImGui::DragFloat("Flow Duration", &editorStyle.FlowDuration, 0.001f, 0.0f, 5.0f);
    //ImVec2  PivotAlignment;
    //ImVec2  PivotSize;
    //ImVec2  PivotScale;
    //float   PinCorners;
    //float   PinRadius;
    //float   PinArrowSize;
    //float   PinArrowWidth;
    ImGui::DragFloat("Group Rounding", &editorStyle.GroupRounding, 0.1f, 0.0f, 40.0f);
    ImGui::DragFloat("Group Border Width", &editorStyle.GroupBorderWidth, 0.1f, 0.0f, 15.0f);

    ImGui::Separator();

    static ImGuiColorEditFlags edit_mode = ImGuiColorEditFlags_DisplayRGB;
    ImGui::BeginHorizontal("Color Mode", ImVec2(paneWidth, 0), 1.0f);
    ImGui::TextUnformatted("Filter Colors");
    ImGui::Spring();
    ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditFlags_DisplayRGB);
    ImGui::Spring(0);
    ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditFlags_DisplayHSV);
    ImGui::Spring(0);
    ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditFlags_DisplayHex);
    ImGui::EndHorizontal();

    static ImGuiTextFilter filter;
    filter.Draw("##filter", paneWidth);

    ImGui::Spacing();

    ImGui::PushItemWidth(-160);
    for (int i = 0; i < ed::StyleColor_Count; ++i) {
        auto name = ed::GetStyleColorName(static_cast<ed::StyleColor>(i));
        if (!filter.PassFilter(name))
            continue;

        ImGui::ColorEdit4(name, &editorStyle.Colors[i].x, edit_mode);
    }
    ImGui::PopItemWidth();

    ImGui::End();
}

inline void Skills::ShowLeftPane(float paneWidth) {
    ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

    paneWidth = ImGui::GetContentRegionAvail().x;

    static bool showStyleEditor = false;
    ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
    ImGui::Spring(0.0f, 0.0f);
    if (ImGui::Button("Zoom to Content"))
        ed::NavigateToContent();
    ImGui::Spring(0.0f);
    if (ImGui::Button("Show Flow")) {
        for (const auto &link: m_Links)
            ed::Flow(link.ID);
    }
    ImGui::Spring();
    if (ImGui::Button("Edit Style"))
        showStyleEditor = true;
    ImGui::EndHorizontal();

    if (showStyleEditor)
        ShowStyleEditor(&showStyleEditor);


    ImGui::BeginHorizontal("Name Editor", ImVec2(paneWidth, 0));
    ImGui::Text("Alex Skill:");
    ImGui::Spring(0.0f);
    static char name[128] = "namespace@skill";

    ImGui::InputText("", name, 127);
    ImGui::EndHorizontal();

    ImGui::BeginHorizontal(R"(Can Repeat Skill execution)", ImVec2(paneWidth, 0));
    ImGui::Text("\tCan Repeat Skill execution: ");       ImGui::NextColumn();
    ImGui::Spring(0.0f);
    static bool can_repeat= true;
    ImGui::Checkbox("", &can_repeat);
    ImGui::EndHorizontal();

    ImGui::BeginHorizontal("Can Repeat Last Sentence", ImVec2(paneWidth, 0));
    ImGui::Text("\tCan Repeat Last Sentence: ");
    ImGui::Spring(0.0f);
    static bool can_repeat_last_sentence = true;
    ImGui::Checkbox("", &can_repeat_last_sentence);
    ImGui::EndHorizontal();

        // Variables Section
    ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
            ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing(); ImGui::SameLine();
    ImGui::TextUnformatted("Variables");
    ImGui::Indent();
    static char varName[64] = "";
    static int varTypeIndex = 0;
    const char *varTypes[] = {"Bool", "Int", "Float", "String", "List", "Dictionary", "Object"};

    if (ImGui::Button("Add Variable", ImVec2(paneWidth*0.9, 25))) {
        ImGui::OpenPopup("AddVariable");
    }

    if (ImGui::BeginPopup("AddVariable")) {
        ImGui::InputText("Name", varName, sizeof(varName));
        ImGui::Combo("Type", &varTypeIndex, varTypes, IM_ARRAYSIZE(varTypes));

        if (ImGui::Button("Create") && strlen(varName) > 0) {
            bool can_go = true;
            for (Variable var: m_Variables)
                if (var.name == varName)
                    can_go = false;
            for (Function func: m_Functions)
                if (func.name == varName)
                    can_go = false;
            PinType type = {};
            switch (varTypeIndex) {
                case 0: type = PinType::Bool;
                    break;
                case 1: type = PinType::Int;
                    break;
                case 2: type = PinType::Float;
                    break;
                case 3: type = PinType::String;
                    break;
                case 4: type = PinType::List;
                    break;
                case 5: type = PinType::Dictionary;
                    break;
                case 6: type = PinType::Object;
                    break;
                default: type = PinType::String;
            }
            if (can_go)
                m_Variables.push_back({varName, type});

            varName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }


    for (const auto& var : m_Variables) {
        switch(var.type) {
            case PinType::Bool: ImGui::Text((var.name + " (Boolean)").c_str()); break;
            case PinType::Int: ImGui::Text((var.name + " (Integer)").c_str()); break;
            case PinType::Float: ImGui::Text((var.name + " (Float)").c_str());  break;
            case PinType::String: ImGui::Text((var.name + " (String)").c_str()); break;
            case PinType::List: ImGui::Text((var.name + " (List)").c_str()); break;
            case PinType::Dictionary: ImGui::Text((var.name + " (Dictionary)").c_str()); break;
            default: ImGui::Text((var.name + " (Unknown)").c_str());;
        }
    }
    ImGui::Unindent();

    // Functions Section
    ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
            ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing(); ImGui::SameLine();
    ImGui::TextUnformatted("Functions");

    ImGui::Indent();

    static char funcName[64] = "";
    static int funcReturnTypeIndex = 0;
    const char *funcReturnTypes[] = {"None", "Bool", "Int", "Float", "String", "List", "Dictionary", "Object"};
    static int argsTypeIndex = 0;
    const char *argsTypes[] = {"Bool", "Int", "Float", "String", "List", "Dictionary", "Object"};
    static std::vector<Pin> parameters;

    if (ImGui::Button("Add Function", ImVec2(paneWidth*0.9, 25))) {
        ImGui::OpenPopup("AddFunction");
    }

    if (ImGui::BeginPopup("AddFunction")) {
        Function func;

        ImGui::InputText("Name", funcName, sizeof(funcName));
        ImGui::Combo("Return Type", &funcReturnTypeIndex, funcReturnTypes, IM_ARRAYSIZE(funcReturnTypes));
        func.name = funcName;

        ImGui::Indent();
        for (Pin p: parameters) {
            ImGui::Text(p.Name.c_str());
        }
        ImGui::Unindent();

        if (ImGui::Button("Add Arg")) {
            ImGui::OpenPopup("AddArg");
        }

        if (ImGui::BeginPopup("AddArg")) {
            static char argsName[64] = "";
            static bool can_ask = false;
            ImGui::InputText("Name", argsName, sizeof(argsName));
            ImGui::Checkbox("Have default value", &can_ask);
            ImGui::Combo("Type", &argsTypeIndex, argsTypes, IM_ARRAYSIZE(argsTypes));
            if (ImGui::Button("Create") && strlen(argsName) > 0) {
                PinType type = {};
                switch (argsTypeIndex) {
                    case 0: type = PinType::Bool;
                        break;
                    case 1: type = PinType::Int;
                        break;
                    case 2: type = PinType::Float;
                        break;
                    case 3: type = PinType::String;
                        break;
                    case 4: type = PinType::List;
                        break;
                    case 5: type = PinType::Dictionary;
                        break;
                    case 6: type = PinType::Object;
                        break;
                    default: type = PinType::String;
                }
                Pin new_arg = {0, argsName, type, can_ask};
                parameters.emplace_back(new_arg);
                argsName[0] = '\0';
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::Button("Create") && strlen(funcName) > 0) {
            bool can_go = true;
            for (Variable var: m_Variables)
                if (var.name == funcName)
                    can_go = false;
            for (Function func: m_Functions)
                if (func.name == funcName)
                    can_go = false;
            PinType type = {};
            switch (funcReturnTypeIndex) {
                case 0: type = PinType::None;
                    break;
                case 1: type = PinType::Bool;
                    break;
                case 2: type = PinType::Int;
                    break;
                case 3: type = PinType::Float;
                    break;
                case 4: type = PinType::String;
                    break;
                case 5: type = PinType::List;
                    break;
                case 6: type = PinType::Dictionary;
                    break;
                case 7: type = PinType::Object;
                    break;
                default: type = PinType::None;
            }

            func.returnType = type;
            func.parameters = parameters;
            if (can_go)
                m_Functions.push_back(func);

            funcName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }


    for (const auto& func : m_Functions) {
        switch(func.returnType) {
            case PinType::None: ImGui::Text(func.name.c_str()); break;
            case PinType::Bool: ImGui::Text((func.name + " (Boolean)").c_str()); break;
            case PinType::Int: ImGui::Text((func.name + " (Integer)").c_str()); break;
            case PinType::Float: ImGui::Text((func.name + " (Float)").c_str());  break;
            case PinType::String: ImGui::Text((func.name + " (String)").c_str()); break;
            case PinType::List: ImGui::Text((func.name + " (List)").c_str()); break;
            case PinType::Dictionary: ImGui::Text((func.name + " (Dictionary)").c_str()); break;
            default: ImGui::Text((func.name + " (Unknown)").c_str());;
        }
    }
    ImGui::Unindent();

    ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
            ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing(); ImGui::SameLine();
    ImGui::TextUnformatted("Configurations");

    ImGui::GetWindowDrawList()->AddRectFilled(
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
            ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
    ImGui::Spacing(); ImGui::SameLine();
    ImGui::TextUnformatted("Assets");

    KeyBoardHandler();

    ImGui::EndChild();
}

inline void Skills::KeyBoardHandler() {
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F)))
        for (const auto &link: m_Links)
            ed::Flow(link.ID);

    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Z)))
        ed::NavigateToContent();
}


#endif //SKILLS_H
