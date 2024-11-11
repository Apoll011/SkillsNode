#ifndef NODES_H
#define NODES_H

#include <functional>

#include "utilities/builders.h"
#include "utilities/widgets.h"

#include <imgui_node_editor.h>
#include <map>
#include <set>

#include <string>
#include <unordered_map>
#include <vector>
#include "crude_json.h"
namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;

using namespace ax;

using ax::Widgets::IconType;

enum class PinType {
    None,           // Represents no pin type
    Flow,           // Execution flow pin
    Bool,           // Boolean value
    Int,            // Integer value
    Float,          // Floating point value
    String,         // Text string
    List,           // Array of values
    Dictionary,     // Key-value pairs
    Object,         // Generic object reference
    Event,          // Event trigger
    Function,       // Function reference
    UserResponse,   // User interaction response
    Vector,         // Holds a 3D coordinate or direction (X, Y, Z), commonly used for spatial values
    Rotator,        // Defines a rotation in terms of yaw, pitch, and roll
    Transform,      // Contains a position, rotation, and scale value; often used to define an object's state in space
    Enum,           // Represents predefined options in a set, often used for states
    Struct,         // Holds grouped data (such as Vector, Rotator, Transform) in a single pin
    Set,            // Holds a unique collection of elements of the same type
    Delegate        // Used for event dispatching, enabling Blueprints to listen to or respond to events
};

enum class PinKind {
    Output,
    Input
};

enum class NodeType {
    Blueprint,
    Simple,
    Tree,
    Comment,
};

enum class NodeCategory {
    Data,           // Data
    Utility,        // Utilities
    SlotManagement, // Slot Management
    Response,       // Responses
    User,           // User
    Assets,         // Assets
    Configuration,  // Configuration
    Context,        // Context
    Custom,         // Other
    Action,         // Actions
    AI,             // AI
    Array,          // Array
    Audio,          // Audio
    Flow_Control,   // Flow Control
    Conversion,     // Conversion
    Data_Table,     // Data Table
    Do_N,           // Do N
    Enum,           // Enum
    Event,          // Event
    Loop,           // Loop
    Functions,      // Functions
    Math,           // Math
    Networking,     // Networking
    Object,         // Object
    Replication,    // Replication
    Set,            // Set
    String,         // String
    Struct,         // Struct
    Time,           // Time
    Transform,      // Transform
    Variable        // Variable
};

inline std::string category_to_string(NodeCategory category) {
    static const std::unordered_map<NodeCategory, std::string> categoryNames = {
        {NodeCategory::Event, "Event"},
        {NodeCategory::Action, "Actions"},
        {NodeCategory::Data, "Data"},
        {NodeCategory::Utility, "Utilities"},
        {NodeCategory::SlotManagement, "Slot Management"},
        {NodeCategory::Response, "Responses"},
        {NodeCategory::User, "User"},
        {NodeCategory::Assets, "Assets"},
        {NodeCategory::Configuration, "Configuration"},
        {NodeCategory::Context, "Context"},
        {NodeCategory::Custom, "Other"},
        {NodeCategory::AI, "AI"},
        {NodeCategory::Array, "Array"},
        {NodeCategory::Audio, "Audio"},
        {NodeCategory::Flow_Control, "Flow Control"},
        {NodeCategory::Conversion, "Conversion"},
        {NodeCategory::Data_Table, "Data Table"},
        {NodeCategory::Do_N, "Do N"},
        {NodeCategory::Enum, "Enum"},
        {NodeCategory::Loop, "Loop"},
        {NodeCategory::Functions, "Functions"},
        {NodeCategory::Math, "Math"},
        {NodeCategory::Networking, "Networking"},
        {NodeCategory::Object, "Object"},
        {NodeCategory::Replication, "Replication"},
        {NodeCategory::Set, "Set"},
        {NodeCategory::String, "String"},
        {NodeCategory::Struct, "Struct"},
        {NodeCategory::Time, "Time"},
        {NodeCategory::Transform, "Transform"},
        {NodeCategory::Variable, "Variable"}
    };

    auto it = categoryNames.find(category);
    if (it != categoryNames.end()) {
        return it->second;
    } else {
        return "Misc";
    }
}

inline ImColor category_color(NodeCategory category) {
    static const std::unordered_map<NodeCategory, ImColor> categoryColors = {
        // Actions
        {NodeCategory::Action, ImColor(230, 95, 48)},
        // Flow Control
        {NodeCategory::Flow_Control, ImColor(97, 182, 255)},
        {NodeCategory::Loop, ImColor(97, 182, 255)},
        {NodeCategory::Do_N, ImColor(97, 182, 255)},
        // Data
        {NodeCategory::Data, ImColor(255, 171, 145)},
        {NodeCategory::Data_Table, ImColor(255, 171, 145)},
        {NodeCategory::Struct, ImColor(255, 171, 145)},
        {NodeCategory::Array, ImColor(255, 171, 145)},
        {NodeCategory::Set, ImColor(255, 171, 145)},
        {NodeCategory::Variable, ImColor(255, 171, 145)},
        // Utilities
        {NodeCategory::Utility, ImColor(255, 213, 79)},
        {NodeCategory::Conversion, ImColor(255, 213, 79)},
        {NodeCategory::String, ImColor(255, 213, 79)},
        {NodeCategory::Math, ImColor(255, 213, 79)},
        {NodeCategory::Time, ImColor(255, 213, 79)},
        {NodeCategory::Transform, ImColor(255, 213, 79)},
        // Other
        {NodeCategory::Event, ImColor(110, 207, 151)},
        {NodeCategory::Response, ImColor(76, 175, 80)},
        {NodeCategory::User, ImColor(233, 30, 99)},
        {NodeCategory::Assets, ImColor(255, 152, 0)},
        {NodeCategory::Configuration, ImColor(103, 58, 183)},
        {NodeCategory::Context, ImColor(0, 188, 212)},
        {NodeCategory::Custom, ImColor(117, 117, 117)},
        {NodeCategory::AI, ImColor(156, 39, 176)},
        {NodeCategory::Enum, ImColor(224, 64, 251)},
        {NodeCategory::Functions, ImColor(76, 175, 80)},
        {NodeCategory::Networking, ImColor(103, 58, 183)},
        {NodeCategory::Object, ImColor(0, 188, 212)},
        {NodeCategory::Replication, ImColor(142, 36, 170)},
        {NodeCategory::SlotManagement, ImColor(187, 107, 217)}
    };

    auto it = categoryColors.find(category);
    if (it != categoryColors.end()) {
        return it->second;
    } else {
        return ImColor(117, 117, 117); // Return a default color if not found
    }
}
struct Node;

struct Pin {
    ed::PinId ID;
    ::Node *Node;
    std::string Name;
    PinType Type;
    PinKind Kind;
    bool can_ask_on_device;

    Pin(int id, const char *name, const PinType type): ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input), can_ask_on_device(false) {
    }
    Pin(int id, const char *name, const PinType type, const bool can_ask_on_device): ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input), can_ask_on_device(can_ask_on_device) {
    }
};

struct Node {
    ed::NodeId ID;
    std::string Name;
    std::string Description;
    std::vector<Pin> Inputs;
    std::vector<Pin> Outputs;
    ImColor Color;
    NodeType Type;
    ImVec2 Size;
    NodeCategory Category;

    std::string State;
    std::string SavedState;

    Node(const unsigned long id, const char *name, const ImColor color = ImColor(255, 255, 255), const NodeCategory category = NodeCategory::Custom)
        : ID(id), Name(name), Description(name), Color(color), Type(NodeType::Blueprint), Size(0, 0), Category(category) {
    }

    Node(const unsigned long id, const char *name, const char *description, const ImColor color = ImColor(255, 255, 255), const NodeCategory category = NodeCategory::Custom)
        : ID(id), Name(name), Description(description), Color(color), Type(NodeType::Blueprint), Size(0, 0), Category(category) {
    }

    Node(const unsigned long id, const std::string& name, const ImColor color = ImColor(255, 255, 255), const NodeCategory category = NodeCategory::Custom)
        : ID(id), Name(name), Description(name), Color(color), Type(NodeType::Blueprint), Size(0, 0), Category(category) {
    }

    Node(const unsigned long id, const std::string &name, const char *description, const ImColor color = ImColor(255, 255, 255), const NodeCategory category = NodeCategory::Custom)
        : ID(id), Name(name), Description(description), Color(color), Type(NodeType::Blueprint), Size(0, 0), Category(category) {
    }
};

struct Link {
    ed::LinkId ID;

    ed::PinId StartPinID;
    ed::PinId EndPinID;

    ImColor Color;

    Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId): ID(id), StartPinID(startPinId), EndPinID(endPinId),
                                                                   Color(255, 255, 255) {
    }
};

struct NodeIdLess {
    bool operator()(const ed::NodeId &lhs, const ed::NodeId &rhs) const {
        return lhs.AsPointer() < rhs.AsPointer();
    }
};

struct Variable {
    std::string name;
    PinType type;
};

struct Function {
    std::string name;
    std::vector<Pin> parameters;
    PinType returnType;
};


struct Nodes {
    unsigned long GetNextId();

    ed::LinkId GetNextLinkId();

    void TouchNode(ed::NodeId id);

    float GetTouchProgress(ed::NodeId id);

    void UpdateTouch();

    Node *FindNode(ed::NodeId id);

    Link *FindLink(ed::LinkId id);

    Pin *FindPin(ed::PinId id);

    bool IsPinLinked(ed::PinId id);

    ed::LinkId *PinLinkId(ed::PinId id);

    Node* choice();

    void draw_node(Node &node, util::BlueprintNodeBuilder &builder, const Pin* newLinkPin);

    Pin* handle_creation_and_destruction(const Pin* newLinkPin, bool &createNewNode);

    auto hasPathToNode(const Node *start, const Node *target, std::set<const Node *> &visited) -> bool;

    static ImColor GetIconColor(PinType type);

    void DrawPinIcon(const Pin &pin, bool connected, int alpha) const;

    Node *generate_node_context_menu_generate(const std::string& label, NodeCategory category);

    void create_node(Node* node, ImVec2 newNodePosition, bool &createNewNode, Pin* newNodeLinkPin);

    static bool CanCreateLink(const Pin *a, const Pin *b);

    static void BuildNode(Node *node);

    void CreateNodes();

    void BuildNodes();

    int m_NextId = 1;
    std::vector<Node> m_Nodes;
    std::vector<Link> m_Links;

    const float m_TouchTime = 1.0f;
    std::map<ed::NodeId, float, NodeIdLess> m_NodeTouchTime;
    const int m_PinIconSize = 24;

    // New function to register and create nodes
    void RegisterNode(const char* name, NodeCategory category, const std::function<Node*()>& createFunction);
    std::map<std::string, std::function<Node*()>> m_NodeCreators;
    std::map<NodeCategory, std::vector<std::string>> m_NodeCategory;
    std::vector<Variable> m_Variables;
    std::vector<Function> m_Functions;
};

#endif //NODES_H
