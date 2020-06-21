#include "nodeconstructors.h"
#include "grapheval.h"

namespace NodeConstructors {

static inline Node &newNode(Graph *g, const char *typeName, NodeType type, Node::EvalFunc f)
{
    Node &n(g->newNode());
    n.type = type;
    n.evalFunc = f;
    char s[128];
    sprintf(s, "%s [%d]", typeName, n.id);
    n.text = s;
    return n;
}

static inline void addConstantPorts(Graph *g, Node &n, const PortDataVar &d)
{
    n.inputPortCount = 0;
    {
        Port &port = g->addPort(n, PortDirection::Static);
        port.order = 0;
        port.text = "Value";
        port.data.d = d;
    }
    {
        Port &port = g->addPort(n, PortDirection::Output);
        port.order = 1;
        port.text = "Result";
    }
}

Id constructFloatNode(Graph *g)
{
    Node &n(newNode(g, "Float", NodeType::Float, GraphEval::evalConstantNode));
    addConstantPorts(g, n, PortDataFloat { 0.0f });
    return n.id;
}

Id constructVec2Node(Graph *g)
{
    Node &n(newNode(g, "Vec2", NodeType::Vec2, GraphEval::evalConstantNode));
    addConstantPorts(g, n, PortDataVec2 { glm::vec2() });
    return n.id;
}

Id constructVec3Node(Graph *g)
{
    Node &n(newNode(g, "Vec3", NodeType::Vec3, GraphEval::evalConstantNode));
    addConstantPorts(g, n, PortDataVec3 { glm::vec3() });
    return n.id;
}

Id constructVec4Node(Graph *g)
{
    Node &n(newNode(g, "Vec4", NodeType::Vec4, GraphEval::evalConstantNode));
    addConstantPorts(g, n, PortDataVec4 { glm::vec4() });
    return n.id;
}

Id constructMat3Node(Graph *g)
{
    Node &n(newNode(g, "Mat3", NodeType::Mat3, GraphEval::evalConstantNode));
    addConstantPorts(g, n, PortDataMat3 { glm::mat3(1, 0, 0, 0, 1, 0, 0, 0, 1) });
    return n.id;
}

Id constructMat4Node(Graph *g)
{
    Node &n(newNode(g, "Mat4", NodeType::Mat4, GraphEval::evalConstantNode));
    addConstantPorts(g, n, PortDataMat4 { glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) });
    return n.id;
}

static inline void addOp2Ports(Graph *g, Node &n)
{
    n.inputPortCount = 2;
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 0;
        port.text = "LHS";
    }
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 1;
        port.text = "RHS";
    }
    {
        Port &port = g->addPort(n, PortDirection::Output);
        port.order = 2;
        port.text = "Result";
    }
}

Id constructPlusNode(Graph *g)
{
    Node &n(newNode(g, "Plus", NodeType::Plus, GraphEval::evalPlusNode));
    addOp2Ports(g, n);
    return n.id;
}

Id constructMinusNode(Graph *g)
{
    Node &n(newNode(g, "Minus", NodeType::Minus, GraphEval::evalMinusNode));
    addOp2Ports(g, n);
    return n.id;
}

Id constructMulNode(Graph *g)
{
    Node &n(newNode(g, "Mul", NodeType::Mul, GraphEval::evalMulNode));
    addOp2Ports(g, n);
    return n.id;
}

Id constructDivNode(Graph *g)
{
    Node &n(newNode(g, "Div", NodeType::Div, GraphEval::evalDivNode));
    addOp2Ports(g, n);
    return n.id;
}

Id constructDistanceNode(Graph *g)
{
    Node &n(newNode(g, "Distance", NodeType::Distance, GraphEval::evalDistanceNode));
    addOp2Ports(g, n);
    return n.id;
}

Id constructDotNode(Graph *g)
{
    Node &n(newNode(g, "Dot", NodeType::Dot, GraphEval::evalDotNode));
    addOp2Ports(g, n);
    return n.id;
}

Id constructCrossNode(Graph *g)
{
    Node &n(newNode(g, "Cross", NodeType::Cross, GraphEval::evalCrossNode));
    addOp2Ports(g, n);
    return n.id;
}

static inline void addOp1Ports(Graph *g, Node &n)
{
    n.inputPortCount = 1;
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 0;
        port.text = "Input";
    }
    {
        Port &port = g->addPort(n, PortDirection::Output);
        port.order = 2;
        port.text = "Result";
    }
}

Id constructNegateNode(Graph *g)
{
    Node &n(newNode(g, "Negate", NodeType::Negate, GraphEval::evalNegateNode));
    addOp1Ports(g, n);
    return n.id;
}

Id constructVec2CastNode(Graph *g)
{
    Node &n(newNode(g, "Cast to Vec2", NodeType::Vec2Cast, GraphEval::evalVec2CastNode));
    addOp1Ports(g, n);
    return n.id;
}

Id constructVec3CastNode(Graph *g)
{
    Node &n(newNode(g, "Cast to Vec3", NodeType::Vec3Cast, GraphEval::evalVec3CastNode));
    addOp1Ports(g, n);
    return n.id;
}

Id constructVec4CastNode(Graph *g)
{
    Node &n(newNode(g, "Cast to Vec4", NodeType::Vec4Cast, GraphEval::evalVec4CastNode));
    addOp1Ports(g, n);
    return n.id;
}

Id constructMat3CastNode(Graph *g)
{
    Node &n(newNode(g, "Cast to Mat3", NodeType::Mat3Cast, GraphEval::evalMat3CastNode));
    addOp1Ports(g, n);
    return n.id;
}

Id constructMat4CastNode(Graph *g)
{
    Node &n(newNode(g, "Cast to Mat4", NodeType::Mat4Cast, GraphEval::evalMat4CastNode));
    addOp1Ports(g, n);
    return n.id;
}

Id constructLengthNode(Graph *g)
{
    Node &n(newNode(g, "Length", NodeType::Length, GraphEval::evalLengthNode));
    addOp1Ports(g, n);
    return n.id;
}

Id constructNormalizeNode(Graph *g)
{
    Node &n(newNode(g, "Normalize", NodeType::Normalize, GraphEval::evalNormalizeNode));
    addOp1Ports(g, n);
    return n.id;
}

Id constructTransposeNode(Graph *g)
{
    Node &n(newNode(g, "Transpose", NodeType::Transpose, GraphEval::evalTransposeNode));
    addOp1Ports(g, n);
    return n.id;
}

Id constructInverseNode(Graph *g)
{
    Node &n(newNode(g, "Inverse", NodeType::Inverse, GraphEval::evalInverseNode));
    addOp1Ports(g, n);
    return n.id;
}

Id constructDeterminantNode(Graph *g)
{
    Node &n(newNode(g, "Determinant", NodeType::Determinant, GraphEval::evalDeterminantNode));
    addOp1Ports(g, n);
    return n.id;
}

Id constructVec2CombineNode(Graph *g)
{
    Node &n(newNode(g, "Combine into Vec2", NodeType::Vec2Combine, GraphEval::evalVec2CombineNode));
    n.inputPortCount = 2;
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 0;
        port.text = "X (R)";
    }
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 1;
        port.text = "Y (G)";
    }
    {
        Port &port = g->addPort(n, PortDirection::Output);
        port.order = 2;
        port.text = "Result";
    }
    return n.id;
}

Id constructVec3CombineNode(Graph *g)
{
    Node &n(newNode(g, "Combine into Vec3", NodeType::Vec3Combine, GraphEval::evalVec3CombineNode));
    n.inputPortCount = 3;
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 0;
        port.text = "X (R)";
    }
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 1;
        port.text = "Y (G)";
    }
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 2;
        port.text = "Z (B)";
    }
    {
        Port &port = g->addPort(n, PortDirection::Output);
        port.order = 3;
        port.text = "Result";
    }
    return n.id;
}

Id constructVec4CombineNode(Graph *g)
{
    Node &n(newNode(g, "Combine into Vec4", NodeType::Vec4Combine, GraphEval::evalVec4CombineNode));
    n.inputPortCount = 4;
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 0;
        port.text = "X (R)";
    }
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 1;
        port.text = "Y (G)";
    }
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 2;
        port.text = "Z (B)";
    }
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 3;
        port.text = "W (A)";
    }
    {
        Port &port = g->addPort(n, PortDirection::Output);
        port.order = 4;
        port.text = "Result";
    }
    return n.id;
}

Id constructSwizzleNode(Graph *g)
{
    Node &n(newNode(g, "Swizzle", NodeType::Swizzle, GraphEval::evalSwizzleNode));
    n.inputPortCount = 1;
    {
        Port &port = g->addPort(n, PortDirection::Input);
        port.order = 0;
        port.text = "Vector";
    }
    {
        Port &port = g->addPort(n, PortDirection::Static);
        port.order = 1;
        port.text = "Swizzle";
        port.data.d = PortDataString { "xyzw" };
    }
    {
        Port &port = g->addPort(n, PortDirection::Output);
        port.order = 2;
        port.text = "Result";
    }
    return n.id;
}

} // namespace
