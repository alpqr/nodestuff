#include "gui.h"
#include "nodeconstructors.h"
#include "imnodes.h"

void Gui::init(Graph *g)
{
    graph = g;
    imnodes::Initialize();
}

void Gui::cleanup()
{
    imnodes::Shutdown();
}

static void valueEditor(Port &port, bool *active)
{
    std::visit([active](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, PortDataFloat>) {
            ImGui::PushItemWidth(60);
            ImGui::InputFloat("", &arg.v);
            *active |= ImGui::IsItemActive();
            ImGui::PopItemWidth();
        } else if constexpr (std::is_same_v<T, PortDataInt>) {
            ImGui::PushItemWidth(60);
            ImGui::InputScalar("", ImGuiDataType_S32, &arg.v);
            *active |= ImGui::IsItemActive();
            ImGui::PopItemWidth();
        } else if constexpr (std::is_same_v<T, PortDataUInt>) {
            ImGui::PushItemWidth(60);
            ImGui::InputScalar("", ImGuiDataType_U32, &arg.v);
            *active |= ImGui::IsItemActive();
            ImGui::PopItemWidth();
        }
    }, port.data.d);
}

static void valueLabel(const Port &port)
{
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        char s[128];
        if constexpr (std::is_same_v<T, PortDataFloat>) {
            ImGui::PushItemWidth(60);
            sprintf(s, "%.5f", arg.v);
            ImGui::Text(s);
            ImGui::PopItemWidth();
        } else if constexpr (std::is_same_v<T, PortDataInt>) {
            ImGui::PushItemWidth(60);
            sprintf(s, "%d", arg.v);
            ImGui::Text(s);
            ImGui::PopItemWidth();
        } else if constexpr (std::is_same_v<T, PortDataUInt>) {
            ImGui::PushItemWidth(60);
            sprintf(s, "%u", arg.v);
            ImGui::Text(s);
            ImGui::PopItemWidth();
        }
    }, port.data.d);
}

void Gui::frame()
{
    ImGui::SetNextWindowPos(ImVec2(10, 60), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(1260, 640), ImGuiCond_FirstUseEver);
    ImGui::Begin("Graph");

    imnodes::PushAttributeFlag(imnodes::AttributeFlags_EnableLinkDetachWithDragClick);
    imnodes::BeginNodeEditor();

    bool editorActive = false;
    for (auto it = graph->nodes.begin(), end = graph->nodes.end(); it != end; ++it) {
        Node &n(it->second);
        imnodes::BeginNode(it->first);
        imnodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(n.text.c_str());
        imnodes::EndNodeTitleBar();
        for (const Port &port : n.ports) {
            if (port.dir == PortDirection::Input) {
                imnodes::BeginInputAttribute(port.id);
                ImGui::Text(port.text.c_str());
                imnodes::EndInputAttribute();
            }
        }
        for (Port &port : n.ports) {
            if (port.dir == PortDirection::Static) {
                imnodes::BeginStaticAttribute(port.id);
                ImGui::Text(port.text.c_str());
                ImGui::SameLine();
                valueEditor(port, &editorActive);
                imnodes::EndStaticAttribute();
            }
        }
        for (const Port &port : n.ports) {
            if (port.dir == PortDirection::Output) {
                imnodes::BeginOutputAttribute(port.id);
                ImGui::Text(port.text.c_str());
                ImGui::SameLine();
                valueLabel(port);
                imnodes::EndOutputAttribute();
            }
        }
        imnodes::EndNode();
    }

    for (const Connection &c : graph->connections)
        imnodes::Link(c.id, c.port0, c.port1);

    const bool editorHovered = imnodes::IsEditorHovered();
    imnodes::EndNodeEditor();

    int fromPort, toPort;
    if (imnodes::IsLinkCreated(&fromPort, &toPort))
        graph->addConnection(graph->nodeForPort(fromPort).id, fromPort, graph->nodeForPort(toPort).id, toPort);

    int edgeId;
    if (imnodes::IsLinkDestroyed(&edgeId))
        graph->removeConnection(edgeId);

    // right click = popup
    if (ImGui::IsMouseClicked(1) && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && editorHovered)
        ImGui::OpenPopup("editor_menu");
    if (ImGui::BeginPopup("editor_menu")) {
        const ImVec2 pos = ImGui::GetMousePosOnOpeningCurrentPopup();
        if (ImGui::BeginMenu("Add node")) {
            NodeConstructorSet *s = nodeConstructorSets;
            while (s->category && s->constructors) {
                if (ImGui::BeginMenu(s->category)) {
                    NodeConstructor *c = s->constructors;
                    while (c->text && c->func) {
                        if (ImGui::MenuItem(c->text))
                            imnodes::SetNodeScreenSpacePos(c->func(graph), pos);
                        ++c;
                    }
                    ImGui::EndMenu();
                }
                ++s;
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }

    // Del = delete selected node or link (unless an editor is active)
    const int delKey = ImGui::GetKeyIndex(ImGuiKey_Delete);
    if (!editorActive && ImGui::IsKeyPressed(delKey)) {
        static std::vector<int> selected;
        const int selectedLinkCount = imnodes::NumSelectedLinks();
        if (selectedLinkCount > 0) {
            selected.resize(size_t(selectedLinkCount));
            imnodes::GetSelectedLinks(selected.data());
            for (Id edgeId : selected)
                graph->removeConnection(edgeId);
        }
        const int selectedNodeCount = imnodes::NumSelectedNodes();
        if (selectedNodeCount > 0) {
            selected.resize(size_t(selectedNodeCount));
            imnodes::GetSelectedNodes(selected.data());
            for (Id nodeId : selected)
                graph->removeNode(nodeId);
        }
    }

    ImGui::End();
}
