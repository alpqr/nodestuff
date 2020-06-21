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
        } else if constexpr (std::is_same_v<T, PortDataVec2>) {
            ImGui::PushItemWidth(120);
            ImGui::InputFloat2("", glm::value_ptr(arg.v));
            *active |= ImGui::IsItemActive();
            ImGui::PopItemWidth();
        } else if constexpr (std::is_same_v<T, PortDataVec3>) {
            ImGui::PushItemWidth(180);
            ImGui::InputFloat3("", glm::value_ptr(arg.v));
            *active |= ImGui::IsItemActive();
            ImGui::PopItemWidth();
        } else if constexpr (std::is_same_v<T, PortDataVec4>) {
            ImGui::PushItemWidth(240);
            ImGui::InputFloat4("", glm::value_ptr(arg.v));
            *active |= ImGui::IsItemActive();
            ImGui::PopItemWidth();
        } else if constexpr (std::is_same_v<T, PortDataMat3>) {
            ImGui::Text("");
            ImGui::PushItemWidth(180);
            static const char *labels[3] = { "COL 0", "COL 1", "COL 2" };
            for (int col = 0; col < 3; ++col)
                ImGui::InputFloat3(labels[col], glm::value_ptr(arg.v) + 3 * col);
            *active |= ImGui::IsItemActive();
            ImGui::PopItemWidth();
        } else if constexpr (std::is_same_v<T, PortDataMat4>) {
            ImGui::Text("");
            ImGui::PushItemWidth(240);
            static const char *labels[4] = { "COL 0", "COL 1", "COL 2", "COL 3" };
            for (int col = 0; col < 4; ++col)
                ImGui::InputFloat4(labels[col], glm::value_ptr(arg.v) + 4 * col);
            *active |= ImGui::IsItemActive();
            ImGui::PopItemWidth();
        } else if constexpr (std::is_same_v<T, PortDataString>) {
            char s[5];
            strcpy(s, arg.v.c_str());
            ImGui::PushItemWidth(50);
            ImGui::InputText("", s, sizeof(s));
            ImGui::PopItemWidth();
            arg.v = s;
        }
    }, port.data.d);
}

static void valueLabel(const Port &port)
{
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        char s[128];
        if constexpr (std::is_same_v<T, PortDataFloat>) {
            sprintf(s, "%.5f", arg.v);
            ImGui::Text(s);
        } else if constexpr (std::is_same_v<T, PortDataVec2>) {
            sprintf(s, "(%.5f, %.5f)", arg.v[0], arg.v[1]);
            ImGui::Text(s);
        } else if constexpr (std::is_same_v<T, PortDataVec3>) {
            sprintf(s, "(%.5f, %.5f, %.5f)", arg.v[0], arg.v[1], arg.v[2]);
            ImGui::Text(s);
        } else if constexpr (std::is_same_v<T, PortDataVec4>) {
            sprintf(s, "(%.5f, %.5f, %.5f, %.5f)", arg.v[0], arg.v[1], arg.v[2], arg.v[3]);
            ImGui::Text(s);
        } else if constexpr (std::is_same_v<T, PortDataMat3>) {
            ImGui::Text("");
            for (int col = 0; col < 3; ++col) {
                sprintf(s, "%s%.5f, %.5f, %.5f%s", col == 0 ? "( " : "  ", arg.v[col][0], arg.v[col][1], arg.v[col][2], col == 2 ? " )" : "");
                ImGui::Text(s);
            }
        } else if constexpr (std::is_same_v<T, PortDataMat4>) {
            ImGui::Text("");
            for (int col = 0; col < 4; ++col) {
                sprintf(s, "%s%.5f, %.5f, %.5f, %.5f%s", col == 0 ? "( " : "  ", arg.v[col][0], arg.v[col][1], arg.v[col][2], arg.v[col][3], col == 3 ? " )" : "");
                ImGui::Text(s);
            }
        } else if constexpr (std::is_same_v<T, PortDataString>) {
            ImGui::Text("%s", arg.v.c_str());
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
                if (!port.data.desc.empty()) {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", port.data.desc.c_str());
                    ImGui::SameLine();
                }
                valueLabel(port);
                imnodes::EndOutputAttribute();
            }
        }
        imnodes::EndNode();
    }

    for (const Connection &c : graph->connections)
        imnodes::Link(c.id, c.ep[0].portId, c.ep[1].portId);

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
