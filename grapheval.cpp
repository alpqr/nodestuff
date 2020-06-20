#include "grapheval.h"
#include "graph.h"
#include <unordered_set>

namespace GraphEval {

static inline Port &outPort(Node &node)
{
    auto outPort = std::find_if(node.ports.begin(), node.ports.end(),
        [](const Port &port) { return port.dir == PortDirection::Output; });
    return *outPort;
}

void update(Graph &g)
{
    static std::unordered_set<Id> evaluated;
    evaluated.clear();

    for (auto it = g.nodes.begin(), end = g.nodes.end(); it != end; ++it) {
        if (evaluated.find(it->first) != evaluated.end())
            continue;

        static std::vector<PortData> evalStack;
        evalStack.clear();
        static std::vector<Id> dfsStack;
        dfsStack.clear();
        struct EvalNode { Id id; size_t sourceCount; };
        static std::vector<EvalNode> nodes;
        nodes.clear();

        Node &n(it->second);
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
            Node &node(g.node(cur.id));
            if (cur.sourceCount != node.inputPortCount) {
                // not enough connections
                for (size_t i = 0; i < cur.sourceCount; ++i)
                    evalStack.pop_back();
                PortData emptyResult;
                outPort(node).data = emptyResult;
                evalStack.push_back(emptyResult);
            } else if (evaluated.find(cur.id) != evaluated.end()) {
                // already evaluated in this frame
                for (size_t i = 0; i < node.inputPortCount; ++i)
                    evalStack.pop_back();
                evalStack.push_back(outPort(node).data);
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
    evalStack.push_back(src->data);
    outPort(n).data = src->data;
}

static inline void pushArith2Result(std::vector<PortData>& evalStack, Node &n, const PortData &result)
{
    evalStack.pop_back();
    evalStack.pop_back();
    evalStack.push_back(result);
    outPort(n).data = result;
}

template<typename T_LHS, typename V_LHS, typename T_RHS, typename V_RHS>
inline bool evalArith2Node(Graph &g, Node &n, std::vector<PortData>& evalStack, std::function<PortDataVar(const T_LHS &a, const T_RHS &b)> f)
{
    const PortData rhs = evalStack.back();
    const PortData lhs = evalStack[evalStack.size() - 2];

    if (std::holds_alternative<V_LHS>(lhs.d) && std::holds_alternative<V_RHS>(rhs.d)) {
        const T_LHS a = std::get<V_LHS>(lhs.d).v;
        const T_RHS b = std::get<V_RHS>(rhs.d).v;
        PortData result;
        result.d = f(a, b);
        pushArith2Result(evalStack, n, result);
        return true;
    }

    return false;
}

void evalPlusNode(Graph &g, Node &n, std::vector<PortData>& evalStack)
{
    if (evalArith2Node<float, PortDataFloat, float, PortDataFloat>(g, n, evalStack, [](float a, float b) { return PortDataFloat { a + b }; }))
        return;

    if (evalArith2Node<glm::vec2, PortDataVec2, glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a, const glm::vec2 &b) { return PortDataVec2 { a + b }; }))
        return;

    if (evalArith2Node<glm::vec3, PortDataVec3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a, const glm::vec3 &b) { return PortDataVec3 { a + b }; }))
        return;

    if (evalArith2Node<glm::vec4, PortDataVec4, glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a, const glm::vec4 &b) { return PortDataVec4 { a + b }; }))
        return;

    pushArith2Result(evalStack, n, PortData { });
}

void evalMinusNode(Graph &g, Node &n, std::vector<PortData>& evalStack)
{
    if (evalArith2Node<float, PortDataFloat, float, PortDataFloat>(g, n, evalStack, [](float a, float b) { return PortDataFloat { a - b }; }))
        return;

    if (evalArith2Node<glm::vec2, PortDataVec2, glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a, const glm::vec2 &b) { return PortDataVec2 { a - b }; }))
        return;

    if (evalArith2Node<glm::vec3, PortDataVec3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a, const glm::vec3 &b) { return PortDataVec3 { a - b }; }))
        return;

    if (evalArith2Node<glm::vec4, PortDataVec4, glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a, const glm::vec4 &b) { return PortDataVec4 { a - b }; }))
        return;

    pushArith2Result(evalStack, n, PortData { });
}

void evalMulNode(Graph &g, Node &n, std::vector<PortData>& evalStack)
{
    if (evalArith2Node<float, PortDataFloat, float, PortDataFloat>(g, n, evalStack, [](float a, float b) { return PortDataFloat { a * b }; }))
        return;

    if (evalArith2Node<glm::vec2, PortDataVec2, glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a, const glm::vec2 &b) { return PortDataVec2 { a * b }; }))
        return;

    if (evalArith2Node<glm::vec2, PortDataVec2, float, PortDataFloat>(g, n, evalStack, [](const glm::vec2 &a, float b) { return PortDataVec2 { a * b }; }))
        return;

    if (evalArith2Node<float, PortDataFloat, glm::vec2, PortDataVec2>(g, n, evalStack, [](float a, const glm::vec2 &b) { return PortDataVec2 { a * b }; }))
        return;

    if (evalArith2Node<glm::vec3, PortDataVec3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a, const glm::vec3 &b) { return PortDataVec3 { a * b }; }))
        return;

    if (evalArith2Node<glm::vec3, PortDataVec3, float, PortDataFloat>(g, n, evalStack, [](const glm::vec3 &a, float b) { return PortDataVec3 { a * b }; }))
        return;

    if (evalArith2Node<float, PortDataFloat, glm::vec3, PortDataVec3>(g, n, evalStack, [](float a, const glm::vec3 &b) { return PortDataVec3 { a * b }; }))
        return;

    if (evalArith2Node<glm::vec4, PortDataVec4, glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a, const glm::vec4 &b) { return PortDataVec4 { a * b }; }))
        return;

    if (evalArith2Node<glm::vec4, PortDataVec4, float, PortDataFloat>(g, n, evalStack, [](const glm::vec4 &a, float b) { return PortDataVec4 { a * b }; }))
        return;

    if (evalArith2Node<float, PortDataFloat, glm::vec4, PortDataVec4>(g, n, evalStack, [](float a, const glm::vec4 &b) { return PortDataVec4 { a * b }; }))
        return;

    if (evalArith2Node<glm::mat3, PortDataMat3, glm::mat3, PortDataMat3>(g, n, evalStack, [](const glm::mat3 &a, const glm::mat3 &b) { return PortDataMat3 { a * b }; }))
        return;

    if (evalArith2Node<glm::mat3, PortDataMat3, float, PortDataFloat>(g, n, evalStack, [](const glm::mat3 &a, float b) { return PortDataMat3 { a * b }; }))
        return;

    if (evalArith2Node<float, PortDataFloat, glm::mat3, PortDataMat3>(g, n, evalStack, [](float a, const glm::mat3 &b) { return PortDataMat3 { a * b }; }))
        return;

    if (evalArith2Node<glm::mat3, PortDataMat3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::mat3 &a, const glm::vec3 &b) { return PortDataVec3 { a * b }; }))
        return;

    if (evalArith2Node<glm::vec3, PortDataVec3, glm::mat3, PortDataMat3>(g, n, evalStack, [](const glm::vec3 &a, const glm::mat3 &b) { return PortDataVec3 { a * b }; }))
        return;

    if (evalArith2Node<glm::mat4, PortDataMat4, glm::mat4, PortDataMat4>(g, n, evalStack, [](const glm::mat4 &a, const glm::mat4 &b) { return PortDataMat4 { a * b }; }))
        return;

    if (evalArith2Node<glm::mat4, PortDataMat4, float, PortDataFloat>(g, n, evalStack, [](const glm::mat4 &a, float b) { return PortDataMat4 { a * b }; }))
        return;

    if (evalArith2Node<float, PortDataFloat, glm::mat4, PortDataMat4>(g, n, evalStack, [](float a, const glm::mat4 &b) { return PortDataMat4 { a * b }; }))
        return;

    if (evalArith2Node<glm::mat4, PortDataMat4, glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::mat4 &a, const glm::vec4 &b) { return PortDataVec4 { a * b }; }))
        return;

    if (evalArith2Node<glm::vec4, PortDataVec4, glm::mat4, PortDataMat4>(g, n, evalStack, [](const glm::vec4 &a, const glm::mat4 &b) { return PortDataVec4 { a * b }; }))
        return;

    pushArith2Result(evalStack, n, PortData { });
}

void evalDivNode(Graph &g, Node &n, std::vector<PortData>& evalStack)
{
    if (evalArith2Node<float, PortDataFloat, float, PortDataFloat>(g, n, evalStack, [](float a, float b) { return PortDataFloat { a / b }; }))
        return;

    if (evalArith2Node<glm::vec2, PortDataVec2, glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a, const glm::vec2 &b) { return PortDataVec2 { a / b }; }))
        return;

    if (evalArith2Node<glm::vec3, PortDataVec3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a, const glm::vec3 &b) { return PortDataVec3 { a / b }; }))
        return;

    if (evalArith2Node<glm::vec4, PortDataVec4, glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a, const glm::vec4 &b) { return PortDataVec4 { a / b }; }))
        return;

    pushArith2Result(evalStack, n, PortData { });
}

static inline void pushArith1Result(std::vector<PortData>& evalStack, Node &n, const PortData &result)
{
    evalStack.pop_back();
    evalStack.push_back(result);
    outPort(n).data = result;
}

template<typename T, typename V>
inline bool evalArith1Node(Graph &g, Node &n, std::vector<PortData>& evalStack, std::function<PortDataVar(const T &a)> f)
{
    const PortData src = evalStack.back();

    if (std::holds_alternative<V>(src.d)) {
        const T a = std::get<V>(src.d).v;
        PortData result;
        result.d = f(a);
        pushArith1Result(evalStack, n, result);
        return true;
    }

    return false;
}

void evalNegateNode(Graph &g, Node &n, std::vector<PortData>& evalStack)
{
    if (evalArith1Node<float, PortDataFloat>(g, n, evalStack, [](float a) { return PortDataFloat { -a }; }))
        return;

    if (evalArith1Node<glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a) { return PortDataVec2 { -a }; }))
        return;

    if (evalArith1Node<glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a) { return PortDataVec3 { -a }; }))
        return;

    if (evalArith1Node<glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a) { return PortDataVec4 { -a }; }))
        return;

    pushArith1Result(evalStack, n, PortData { });
}

} // namespace
