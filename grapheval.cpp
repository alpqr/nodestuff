#include "grapheval.h"
#include "graph.h"
#include <unordered_set>

namespace GraphEval {

static inline Port *outPort(Node &node)
{
    auto outPort = std::find_if(node.ports.begin(), node.ports.end(),
        [](const Port &port) { return port.dir == PortDirection::Output; });
    return outPort != node.ports.end() ? &*outPort : nullptr;
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
                if (Port *oprt = outPort(node)) {
                    PortData result = PortData::notEnoughArgsResult();
                    oprt->data = result;
                    evalStack.push_back(result);
                }
            } else if (evaluated.find(cur.id) != evaluated.end()) {
                // already evaluated in this frame
                for (size_t i = 0; i < node.inputPortCount; ++i)
                    evalStack.pop_back();
                if (Port *oprt = outPort(node))
                    evalStack.push_back(oprt->data);
            } else {
                evaluated.insert(cur.id);
                if (node.evalFunc)
                    node.evalFunc(g, node, evalStack);
            }
        }
    }
}

void evalConstantNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    auto src = std::find_if(n.ports.cbegin(), n.ports.cend(), [](const Port &port) { return port.dir == PortDirection::Static; });
    evalStack.push_back(src->data);
    outPort(n)->data = src->data;
}

static inline void pushOpResult(EvalStackType &evalStack, Node &n, const PortData &result, int argCount)
{
    for (int i = 0; i < argCount; ++i)
        evalStack.pop_back();
    evalStack.push_back(result);
    outPort(n)->data = result;
}

template<typename T, typename V>
inline bool evalOp1Node(Graph &g, Node &n, EvalStackType &evalStack, std::function<PortDataVar(const T &a)> f)
{
    const PortData src = evalStack.back();

    if (std::holds_alternative<V>(src.d)) {
        const T a = std::get<V>(src.d).v;
        PortData result;
        result.d = f(a);
        pushOpResult(evalStack, n, result, 1);
        return true;
    }

    return false;
}

template<typename T_LHS, typename V_LHS, typename T_RHS, typename V_RHS>
inline bool evalOp2Node(Graph &g, Node &n, EvalStackType &evalStack, std::function<PortDataVar(const T_LHS &a, const T_RHS &b)> f)
{
    const PortData rhs = evalStack.back();
    const PortData lhs = evalStack[evalStack.size() - 2];

    if (std::holds_alternative<V_LHS>(lhs.d) && std::holds_alternative<V_RHS>(rhs.d)) {
        const T_LHS a = std::get<V_LHS>(lhs.d).v;
        const T_RHS b = std::get<V_RHS>(rhs.d).v;
        PortData result;
        result.d = f(a, b);
        pushOpResult(evalStack, n, result, 2);
        return true;
    }

    return false;
}

template<typename T_A, typename V_A, typename T_B, typename V_B, typename T_C, typename V_C>
inline bool evalOp3Node(Graph &g, Node &n, EvalStackType &evalStack, std::function<PortDataVar(const T_A &a, const T_B &b, const T_C &c)> f)
{
    const PortData vc = evalStack.back();
    const PortData vb = evalStack[evalStack.size() - 2];
    const PortData va = evalStack[evalStack.size() - 3];

    if (std::holds_alternative<V_A>(va.d) && std::holds_alternative<V_B>(vb.d) && std::holds_alternative<V_C>(vc.d)) {
        const T_A a = std::get<V_A>(va.d).v;
        const T_B b = std::get<V_B>(vb.d).v;
        const T_C c = std::get<V_C>(vc.d).v;
        PortData result;
        result.d = f(a, b, c);
        pushOpResult(evalStack, n, result, 3);
        return true;
    }

    return false;
}

template<typename T_A, typename V_A, typename T_B, typename V_B, typename T_C, typename V_C, typename T_D, typename V_D>
inline bool evalOp4Node(Graph &g, Node &n, EvalStackType &evalStack, std::function<PortDataVar(const T_A &a, const T_B &b, const T_C &c, const T_D &d)> f)
{
    const PortData vd = evalStack.back();
    const PortData vc = evalStack[evalStack.size() - 2];
    const PortData vb = evalStack[evalStack.size() - 3];
    const PortData va = evalStack[evalStack.size() - 4];

    if (std::holds_alternative<V_A>(va.d) && std::holds_alternative<V_B>(vb.d) && std::holds_alternative<V_C>(vc.d) && std::holds_alternative<V_D>(vd.d)) {
        const T_A a = std::get<V_A>(va.d).v;
        const T_B b = std::get<V_B>(vb.d).v;
        const T_C c = std::get<V_C>(vc.d).v;
        const T_D d = std::get<V_D>(vd.d).v;
        PortData result;
        result.d = f(a, b, c, d);
        pushOpResult(evalStack, n, result, 4);
        return true;
    }

    return false;
}

void evalPlusNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp2Node<float, PortDataFloat, float, PortDataFloat>(g, n, evalStack, [](float a, float b) { return PortDataFloat { a + b }; }))
        return;

    if (evalOp2Node<glm::vec2, PortDataVec2, glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a, const glm::vec2 &b) { return PortDataVec2 { a + b }; }))
        return;

    if (evalOp2Node<glm::vec3, PortDataVec3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a, const glm::vec3 &b) { return PortDataVec3 { a + b }; }))
        return;

    if (evalOp2Node<glm::vec4, PortDataVec4, glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a, const glm::vec4 &b) { return PortDataVec4 { a + b }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 2);
}

void evalMinusNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp2Node<float, PortDataFloat, float, PortDataFloat>(g, n, evalStack, [](float a, float b) { return PortDataFloat { a - b }; }))
        return;

    if (evalOp2Node<glm::vec2, PortDataVec2, glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a, const glm::vec2 &b) { return PortDataVec2 { a - b }; }))
        return;

    if (evalOp2Node<glm::vec3, PortDataVec3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a, const glm::vec3 &b) { return PortDataVec3 { a - b }; }))
        return;

    if (evalOp2Node<glm::vec4, PortDataVec4, glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a, const glm::vec4 &b) { return PortDataVec4 { a - b }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 2);
}

void evalMulNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp2Node<float, PortDataFloat, float, PortDataFloat>(g, n, evalStack, [](float a, float b) { return PortDataFloat { a * b }; }))
        return;

    if (evalOp2Node<glm::vec2, PortDataVec2, glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a, const glm::vec2 &b) { return PortDataVec2 { a * b }; }))
        return;

    if (evalOp2Node<glm::vec2, PortDataVec2, float, PortDataFloat>(g, n, evalStack, [](const glm::vec2 &a, float b) { return PortDataVec2 { a * b }; }))
        return;

    if (evalOp2Node<float, PortDataFloat, glm::vec2, PortDataVec2>(g, n, evalStack, [](float a, const glm::vec2 &b) { return PortDataVec2 { a * b }; }))
        return;

    if (evalOp2Node<glm::vec3, PortDataVec3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a, const glm::vec3 &b) { return PortDataVec3 { a * b }; }))
        return;

    if (evalOp2Node<glm::vec3, PortDataVec3, float, PortDataFloat>(g, n, evalStack, [](const glm::vec3 &a, float b) { return PortDataVec3 { a * b }; }))
        return;

    if (evalOp2Node<float, PortDataFloat, glm::vec3, PortDataVec3>(g, n, evalStack, [](float a, const glm::vec3 &b) { return PortDataVec3 { a * b }; }))
        return;

    if (evalOp2Node<glm::vec4, PortDataVec4, glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a, const glm::vec4 &b) { return PortDataVec4 { a * b }; }))
        return;

    if (evalOp2Node<glm::vec4, PortDataVec4, float, PortDataFloat>(g, n, evalStack, [](const glm::vec4 &a, float b) { return PortDataVec4 { a * b }; }))
        return;

    if (evalOp2Node<float, PortDataFloat, glm::vec4, PortDataVec4>(g, n, evalStack, [](float a, const glm::vec4 &b) { return PortDataVec4 { a * b }; }))
        return;

    if (evalOp2Node<glm::mat3, PortDataMat3, glm::mat3, PortDataMat3>(g, n, evalStack, [](const glm::mat3 &a, const glm::mat3 &b) { return PortDataMat3 { a * b }; }))
        return;

    if (evalOp2Node<glm::mat3, PortDataMat3, float, PortDataFloat>(g, n, evalStack, [](const glm::mat3 &a, float b) { return PortDataMat3 { a * b }; }))
        return;

    if (evalOp2Node<float, PortDataFloat, glm::mat3, PortDataMat3>(g, n, evalStack, [](float a, const glm::mat3 &b) { return PortDataMat3 { a * b }; }))
        return;

    if (evalOp2Node<glm::mat3, PortDataMat3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::mat3 &a, const glm::vec3 &b) { return PortDataVec3 { a * b }; }))
        return;

    if (evalOp2Node<glm::vec3, PortDataVec3, glm::mat3, PortDataMat3>(g, n, evalStack, [](const glm::vec3 &a, const glm::mat3 &b) { return PortDataVec3 { a * b }; }))
        return;

    if (evalOp2Node<glm::mat4, PortDataMat4, glm::mat4, PortDataMat4>(g, n, evalStack, [](const glm::mat4 &a, const glm::mat4 &b) { return PortDataMat4 { a * b }; }))
        return;

    if (evalOp2Node<glm::mat4, PortDataMat4, float, PortDataFloat>(g, n, evalStack, [](const glm::mat4 &a, float b) { return PortDataMat4 { a * b }; }))
        return;

    if (evalOp2Node<float, PortDataFloat, glm::mat4, PortDataMat4>(g, n, evalStack, [](float a, const glm::mat4 &b) { return PortDataMat4 { a * b }; }))
        return;

    if (evalOp2Node<glm::mat4, PortDataMat4, glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::mat4 &a, const glm::vec4 &b) { return PortDataVec4 { a * b }; }))
        return;

    if (evalOp2Node<glm::vec4, PortDataVec4, glm::mat4, PortDataMat4>(g, n, evalStack, [](const glm::vec4 &a, const glm::mat4 &b) { return PortDataVec4 { a * b }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 2);
}

void evalDivNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp2Node<float, PortDataFloat, float, PortDataFloat>(g, n, evalStack, [](float a, float b) { return PortDataFloat { a / b }; }))
        return;

    if (evalOp2Node<glm::vec2, PortDataVec2, glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a, const glm::vec2 &b) { return PortDataVec2 { a / b }; }))
        return;

    if (evalOp2Node<glm::vec3, PortDataVec3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a, const glm::vec3 &b) { return PortDataVec3 { a / b }; }))
        return;

    if (evalOp2Node<glm::vec4, PortDataVec4, glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a, const glm::vec4 &b) { return PortDataVec4 { a / b }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 2);
}

void evalDistanceNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp2Node<glm::vec2, PortDataVec2, glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a, const glm::vec2 &b) { return PortDataFloat { glm::distance(a, b) }; }))
        return;

    if (evalOp2Node<glm::vec3, PortDataVec3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a, const glm::vec3 &b) { return PortDataFloat { glm::distance(a, b) }; }))
        return;

    if (evalOp2Node<glm::vec4, PortDataVec4, glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a, const glm::vec4 &b) { return PortDataFloat { glm::distance(a, b) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 2);
}

void evalDotNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp2Node<glm::vec2, PortDataVec2, glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a, const glm::vec2 &b) { return PortDataFloat { glm::dot(a, b) }; }))
        return;

    if (evalOp2Node<glm::vec3, PortDataVec3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a, const glm::vec3 &b) { return PortDataFloat { glm::dot(a, b) }; }))
        return;

    if (evalOp2Node<glm::vec4, PortDataVec4, glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a, const glm::vec4 &b) { return PortDataFloat { glm::dot(a, b) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 2);
}

void evalCrossNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp2Node<glm::vec3, PortDataVec3, glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a, const glm::vec3 &b) { return PortDataVec3 { glm::cross(a, b) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 2);
}

void evalNegateNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp1Node<float, PortDataFloat>(g, n, evalStack, [](float a) { return PortDataFloat { -a }; }))
        return;

    if (evalOp1Node<glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a) { return PortDataVec2 { -a }; }))
        return;

    if (evalOp1Node<glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a) { return PortDataVec3 { -a }; }))
        return;

    if (evalOp1Node<glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a) { return PortDataVec4 { -a }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

void evalVec2CastNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp1Node<glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a) { return PortDataVec2 { a }; }))
        return;

    if (evalOp1Node<float, PortDataFloat>(g, n, evalStack, [](float a) { return PortDataVec2 { glm::vec2(a) }; }))
        return;

    if (evalOp1Node<glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a) { return PortDataVec2 { glm::vec2(a) }; }))
        return;

    if (evalOp1Node<glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a) { return PortDataVec2 { glm::vec2(a) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

void evalVec3CastNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp1Node<glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a) { return PortDataVec3 { a }; }))
        return;

    if (evalOp1Node<float, PortDataFloat>(g, n, evalStack, [](float a) { return PortDataVec3 { glm::vec3(a) }; }))
        return;

    if (evalOp1Node<glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a) { return PortDataVec3 { glm::vec3(a.x, a.y, 0.0f) }; }))
        return;

    if (evalOp1Node<glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a) { return PortDataVec3 { glm::vec3(a.x, a.y, a.z) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

void evalVec4CastNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp1Node<glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a) { return PortDataVec4 { a }; }))
        return;

    if (evalOp1Node<float, PortDataFloat>(g, n, evalStack, [](float a) { return PortDataVec4 { glm::vec4(a) }; }))
        return;

    if (evalOp1Node<glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a) { return PortDataVec4 { glm::vec4(a.x, a.y, 0.0f, 0.0f) }; }))
        return;

    if (evalOp1Node<glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a) { return PortDataVec4 { glm::vec4(a.x, a.y, a.z, 0.0f) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

void evalMat3CastNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp1Node<glm::mat3, PortDataMat3>(g, n, evalStack, [](const glm::mat3 &a) { return PortDataMat3 { a }; }))
        return;

    if (evalOp1Node<glm::mat4, PortDataMat4>(g, n, evalStack, [](const glm::mat4 &a) { return PortDataMat3 { glm::mat3(a) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

void evalMat4CastNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp1Node<glm::mat4, PortDataMat4>(g, n, evalStack, [](const glm::mat4 &a) { return PortDataMat4 { a }; }))
        return;

    if (evalOp1Node<glm::mat3, PortDataMat3>(g, n, evalStack, [](const glm::mat3 &a) { return PortDataMat4 { glm::mat4(a) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

void evalLengthNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp1Node<glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a) { return PortDataFloat { glm::length(a) }; }))
        return;

    if (evalOp1Node<glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a) { return PortDataFloat { glm::length(a) }; }))
        return;

    if (evalOp1Node<glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a) { return PortDataFloat { glm::length(a) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

void evalNormalizeNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp1Node<glm::vec2, PortDataVec2>(g, n, evalStack, [](const glm::vec2 &a) { return PortDataVec2 { glm::normalize(a) }; }))
        return;

    if (evalOp1Node<glm::vec3, PortDataVec3>(g, n, evalStack, [](const glm::vec3 &a) { return PortDataVec3 { glm::normalize(a) }; }))
        return;

    if (evalOp1Node<glm::vec4, PortDataVec4>(g, n, evalStack, [](const glm::vec4 &a) { return PortDataVec4 { glm::normalize(a) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

void evalTransposeNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp1Node<glm::mat3, PortDataMat3>(g, n, evalStack, [](const glm::mat3 &a) { return PortDataMat3 { glm::transpose(a) }; }))
        return;

    if (evalOp1Node<glm::mat4, PortDataMat4>(g, n, evalStack, [](const glm::mat4 &a) { return PortDataMat4 { glm::transpose(a) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

void evalInverseNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp1Node<glm::mat3, PortDataMat3>(g, n, evalStack, [](const glm::mat3 &a) { return PortDataMat3 { glm::inverse(a) }; }))
        return;

    if (evalOp1Node<glm::mat4, PortDataMat4>(g, n, evalStack, [](const glm::mat4 &a) { return PortDataMat4 { glm::inverse(a) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

void evalDeterminantNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp1Node<glm::mat3, PortDataMat3>(g, n, evalStack, [](const glm::mat3 &a) { return PortDataFloat { glm::determinant(a) }; }))
        return;

    if (evalOp1Node<glm::mat4, PortDataMat4>(g, n, evalStack, [](const glm::mat4 &a) { return PortDataFloat { glm::determinant(a) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

void evalVec2CombineNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp2Node<float, PortDataFloat, float, PortDataFloat>(g, n, evalStack, [](float a, float b) { return PortDataVec2 { glm::vec2(a, b) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 2);
}

void evalVec3CombineNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp3Node<float, PortDataFloat, float, PortDataFloat, float, PortDataFloat>(g, n, evalStack,
            [](float a, float b, float c) { return PortDataVec3 { glm::vec3(a, b, c) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

void evalVec4CombineNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    if (evalOp4Node<float, PortDataFloat, float, PortDataFloat, float, PortDataFloat, float, PortDataFloat>(g, n, evalStack,
            [](float a, float b, float c, float d) { return PortDataVec4 { glm::vec4(a, b, c, d) }; }))
        return;

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

static inline size_t componentIndexForSwizzle(char c)
{
    switch (c) {
        case 'r':
        case 'x':
        case 's':
            return 0;
        case 'g':
        case 'y':
        case 't':
            return 1;
        case 'b':
        case 'z':
            return 2;
        case 'a':
        case 'w':
            return 3;
    }
    return INT_MAX;
}

static inline float swizzleToFloat(const float *src, size_t srcCompCount, const char *swizzle)
{
    const size_t idx = componentIndexForSwizzle(swizzle[0]);
    return idx < srcCompCount ? src[idx] : 0.0f;
}

static inline glm::vec2 swizzleToVec2(const float *src, size_t srcCompCount, const char *swizzle)
{
    const size_t idx0 = componentIndexForSwizzle(swizzle[0]);
    const size_t idx1 = componentIndexForSwizzle(swizzle[1]);
    const float a = idx0 < srcCompCount ? src[idx0] : 0.0f;
    const float b = idx1 < srcCompCount ? src[idx1] : 0.0f;
    return glm::vec2(a, b);
}

static inline glm::vec3 swizzleToVec3(const float *src, size_t srcCompCount, const char *swizzle)
{
    const size_t idx0 = componentIndexForSwizzle(swizzle[0]);
    const size_t idx1 = componentIndexForSwizzle(swizzle[1]);
    const size_t idx2 = componentIndexForSwizzle(swizzle[2]);
    const float a = idx0 < srcCompCount ? src[idx0] : 0.0f;
    const float b = idx1 < srcCompCount ? src[idx1] : 0.0f;
    const float c = idx2 < srcCompCount ? src[idx2] : 0.0f;
    return glm::vec3(a, b, c);
}

static inline glm::vec4 swizzleToVec4(const float *src, size_t srcCompCount, const char *swizzle)
{
    const size_t idx0 = componentIndexForSwizzle(swizzle[0]);
    const size_t idx1 = componentIndexForSwizzle(swizzle[1]);
    const size_t idx2 = componentIndexForSwizzle(swizzle[2]);
    const size_t idx3 = componentIndexForSwizzle(swizzle[3]);
    const float a = idx0 < srcCompCount ? src[idx0] : 0.0f;
    const float b = idx1 < srcCompCount ? src[idx1] : 0.0f;
    const float c = idx2 < srcCompCount ? src[idx2] : 0.0f;
    const float d = idx3 < srcCompCount ? src[idx3] : 0.0f;
    return glm::vec4(a, b, c, d);
}

void evalSwizzleNode(Graph &g, Node &n, EvalStackType &evalStack)
{
    auto swizzleSrc = std::find_if(n.ports.cbegin(), n.ports.cend(), [](const Port &port) { return port.dir == PortDirection::Static; });
    const std::string swizzleStr = std::get<PortDataString>(swizzleSrc->data.d).v;
    const PortData src = evalStack.back();

    const size_t outCompCount = swizzleStr.size();
    if (outCompCount >= 1 && outCompCount <= 4) {
        const float *srcData = nullptr;
        size_t srcCompCount = 0;
        if (std::holds_alternative<PortDataVec2>(src.d)) {
            srcData = glm::value_ptr(std::get<PortDataVec2>(src.d).v);
            srcCompCount = 2;
        } else if (std::holds_alternative<PortDataVec3>(src.d)) {
            srcData = glm::value_ptr(std::get<PortDataVec3>(src.d).v);
            srcCompCount = 3;
        } else if (std::holds_alternative<PortDataVec4>(src.d)) {
            srcData = glm::value_ptr(std::get<PortDataVec4>(src.d).v);
            srcCompCount = 4;
        }
        if (srcData) {
            PortData result;
            switch (outCompCount) {
                case 1:
                    result.d = PortDataFloat { swizzleToFloat(srcData, srcCompCount, swizzleStr.c_str()) };
                    break;
                case 2:
                    result.d = PortDataVec2 { swizzleToVec2(srcData, srcCompCount, swizzleStr.c_str()) };
                    break;
                case 3:
                    result.d = PortDataVec3 { swizzleToVec3(srcData, srcCompCount, swizzleStr.c_str()) };
                    break;
                case 4:
                    result.d = PortDataVec4 { swizzleToVec4(srcData, srcCompCount, swizzleStr.c_str()) };
                    break;
                default:
                    break;
            }
            pushOpResult(evalStack, n, result, 1);
            return;
        }
    }

    pushOpResult(evalStack, n, PortData::invalidArgsResult(), 1);
}

} // namespace
