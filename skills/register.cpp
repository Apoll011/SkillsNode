//
// Created by pegasus on 11/11/24.
//
#include "nodes.h"

void Nodes::CreateNodes() {
    RegisterNode("Execute", NodeCategory::Event, [this] {
        const auto node = new Node(GetNextId(), "Execute",
                                   "This function is called when this skill is requested it should have exactly one on the scope.");
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("Optional", NodeCategory::SlotManagement, [this] {
        const auto node = new Node(GetNextId(), "Optional",
                                   "Optional slots - Initialize the slot and won't raise if missing");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Slot Name", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("Required", NodeCategory::SlotManagement, [this] {
        const auto node = new Node(GetNextId(), "Required",
                                   "Required slots - Initialize the slot and will raise SkillSlotNotFound if missing",
                                   ImColor(230, 95, 48));
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Slot Name", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("Get", NodeCategory::SlotManagement, [this] {
        const auto node = new Node(GetNextId(), "Get",
                                   "Get processed value or Get raw input without processing like 'one' instead of 1",
                                   ImColor(230, 95, 48));
        node->Inputs.emplace_back(GetNextId(), "Slot Name", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Raw", PinType::Bool, true);
        node->Outputs.emplace_back(GetNextId(), "Value", PinType::Object);
        return node;
    });

    RegisterNode("Slot Exists", NodeCategory::SlotManagement, [this] {
        const auto node = new Node(GetNextId(), "Slot Exists", "Checks if a given slot name exists");
        node->Inputs.emplace_back(GetNextId(), "Slot Name", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Exits", PinType::Bool);
        return node;
    });

    RegisterNode("Assert Equal", NodeCategory::SlotManagement, [this] {
        const auto node = new Node(GetNextId(), "Assert Equal", "Check if the slot value equals the Text");
        node->Inputs.emplace_back(GetNextId(), "Slot Name", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Text", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Is Equal", PinType::Bool);
        return node;
    });

    RegisterNode("Assert In", NodeCategory::SlotManagement, [this] {
        const auto node = new Node(GetNextId(), "Assert In", "Check if the slot value is in the List",
                                   ImColor(230, 95, 48));
        node->Inputs.emplace_back(GetNextId(), "Slot Name", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "List", PinType::List);
        node->Outputs.emplace_back(GetNextId(), "Is In", PinType::Bool);
        return node;
    });

    RegisterNode("Assert In Dictionary", NodeCategory::SlotManagement, [this] {
        const auto node = new Node(GetNextId(), "Assert In Dictionary",
                                   "Check if the slot value is in the Dictionary as a Key");
        node->Inputs.emplace_back(GetNextId(), "Slot Name", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Dictionary", PinType::Dictionary);
        node->Outputs.emplace_back(GetNextId(), "Is In Dictionary", PinType::Bool);
        return node;
    });

    RegisterNode("Lengths of Slots", NodeCategory::SlotManagement, [this] {
        const auto node = new Node(GetNextId(), "Assert In Dictionary",
                                   ImColor(230, 95, 48));
        node->Outputs.emplace_back(GetNextId(), "Length", PinType::Int);
        return node;
    });

    RegisterNode("Response", NodeCategory::Response, [this] {
        const auto node = new Node(GetNextId(), "Response", "Makes Alex say the raw text");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Text", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Size = ImVec2(300, 200);
        return node;
    });

    RegisterNode("Say", NodeCategory::Response, [this] {
        const auto node = new Node(GetNextId(), "Say",
                                   "Makes Alex say the translated text with a given replacer (Context)");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Key To Translation", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Translation Context", PinType::Dictionary);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("Question", NodeCategory::User, [this] {
        const auto node = new Node(GetNextId(), "Question",
                                   "Says the given translated text and executes the function after receiving a value");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Key Translation", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Translation Replacers", PinType::Dictionary);
        node->Inputs.emplace_back(GetNextId(), "Function", PinType::Function);
        node->Inputs.emplace_back(GetNextId(), "Function Args", PinType::List);
        node->Inputs.emplace_back(GetNextId(), "Expected", PinType::UserResponse);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("On Next Input", NodeCategory::User, [this] {
        const auto node = new Node(GetNextId(), "On Next Input",
                                   ImColor(240, 220, 95));
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Function", PinType::Function);
        node->Inputs.emplace_back(GetNextId(), "Function Args", PinType::List);
        node->Inputs.emplace_back(GetNextId(), "Expected", PinType::UserResponse);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("Something From List or None Response", NodeCategory::User, [this] {
        const auto node = new Node(GetNextId(), "Something From List or None Response", ImColor(240, 220, 95));
        node->Inputs.emplace_back(GetNextId(), "List of Possible Responses", PinType::List);
        node->Outputs.emplace_back(GetNextId(), "response", PinType::UserResponse);
        return node;
    });

    std::vector<std::string> responses = {"Bool Response", "Any Response", "Something or None Response"};

    for (const std::string &response: responses) {
        RegisterNode(response.c_str(), NodeCategory::User, [this, response] {
            const auto node = new Node(GetNextId(), response.c_str());
            node->Outputs.emplace_back(GetNextId(), "response", PinType::UserResponse);
            return node;
        });
    }

    RegisterNode("Request Attention", NodeCategory::User, [this] {
        const auto node = new Node(GetNextId(), "Request Attention",
                                   "Says the user name to alert him of something.",
                                   ImColor(240, 220, 95));
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("Save Context", NodeCategory::Context, [this] {
        const auto node = new Node(GetNextId(), "Save Context");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Context Name", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("Load Context", NodeCategory::Context, [this] {
        const auto node = new Node(GetNextId(), "Load Context");
        node->Inputs.emplace_back(GetNextId(), "Context Name", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Object", PinType::Object);
        return node;
    });

    RegisterNode("Get Assets", NodeCategory::Assets, [this] {
        const auto node = new Node(GetNextId(), "Get Assets");
        node->Inputs.emplace_back(GetNextId(), "Asset Name", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Path", PinType::String);
        return node;
    });

    RegisterNode("Play Audio", NodeCategory::Assets, [this] {
        const auto node = new Node(GetNextId(), "Play Audio");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Audio Path", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("Get Configuration", NodeCategory::Configuration, [this] {
        const auto node = new Node(GetNextId(), "Get Configuration");
        node->Inputs.emplace_back(GetNextId(), "Config Name", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Value", PinType::Object);
        return node;
    });

    RegisterNode("Get Setting", NodeCategory::Configuration, [this] {
        const auto node = new Node(GetNextId(), "Get Setting");
        node->Inputs.emplace_back(GetNextId(), "Setting Name", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Value", PinType::Object);
        return node;
    });

    RegisterNode("Save Setting", NodeCategory::Configuration, [this] {
        const auto node = new Node(GetNextId(), "Get Setting");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Setting Name", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Value", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });


    RegisterNode("To Str", NodeCategory::Conversion, [this] {
        const auto node = new Node(GetNextId(), "To Str");
        node->Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "String", PinType::String);
        return node;
    });

    RegisterNode("To Int", NodeCategory::Conversion, [this] {
        const auto node = new Node(GetNextId(), "To Int");
        node->Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "Int", PinType::Int);
        return node;
    });

    RegisterNode("To Float", NodeCategory::Conversion, [this] {
        const auto node = new Node(GetNextId(), "To Float");
        node->Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "Float", PinType::Float);
        return node;
    });

    RegisterNode("To Bool", NodeCategory::Conversion, [this] {
        const auto node = new Node(GetNextId(), "To Bool");
        node->Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "Bool", PinType::Bool);
        return node;
    });

    RegisterNode("Not None", NodeCategory::Data, [this] {
        const auto node = new Node(GetNextId(), "Not None");
        node->Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "Bool", PinType::Bool);
        node->Size = ImVec2(300, 200);
        return node;
    });

    RegisterNode("Is None", NodeCategory::Data, [this] {
        const auto node = new Node(GetNextId(), "Is None");
        node->Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "Bool", PinType::Bool);
        node->Size = ImVec2(300, 200);
        return node;
    });

    RegisterNode("Register Event", NodeCategory::Event, [this] {
        const auto node = new Node(GetNextId(), "Register Event",
                                   "WIll register the selected event in the system information");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Event", PinType::Event, true);
        return node;
    });

    RegisterNode("Get Translation", NodeCategory::Response, [this] {
        const auto node = new Node(GetNextId(), "Get Translation",
                                   "WIll register the selected event in the system information");
        node->Inputs.emplace_back(GetNextId(), "Key To Translation", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Translation Context", PinType::Dictionary);
        node->Inputs.emplace_back(GetNextId(), "Return None", PinType::Bool, true);
        node->Inputs.emplace_back(GetNextId(), "Fallback", PinType::String, true);
        node->Outputs.emplace_back(GetNextId(), "Translated String", PinType::String);
        return node;
    });


    RegisterNode("Comment", NodeCategory::Custom, [this] {
        const auto node = new Node(GetNextId(), "Test Comment");
        node->Type = NodeType::Comment;
        return node;
    });

    RegisterNode("None", NodeCategory::Data, [this] {
        const auto node = new Node(GetNextId(), "None");
        node->Type = NodeType::Simple;
        node->Outputs.emplace_back(GetNextId(), "", PinType::Object);
        return node;
    });


    RegisterNode("String", NodeCategory::Data, [this] {
        const auto node = new Node(GetNextId(), "");
        node->Type = NodeType::Simple;
        node->Outputs.emplace_back(GetNextId(), "String", PinType::String, true);
        return node;
    });

    RegisterNode("Integer", NodeCategory::Data, [this] {
        const auto node = new Node(GetNextId(), "");
        node->Type = NodeType::Simple;
        node->Outputs.emplace_back(GetNextId(), "Integer", PinType::Int, true);
        return node;
    });

    RegisterNode("Bool", NodeCategory::Data, [this] {
        const auto node = new Node(GetNextId(), "");
        node->Type = NodeType::Simple;
        node->Outputs.emplace_back(GetNextId(), "Bool", PinType::Bool, true);
        return node;
    });


    RegisterNode("Float", NodeCategory::Data, [this] {
        const auto node = new Node(GetNextId(), "");
        node->Type = NodeType::Simple;
        node->Outputs.emplace_back(GetNextId(), "Float", PinType::Float, true);
        return node;
    });

    RegisterNode("List", NodeCategory::Data, [this] {
        const auto node = new Node(GetNextId(), "");
        node->Type = NodeType::Simple;
        node->Outputs.emplace_back(GetNextId(), "List", PinType::List, true);
        return node;
    });

    RegisterNode("Dictionary", NodeCategory::Data, [this] {
        const auto node = new Node(GetNextId(), "");
        node->Type = NodeType::Simple;
        node->Outputs.emplace_back(GetNextId(), "Dictionary", PinType::Dictionary, true);
        return node;
    });

    RegisterNode("Language", NodeCategory::Configuration, [this] {
        const auto node = new Node(GetNextId(), "Language");
        node->Outputs.emplace_back(GetNextId(), "Current Language", PinType::String);
        return node;
    });

    RegisterNode("Working Directory", NodeCategory::Configuration, [this] {
        const auto node = new Node(GetNextId(), "Working Directory");
        node->Outputs.emplace_back(GetNextId(), "Current Directory", PinType::String);
        return node;
    });

    RegisterNode("If", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "If",
                                   ImColor(255, 9, 20));
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "True", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "False", PinType::Flow);

        return node;
    });

    RegisterNode("For", NodeCategory::Loop, [this] {
        const auto node = new Node(GetNextId(), "For",
                                   ImColor(255, 9, 20));
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Iterable", PinType::List);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Loop", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Value", PinType::Object);

        return node;
    });

    RegisterNode("While", NodeCategory::Loop, [this] {
        const auto node = new Node(GetNextId(), "While",
                                   ImColor(255, 9, 20));
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
        node->Inputs.emplace_back(GetNextId(), "At least Once", PinType::Bool, true);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Loop", PinType::Flow);
        return node;
    });

    RegisterNode("Break", NodeCategory::Loop, [this] {
        const auto node = new Node(GetNextId(), "Break");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("Continue", NodeCategory::Loop, [this] {
        const auto node = new Node(GetNextId(), "Continue");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("Return", NodeCategory::Functions, [this] {
        const auto node = new Node(GetNextId(), "Return");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Object", PinType::Object);

        return node;
    });

    // -, +, *, /, **
    responses = {"+", "-", "x", "/", "^"};

    for (const std::string &response: responses) {
        RegisterNode(response.c_str(), NodeCategory::Math, [this, response] {
            const auto node = new Node(GetNextId(), response.c_str());
            node->Type = NodeType::Simple;
            node->Inputs.emplace_back(GetNextId(), "", PinType::Object);
            node->Inputs.emplace_back(GetNextId(), "", PinType::Object);
            node->Outputs.emplace_back(GetNextId(), "", PinType::Object);
            return node;
        });
    }

    responses = {"==", "!=", ">=", "<=", ">", "<"};

    for (const std::string &response: responses) {
        RegisterNode(response.c_str(), NodeCategory::Math, [this, response] {
            const auto node = new Node(GetNextId(), response.c_str());
            node->Type = NodeType::Simple;
            node->Inputs.emplace_back(GetNextId(), "", PinType::Object);
            node->Inputs.emplace_back(GetNextId(), "", PinType::Object);
            node->Outputs.emplace_back(GetNextId(), "", PinType::Bool);
            return node;
        });
    }

    RegisterNode("NOT", NodeCategory::Data, [this] {
        const auto node = new Node(GetNextId(), "NOT");
        node->Type = NodeType::Simple;
        node->Inputs.emplace_back(GetNextId(), "", PinType::Bool);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Bool);
        return node;
    });

    responses = {"AND", "OR", "XOR", "NAND"};

    for (const std::string &response: responses) {
        RegisterNode(response.c_str(), NodeCategory::Math, [this, response] {
            const auto node = new Node(GetNextId(), response.c_str());
            node->Type = NodeType::Simple;
            node->Inputs.emplace_back(GetNextId(), "", PinType::Bool);
            node->Inputs.emplace_back(GetNextId(), "", PinType::Bool);
            node->Outputs.emplace_back(GetNextId(), "", PinType::Bool);
            return node;
        });
    }

    RegisterNode("Left Shift", NodeCategory::Math, [this] {
        const auto node = new Node(GetNextId(), "Left shift");
        node->Type = NodeType::Simple;
        node->Inputs.emplace_back(GetNextId(), "Value", PinType::Int);
        node->Inputs.emplace_back(GetNextId(), "Amount", PinType::Int);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Int);
        return node;
    });

    RegisterNode("Right Shift", NodeCategory::Math, [this] {
        const auto node = new Node(GetNextId(), "Right shift");
        node->Type = NodeType::Simple;
        node->Inputs.emplace_back(GetNextId(), "Value", PinType::Int);
        node->Inputs.emplace_back(GetNextId(), "Amount", PinType::Int);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Int);
        return node;
    });

    RegisterNode("Try Block", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Try Block");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Try", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Exception", PinType::Flow);
        // Make Extensive chain of exceptions allowing user to make its own.
        node->Outputs.emplace_back(GetNextId(), "err output", PinType::Object);

        return node;
    });

    RegisterNode("Lambda", NodeCategory::Functions, [this] {
        const auto node = new Node(GetNextId(), "Lambda");
        node->Inputs.emplace_back(GetNextId(), "Parameters", PinType::List);
        node->Outputs.emplace_back(GetNextId(), "Body", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Function", PinType::Function);
        return node;
    });

    RegisterNode("Async", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Async");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Async Block", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Promise", PinType::Object);
        return node;
    });

    RegisterNode("Await", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Await");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Promise", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Result", PinType::Object);
        return node;
    });

    RegisterNode("Map", NodeCategory::Array, [this] {
        const auto node = new Node(GetNextId(), "Map");
        node->Inputs.emplace_back(GetNextId(), "Array", PinType::List);
        node->Inputs.emplace_back(GetNextId(), "Function", PinType::Function);
        node->Outputs.emplace_back(GetNextId(), "Result", PinType::List);
        return node;
    });

    RegisterNode("Filter", NodeCategory::Array, [this] {
        const auto node = new Node(GetNextId(), "Filter");
        node->Inputs.emplace_back(GetNextId(), "Array", PinType::List);
        node->Inputs.emplace_back(GetNextId(), "Predicate", PinType::Function);
        node->Outputs.emplace_back(GetNextId(), "Filtered", PinType::List);
        return node;
    });

    RegisterNode("Split", NodeCategory::Array, [this] {
        const auto node = new Node(GetNextId(), "Split");
        node->Inputs.emplace_back(GetNextId(), "Array", PinType::List);
        node->Inputs.emplace_back(GetNextId(), "Index", PinType::Int);
        node->Outputs.emplace_back(GetNextId(), "Result", PinType::List);
        return node;
    });

    RegisterNode("Yield", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Yield");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Value", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("HTTP Request", NodeCategory::Networking, [this] {
        const auto node = new Node(GetNextId(), "HTTP Request");
        node->Inputs.emplace_back(GetNextId(), "URL", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Method", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Headers", PinType::Dictionary);
        node->Inputs.emplace_back(GetNextId(), "Body", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "Response", PinType::Object);
        return node;
    });

    RegisterNode("WebSocket", NodeCategory::Networking, [this] {
        const auto node = new Node(GetNextId(), "WebSocket");
        node->Inputs.emplace_back(GetNextId(), "URL", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "OnMessage", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "OnClose", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Message", PinType::Object);
        return node;
    });

    RegisterNode("Regex", NodeCategory::String, [this] {
        const auto node = new Node(GetNextId(), "Regex");
        node->Inputs.emplace_back(GetNextId(), "Pattern", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Text", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Matches", PinType::List);
        node->Outputs.emplace_back(GetNextId(), "Success", PinType::Bool);
        return node;
    });

    RegisterNode("Timer", NodeCategory::Time, [this] {
        const auto node = new Node(GetNextId(), "Timer");
        node->Inputs.emplace_back(GetNextId(), "Interval", PinType::Float);
        node->Inputs.emplace_back(GetNextId(), "Repeat", PinType::Bool);
        node->Outputs.emplace_back(GetNextId(), "OnTick", PinType::Flow);
        return node;
    });

    RegisterNode("Stream", NodeCategory::Data, [this] {
        const auto node = new Node(GetNextId(), "Stream");
        node->Inputs.emplace_back(GetNextId(), "Source", PinType::Object);
        node->Inputs.emplace_back(GetNextId(), "Buffer Size", PinType::Int);
        node->Outputs.emplace_back(GetNextId(), "OnData", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Data", PinType::Object);
        return node;
    });

    RegisterNode("Decorator", NodeCategory::Functions, [this] {
        const auto node = new Node(GetNextId(), "Decorator");
        node->Inputs.emplace_back(GetNextId(), "Function", PinType::Function);
        node->Inputs.emplace_back(GetNextId(), "Wrapper", PinType::Function);
        node->Outputs.emplace_back(GetNextId(), "Decorated", PinType::Function);
        return node;
    });


    RegisterNode("MessageQueue", NodeCategory::Networking, [this] {
        const auto node = new Node(GetNextId(), "MessageQueue");
        node->Inputs.emplace_back(GetNextId(), "Queue", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Message", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "OnMessage", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Received", PinType::Object);
        return node;
    });


    RegisterNode("Machine Learning", NodeCategory::AI, [this] {
        const auto node = new Node(GetNextId(), "Machine Learning");
        node->Inputs.emplace_back(GetNextId(), "Model", PinType::Object);
        node->Inputs.emplace_back(GetNextId(), "Data", PinType::List);
        node->Inputs.emplace_back(GetNextId(), "Parameters", PinType::Dictionary);
        node->Outputs.emplace_back(GetNextId(), "Predictions", PinType::List);
        node->Outputs.emplace_back(GetNextId(), "Metrics", PinType::Dictionary);
        return node;
    });

    RegisterNode("Neural Network", NodeCategory::AI, [this] {
        const auto node = new Node(GetNextId(), "Neural Network");
        node->Inputs.emplace_back(GetNextId(), "Architecture", PinType::List);
        node->Inputs.emplace_back(GetNextId(), "Training Data", PinType::List);
        node->Outputs.emplace_back(GetNextId(), "Model", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "Loss", PinType::Float);
        return node;
    });

    RegisterNode("Throw", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Throw");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Exception", PinType::Object);
        return node;
    });

    RegisterNode("Create Exception", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Create Exception");
        node->Inputs.emplace_back(GetNextId(), "Type", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Message", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Exception", PinType::Object);
        return node;
    });

    RegisterNode("Finally", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Finally");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Cleanup", PinType::Flow);
        return node;
    });

    RegisterNode("Catch Type", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Catch Type");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Exception Type", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Handler", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Exception", PinType::Object);
        return node;
    });

    RegisterNode("Get Exception Info", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Get Exception Info");
        node->Inputs.emplace_back(GetNextId(), "Exception", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "Type", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Message", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Stack Trace", PinType::String);
        return node;
    });

    RegisterNode("Assert", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Assert");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Condition", PinType::Bool);
        node->Inputs.emplace_back(GetNextId(), "Message", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    // Conversion Nodes
    RegisterNode("To Set", NodeCategory::Conversion, [this] {
        const auto node = new Node(GetNextId(), "To Set");
        node->Inputs.emplace_back(GetNextId(), "List", PinType::List);
        node->Outputs.emplace_back(GetNextId(), "Set", PinType::Set);
        return node;
    });

    RegisterNode("To List", NodeCategory::Conversion, [this] {
        const auto node = new Node(GetNextId(), "To List");
        node->Inputs.emplace_back(GetNextId(), "Set", PinType::Set);
        node->Outputs.emplace_back(GetNextId(), "List", PinType::List);
        return node;
    });

    RegisterNode("Parse JSON", NodeCategory::Conversion, [this] {
        const auto node = new Node(GetNextId(), "Parse JSON");
        node->Inputs.emplace_back(GetNextId(), "JSON String", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Object", PinType::Object);
        return node;
    });

    RegisterNode("To JSON", NodeCategory::Conversion, [this] {
        const auto node = new Node(GetNextId(), "To JSON");
        node->Inputs.emplace_back(GetNextId(), "Object", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "JSON", PinType::String);
        return node;
    });

    // String Operations
    RegisterNode("Split String", NodeCategory::String, [this] {
        const auto node = new Node(GetNextId(), "Split String");
        node->Inputs.emplace_back(GetNextId(), "String", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Delimiter", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Array", PinType::List);
        return node;
    });

    RegisterNode("Join", NodeCategory::String, [this] {
        const auto node = new Node(GetNextId(), "Join");
        node->Inputs.emplace_back(GetNextId(), "Array", PinType::List);
        node->Inputs.emplace_back(GetNextId(), "Separator", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "String", PinType::String);
        return node;
    });

    RegisterNode("Replace", NodeCategory::String, [this] {
        const auto node = new Node(GetNextId(), "Replace");
        node->Inputs.emplace_back(GetNextId(), "String", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Old", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "New", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Result", PinType::String);
        return node;
    });

    RegisterNode("Regex Match", NodeCategory::String, [this] {
        const auto node = new Node(GetNextId(), "Regex Match");
        node->Inputs.emplace_back(GetNextId(), "String", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Pattern", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Matches", PinType::List);
        node->Outputs.emplace_back(GetNextId(), "Success", PinType::Bool);
        return node;
    });

    RegisterNode("Regex Replace", NodeCategory::String, [this] {
        const auto node = new Node(GetNextId(), "Regex Replace");
        node->Inputs.emplace_back(GetNextId(), "String", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Pattern", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Replacement", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Result", PinType::String);
        return node;
    });

    RegisterNode("Format", NodeCategory::String, [this] {
        const auto node = new Node(GetNextId(), "Format");
        node->Inputs.emplace_back(GetNextId(), "Format String", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Args", PinType::List);
        node->Outputs.emplace_back(GetNextId(), "Result", PinType::String);
        return node;
    });

    RegisterNode("Substring", NodeCategory::String, [this] {
        const auto node = new Node(GetNextId(), "Substring");
        node->Inputs.emplace_back(GetNextId(), "String", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Start", PinType::Int);
        node->Inputs.emplace_back(GetNextId(), "Length", PinType::Int);
        node->Outputs.emplace_back(GetNextId(), "Result", PinType::String);
        return node;
    });

    RegisterNode("String Case", NodeCategory::String, [this] {
        const auto node = new Node(GetNextId(), "String Case");
        node->Inputs.emplace_back(GetNextId(), "String", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Case", PinType::Enum); // upper, lower, title, capitalize
        node->Outputs.emplace_back(GetNextId(), "Result", PinType::String);
        return node;
    });

    RegisterNode("Trim", NodeCategory::String, [this] {
        const auto node = new Node(GetNextId(), "Trim");
        node->Inputs.emplace_back(GetNextId(), "String", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Chars", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Result", PinType::String);
        return node;
    });

    RegisterNode("Base64", NodeCategory::Conversion, [this] {
        const auto node = new Node(GetNextId(), "Base64");
        node->Inputs.emplace_back(GetNextId(), "Data", PinType::Object);
        node->Inputs.emplace_back(GetNextId(), "Operation", PinType::Enum); // encode/decode
        node->Outputs.emplace_back(GetNextId(), "Result", PinType::String);
        return node;
    });

    RegisterNode("URL Encode", NodeCategory::String, [this] {
        const auto node = new Node(GetNextId(), "URL Encode");
        node->Inputs.emplace_back(GetNextId(), "String", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "Encoded", PinType::String);
        return node;
    });

    RegisterNode("Start Thread", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Start Thread");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Name", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Priority", PinType::Int);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Thread ID", PinType::Int);
        node->Outputs.emplace_back(GetNextId(), "Thread Block", PinType::Flow);
        return node;
    });

    RegisterNode("Lock", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Lock");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Lock Name", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Timeout MS", PinType::Int);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Acquired", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Failed", PinType::Flow);
        return node;
    });

    RegisterNode("Unlock", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Unlock");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Lock Name", PinType::String);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("Atomic Operation", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Atomic Operation");
        node->Inputs.emplace_back(GetNextId(), "Variable", PinType::Object);
        node->Inputs.emplace_back(GetNextId(), "Operation", PinType::Enum); // increment, decrement, add, exchange
        node->Inputs.emplace_back(GetNextId(), "Value", PinType::Object);
        node->Outputs.emplace_back(GetNextId(), "Result", PinType::Object);
        return node;
    });

    RegisterNode("Semaphore", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Semaphore");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Name", PinType::String);
        node->Inputs.emplace_back(GetNextId(), "Initial Count", PinType::Int);
        node->Inputs.emplace_back(GetNextId(), "Max Count", PinType::Int);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Acquired", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Released", PinType::Flow);
        return node;
    });

    RegisterNode("Thread Sleep", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Thread Sleep");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Milliseconds", PinType::Int);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        return node;
    });

    RegisterNode("Thread Join", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Thread Join");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Thread ID", PinType::Int);
        node->Inputs.emplace_back(GetNextId(), "Timeout MS", PinType::Int);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Completed", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Timeout", PinType::Flow);
        return node;
    });

    RegisterNode("Thread Pool", NodeCategory::Flow_Control, [this] {
        const auto node = new Node(GetNextId(), "Thread Pool");
        node->Inputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Inputs.emplace_back(GetNextId(), "Tasks", PinType::List);
        node->Inputs.emplace_back(GetNextId(), "Max Threads", PinType::Int);
        node->Outputs.emplace_back(GetNextId(), "", PinType::Flow);
        node->Outputs.emplace_back(GetNextId(), "Results", PinType::List);
        return node;
    });
}
