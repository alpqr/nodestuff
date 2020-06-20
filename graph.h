#ifndef GRAPH_H
#define GRAPH_H

#include "nodetypes.h"
#include "portdata.h"
#include <vector>
#include <array>
#include <unordered_map>
#include <functional>

using Id = int; // uses one global id space for everything

struct Connection
{
    Id id;
    struct {
        Id nodeId;
        Id portId;
    } ep[2];
};

enum class PortDirection {
    Input,
    Output,
    Static
};

struct Port
{
    Id id;
    PortDirection dir;
    int order;
    std::string text;
    PortData data;
};

struct Graph;

struct Node
{
    Id id;
    NodeType type;
    size_t inputPortCount;
    std::function<void(Graph&, Node&, std::vector<PortData>&)> evalFunc;
    std::vector<Port> ports;

    Port &port(Id portId)
    {
        return *std::find_if(ports.begin(), ports.end(), [portId](const Port &port) { return port.id == portId; });
    }

    const Port &port(Id portId) const
    {
        return *std::find_if(ports.cbegin(), ports.cend(), [portId](const Port &port) { return port.id == portId; });
    }

    std::string text;
};

struct Graph
{
    std::unordered_map<Id, Node> nodes;
    std::unordered_map<Id, Id> portNodeMap;
    std::vector<Connection> connections;
    Id nextId = 1;

    Node &newNode()
    {
        const Id id = nextId++;
        nodes[id] = Node { id, NodeType::Invalid, 0, nullptr };
        return nodes[id];
    }

    void removeNode(Id id)
    {
        nodes.erase(id);
        // ### portNodeMap
        connections.erase(std::remove_if(connections.begin(), connections.end(),
            [id](const Connection &c) { return c.ep[0].nodeId == id || c.ep[1].nodeId == id; }), connections.end());
    }

    Node &node(Id id) { return nodes.at(id); }
    const Node &node(Id id) const { return nodes.at(id); }

    Port &addPort(Node &node, PortDirection dir)
    {
        const Id id = nextId++;
        node.ports.push_back(Port { id, dir, 0 });
        portNodeMap[id] = node.id;
        return node.ports[node.ports.size() - 1];
    }

    void removePort(Node &node, Id id)
    {
        node.ports.erase(std::find_if(node.ports.begin(), node.ports.end(), [id](const Port &port) { return port.id == id; }));
        portNodeMap.erase(id);
        connections.erase(std::remove_if(connections.begin(), connections.end(),
            [id](const Connection &c) { return c.ep[0].portId == id || c.ep[1].portId == id; }), connections.end());
    }

    Node &nodeForPort(Id id) { return nodes.at(portNodeMap[id]); }
    const Node &nodeForPort(Id id) const { return nodes.at(portNodeMap.at(id)); }

    Id addConnection(Id fromNode, Id fromPort, Id toNode, Id toPort)
    {
        // disallow Input - Input, Output - Output
        if (node(fromNode).port(fromPort).dir != node(toNode).port(toPort).dir) {
            // disallow multiple connections to an Input
            if (std::find_if(connections.cbegin(), connections.cend(), [this, fromPort, toPort](const Connection &c) {
                for (int i = 0; i < 2; ++i) {
                    const Id cpid = c.ep[i].portId;
                    if (cpid == fromPort || cpid == toPort) {
                        if (node(c.ep[i].nodeId).port(cpid).dir == PortDirection::Input)
                            return true;
                    }
                }
                return false;
            }) == connections.cend()) {
                const Id id = nextId++;
                connections.push_back({ id, { { fromNode, fromPort }, { toNode, toPort } } });
                return id;
            }
        }
        return 0;
    }

    void removeConnection(Id id)
    {
        auto it = std::find_if(connections.begin(), connections.end(), [id](const Connection &c) { return c.id == id; });
        if (it != connections.end())
            connections.erase(it);
    }

    std::vector<std::pair<Id, int>> orderedSourceNodesForNode(Id id) const
    {
        std::vector<std::pair<Id, int>> result;
        const Node &n(node(id));
        for (const Connection &c : connections) {
            for (int i = 0; i < 2; ++i) {
                if (n.id == c.ep[i].nodeId) {
                    const Port &port(n.port(c.ep[i].portId));
                    if (port.dir == PortDirection::Input)
                        result.push_back({ c.ep[1 - i].nodeId, port.order });
                }
            }
        }
        std::sort(result.begin(), result.end(), [](const std::pair<Id, int> &a, const std::pair<Id, int> &b) {
            return a.second < b.second;
        });
        return result;
    }
};

#endif
