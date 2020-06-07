#include "nodeconstructors.h"
#include "grapheval.h"

namespace NodeConstructors {

Id constructFloatNode(Graph *g)
{
    Node &n(g->newNode());
    n.type = NodeType::Float;
    n.inputPortCount = 0;
    n.evalFunc = GraphEval::evalConstantNode;
    char s[128];
    sprintf(s, "Float [%d]", n.id);
    n.text = s;

    {
        Port &port = g->addPort(n, PortDirection::Static);
        port.order = 0;
        port.text = "Value";
        port.data.d = PortDataFloat { 0.0f };
    }
    {
        Port &port = g->addPort(n, PortDirection::Output);
        port.order = 1;
        port.text = "Result";
    }

    return n.id;
}

Id constructIntNode(Graph *g)
{
    Node &n(g->newNode());
    n.type = NodeType::Int;
    n.inputPortCount = 0;
    n.evalFunc = GraphEval::evalConstantNode;
    char s[128];
    sprintf(s, "Int [%d]", n.id);
    n.text = s;

    {
        Port &port = g->addPort(n, PortDirection::Static);
        port.order = 0;
        port.text = "Value";
        port.data.d = PortDataInt { 0 };
    }
    {
        Port &port = g->addPort(n, PortDirection::Output);
        port.order = 1;
        port.text = "Result";
    }

    return n.id;
}

Id constructUIntNode(Graph *g)
{
    Node &n(g->newNode());
    n.type = NodeType::UInt;
    n.inputPortCount = 0;
    n.evalFunc = GraphEval::evalConstantNode;
    char s[128];
    sprintf(s, "UInt [%d]", n.id);
    n.text = s;

    {
        Port &port = g->addPort(n, PortDirection::Static);
        port.order = 0;
        port.text = "Value";
        port.data.d = PortDataUInt { 0 };
    }
    {
        Port &port = g->addPort(n, PortDirection::Output);
        port.order = 1;
        port.text = "Result";
    }

    return n.id;
}

Id constructPlusNode(Graph *g)
{
    Node &n(g->newNode());
    n.type = NodeType::Plus;
    n.inputPortCount = 2;
    n.evalFunc = GraphEval::evalPlusNode;
    char s[128];
    sprintf(s, "Plus [%d]", n.id);
    n.text = s;

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

    return n.id;
}

Id constructMinusNode(Graph *g)
{
    Node &n(g->newNode());
    n.type = NodeType::Minus;
    n.inputPortCount = 2;
    n.evalFunc = GraphEval::evalMinusNode;
    char s[128];
    sprintf(s, "Minus [%d]", n.id);
    n.text = s;

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

    return n.id;
}

} // namespace
