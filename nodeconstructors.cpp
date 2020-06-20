#include "nodeconstructors.h"
#include "grapheval.h"

namespace NodeConstructors {

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
    Node &n(g->newNode());
    n.type = NodeType::Float;
    n.evalFunc = GraphEval::evalConstantNode;
    char s[128];
    sprintf(s, "Float [%d]", n.id);
    n.text = s;
    addConstantPorts(g, n, PortDataFloat { 0.0f });
    return n.id;
}

Id constructVec2Node(Graph *g)
{
    Node &n(g->newNode());
    n.type = NodeType::Vec2;
    n.evalFunc = GraphEval::evalConstantNode;
    char s[128];
    sprintf(s, "Vec2 [%d]", n.id);
    n.text = s;
    addConstantPorts(g, n, PortDataVec2 { { 0.0f, 0.0f } });
    return n.id;
}

Id constructVec3Node(Graph *g)
{
    Node &n(g->newNode());
    n.type = NodeType::Vec3;
    n.evalFunc = GraphEval::evalConstantNode;
    char s[128];
    sprintf(s, "Vec3 [%d]", n.id);
    n.text = s;
    addConstantPorts(g, n, PortDataVec3 { { 0.0f, 0.0f, 0.0f } });
    return n.id;
}

Id constructVec4Node(Graph *g)
{
    Node &n(g->newNode());
    n.type = NodeType::Vec4;
    n.evalFunc = GraphEval::evalConstantNode;
    char s[128];
    sprintf(s, "Vec4 [%d]", n.id);
    n.text = s;
    addConstantPorts(g, n, PortDataVec4 { { 0.0f, 0.0f, 0.0f, 0.0f } });
    return n.id;
}

static inline void addArith2Ports(Graph *g, Node &n)
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
    Node &n(g->newNode());
    n.type = NodeType::Plus;
    n.evalFunc = GraphEval::evalPlusNode;
    char s[128];
    sprintf(s, "Plus [%d]", n.id);
    n.text = s;
    addArith2Ports(g, n);
    return n.id;
}

Id constructMinusNode(Graph *g)
{
    Node &n(g->newNode());
    n.type = NodeType::Minus;
    n.evalFunc = GraphEval::evalMinusNode;
    char s[128];
    sprintf(s, "Minus [%d]", n.id);
    n.text = s;
    addArith2Ports(g, n);
    return n.id;
}

Id constructMulNode(Graph *g)
{
    Node &n(g->newNode());
    n.type = NodeType::Mul;
    n.evalFunc = GraphEval::evalMulNode;
    char s[128];
    sprintf(s, "Mul [%d]", n.id);
    n.text = s;
    addArith2Ports(g, n);
    return n.id;
}

Id constructDivNode(Graph *g)
{
    Node &n(g->newNode());
    n.type = NodeType::Div;
    n.evalFunc = GraphEval::evalDivNode;
    char s[128];
    sprintf(s, "Div [%d]", n.id);
    n.text = s;
    addArith2Ports(g, n);
    return n.id;
}

static inline void addArith1Ports(Graph *g, Node &n)
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
    Node &n(g->newNode());
    n.type = NodeType::Negate;
    n.evalFunc = GraphEval::evalNegateNode;
    char s[128];
    sprintf(s, "Negate [%d]", n.id);
    n.text = s;
    addArith1Ports(g, n);
    return n.id;
}

} // namespace
