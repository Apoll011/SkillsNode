#include "nodes.h"
#include "crude_json.h"

// Convert Pin to JSON
crude_json::value PinToJson(const Pin& pin) {
    crude_json::object pinJson;
    pinJson["id"] = static_cast<double>(pin.ID.Get());
    pinJson["name"] = pin.Name;
    pinJson["type"] = static_cast<double>(pin.Type);
    pinJson["kind"] = static_cast<double>(pin.Kind);
    pinJson["can_ask_on_device"] = pin.can_ask_on_device;
    return pinJson;
}

// Convert Node to JSON
crude_json::value NodeToJson(const Node& node) {
    crude_json::object nodeJson;
    nodeJson["id"] = static_cast<double>(node.ID.Get());
    nodeJson["name"] = node.Name;
    nodeJson["description"] = node.Description;
    nodeJson["type"] = static_cast<double>(node.Type);
    nodeJson["category"] = static_cast<double>(node.Category);
    nodeJson["state"] = node.State;
    nodeJson["saved_state"] = node.SavedState;
    
    // Convert color
    crude_json::array colorArr;
    colorArr.push_back(node.Color.Value.x);
    colorArr.push_back(node.Color.Value.y);
    colorArr.push_back(node.Color.Value.z);
    colorArr.push_back(node.Color.Value.w);
    nodeJson["color"] = colorArr;
    
    // Convert size
    crude_json::array sizeArr;
    sizeArr.push_back(node.Size.x);
    sizeArr.push_back(node.Size.y);
    nodeJson["size"] = sizeArr;
    
    // Convert inputs
    crude_json::array inputs;
    for (const Pin& pin : node.Inputs) {
        inputs.push_back(PinToJson(pin));
    }
    nodeJson["inputs"] = inputs;
    
    // Convert outputs
    crude_json::array outputs;
    for (const Pin& pin : node.Outputs) {
        outputs.push_back(PinToJson(pin));
    }
    nodeJson["outputs"] = outputs;
    
    return nodeJson;
}

// Convert Link to JSON
crude_json::value LinkToJson(const Link& link) {
    crude_json::object linkJson;
    linkJson["id"] = static_cast<double>(link.ID.Get());
    linkJson["start_pin_id"] = static_cast<double>(link.StartPinID.Get());
    linkJson["end_pin_id"] = static_cast<double>(link.EndPinID.Get());
    
    // Convert color
    crude_json::array colorArr;
    colorArr.push_back(link.Color.Value.x);
    colorArr.push_back(link.Color.Value.y);
    colorArr.push_back(link.Color.Value.z);
    colorArr.push_back(link.Color.Value.w);
    linkJson["color"] = colorArr;
    
    return linkJson;
}

// Save nodes and links to JSON file
bool Nodes::SaveToJson(const std::string& filename) {
    crude_json::object root;
    
    // Save next ID counter
    root["next_id"] = static_cast<double>(m_NextId);
    
    // Save nodes
    crude_json::array nodesArr;
    for (const Node& node : m_Nodes) {
        nodesArr.push_back(NodeToJson(node));
    }
    root["nodes"] = nodesArr;
    
    // Save links
    crude_json::array linksArr;
    for (const Link& link : m_Links) {
        linksArr.push_back(LinkToJson(link));
    }
    root["links"] = linksArr;
    
    // Save variables
    crude_json::array varsArr;
    for (const Variable& var : m_Variables) {
        crude_json::object varJson;
        varJson["name"] = var.name;
        varJson["type"] = static_cast<double>(var.type);
        varsArr.push_back(varJson);
    }
    root["variables"] = varsArr;
    
    // Save functions
    crude_json::array funcsArr;
    for (const Function& func : m_Functions) {
        crude_json::object funcJson;
        funcJson["name"] = func.name;
        funcJson["return_type"] = static_cast<double>(func.returnType);
        
        crude_json::array paramsArr;
        for (const Pin& param : func.parameters) {
            paramsArr.push_back(PinToJson(param));
        }
        funcJson["parameters"] = paramsArr;
        
        funcsArr.emplace_back(funcJson);
    }
    root["functions"] = funcsArr;
    
    // Create the JSON value and save to file
    crude_json::value jsonValue(root);
    return jsonValue.save(filename, 2); // Save with indentation of 2 spaces
}