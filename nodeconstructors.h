#ifndef NODECTORS_H
#define NODECTORS_H

#include "graph.h"

namespace NodeConstructors {

Id constructFloatNode(Graph *g);
Id constructVec2Node(Graph *g);
Id constructVec3Node(Graph *g);
Id constructVec4Node(Graph *g);

Id constructPlusNode(Graph *g);
Id constructMinusNode(Graph *g);
Id constructMulNode(Graph *g);
Id constructDivNode(Graph *g);
Id constructNegateNode(Graph *g);

} // namespace

struct NodeConstructor
{
    const char *text;
    Id (*func)(Graph *);
};

struct NodeConstructorSet
{
    char *category;
    NodeConstructor *constructors;
};

static NodeConstructor nodeConstructors_const[] = {
    { "Float", NodeConstructors::constructFloatNode },
    { "Vec2", NodeConstructors::constructVec2Node },
    { "Vec3", NodeConstructors::constructVec3Node },
    { "Vec4", NodeConstructors::constructVec4Node },
    { nullptr, nullptr }
};

static NodeConstructor nodeConstructors_arith[] = {
    { "Plus", NodeConstructors::constructPlusNode },
    { "Minus", NodeConstructors::constructMinusNode },
    { "Mul", NodeConstructors::constructMulNode },
    { "Div", NodeConstructors::constructDivNode },
    { "Negate", NodeConstructors::constructNegateNode },
    { nullptr, nullptr }
};

static NodeConstructorSet nodeConstructorSets[] = {
    { "Constants", nodeConstructors_const },
    { "Arithmetic", nodeConstructors_arith },
    { nullptr, nullptr }
};

#endif
