#include "grapheval.h"
#include "graph.h"
#include <unordered_set>

namespace GraphEval {

void update(Graph &g)
{
    static std::unordered_set<Id> evaluated;
    evaluated.clear();
    for (auto it = g.nodes.begin(), end = g.nodes.end(); it != end; ++it) {
        Node &n(it->second);
        static std::vector<PortData> evalStack;
        evalStack.clear();
        static std::vector<Id> dfsStack;
        dfsStack.clear();
        struct EvalNode { Id id; size_t sourceCount; };
        static std::vector<EvalNode> nodes;
        nodes.clear();
        dfsStack.push_back(n.id);
        while (!dfsStack.empty()) {
            const Id id = dfsStack.back();
            dfsStack.pop_back();
            const auto sourceNodes = g.orderedSourceNodesForNode(id);
            Node &node(g.node(id));
            nodes.push_back({ id, sourceNodes.size() });
            for (const std::pair<Id, int> &src : sourceNodes)
                dfsStack.push_back(src.first);
        }
        while (!nodes.empty()) {
            const EvalNode cur = nodes.back();
            nodes.pop_back();
            const bool alreadyEvaluated = evaluated.find(cur.id) != evaluated.end();
            Node &node(g.node(cur.id));
            if (cur.sourceCount != node.inputPortCount) {
                for (size_t i = 0; i < cur.sourceCount; ++i)
                    evalStack.pop_back();
                auto outPort = std::find_if(node.ports.begin(), node.ports.end(),
                    [](const Port &port) { return port.dir == PortDirection::Output; });
                PortData emptyResult;
                emptyResult.d = PortDataEmpty { };
                outPort->data = emptyResult;
                evalStack.push_back(emptyResult);
            } else if (alreadyEvaluated) {
                for (size_t i = 0; i < node.inputPortCount; ++i)
                    evalStack.pop_back();
                auto outPort = std::find_if(node.ports.cbegin(), node.ports.cend(),
                    [](const Port &port) { return port.dir == PortDirection::Output; });
                evalStack.push_back(outPort->data);
            } else {
                evaluated.insert(cur.id);
                if (node.evalFunc)
                    node.evalFunc(g, node, evalStack);
            }
        }
    }
}

void evalConstantNode(Graph &g, Node &n, std::vector<PortData>& evalStack)
{
    auto src = std::find_if(n.ports.cbegin(), n.ports.cend(), [](const Port &port) { return port.dir == PortDirection::Static; });
    auto dst = std::find_if(n.ports.begin(), n.ports.end(), [](const Port &port) { return port.dir == PortDirection::Output; });
    dst->data = src->data;
    evalStack.push_back(dst->data);
}

void evalPlusNode(Graph &g, Node &n, std::vector<PortData>& evalStack)
{
    PortData rhs = evalStack.back();
    evalStack.pop_back();
    PortData lhs = evalStack.back();
    evalStack.pop_back();

    PortData result;
    if (std::holds_alternative<PortDataFloat>(lhs.d) && std::holds_alternative<PortDataFloat>(rhs.d)) {
        const float a = std::get<PortDataFloat>(lhs.d).v;
        const float b = std::get<PortDataFloat>(rhs.d).v;
        result.d = PortDataFloat { a + b };
    }
    evalStack.push_back(result);

    auto dst = std::find_if(n.ports.begin(), n.ports.end(), [](const Port &port) { return port.dir == PortDirection::Output; });
    dst->data = result;
}

void evalMinusNode(Graph &g, Node &n, std::vector<PortData>& evalStack)
{
    PortData rhs = evalStack.back();
    evalStack.pop_back();
    PortData lhs = evalStack.back();
    evalStack.pop_back();

    PortData result;
    if (std::holds_alternative<PortDataFloat>(lhs.d) && std::holds_alternative<PortDataFloat>(rhs.d)) {
        const float a = std::get<PortDataFloat>(lhs.d).v;
        const float b = std::get<PortDataFloat>(rhs.d).v;
        result.d = PortDataFloat { a - b };
    }
    evalStack.push_back(result);

    auto dst = std::find_if(n.ports.begin(), n.ports.end(), [](const Port &port) { return port.dir == PortDirection::Output; });
    dst->data = result;
}

} // namespace
