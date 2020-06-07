#ifndef NODECTORS_H
#define NODECTORS_H

#include "graph.h"

namespace NodeConstructors {

Id constructFloatNode(Graph *g);
Id constructIntNode(Graph *g);
Id constructUIntNode(Graph *g);

Id constructPlusNode(Graph *g);
Id constructMinusNode(Graph *g);

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
    { "Int", NodeConstructors::constructIntNode },
    { "UInt", NodeConstructors::constructUIntNode },
    { nullptr, nullptr }
};

static NodeConstructor nodeConstructors_arith[] = {
    { "Plus", NodeConstructors::constructPlusNode },
    { "Minus", NodeConstructors::constructMinusNode },
    { nullptr, nullptr }
};

static NodeConstructorSet nodeConstructorSets[] = {
    { "Constants", nodeConstructors_const },
    { "Arithmetic", nodeConstructors_arith },
    { nullptr, nullptr }
};

#endif
