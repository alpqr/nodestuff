#ifndef NODECTORS_H
#define NODECTORS_H

#include "graph.h"

namespace NodeConstructors {

Id constructFloatNode(Graph *g);
Id constructVec2Node(Graph *g);
Id constructVec3Node(Graph *g);
Id constructVec4Node(Graph *g);
Id constructMat3Node(Graph *g);
Id constructMat4Node(Graph *g);

Id constructVec2CastNode(Graph *g);
Id constructVec3CastNode(Graph *g);
Id constructVec4CastNode(Graph *g);
Id constructMat3CastNode(Graph *g);
Id constructMat4CastNode(Graph *g);
Id constructVec2CombineNode(Graph *g);
Id constructVec3CombineNode(Graph *g);
Id constructVec4CombineNode(Graph *g);
Id constructSwizzleNode(Graph *g);

Id constructPlusNode(Graph *g);
Id constructMinusNode(Graph *g);
Id constructMulNode(Graph *g);
Id constructDivNode(Graph *g);
Id constructNegateNode(Graph *g);

Id constructLengthNode(Graph *g);
Id constructDistanceNode(Graph *g);
Id constructDotNode(Graph *g);
Id constructCrossNode(Graph *g);
Id constructNormalizeNode(Graph *g);

Id constructTransposeNode(Graph *g);
Id constructInverseNode(Graph *g);
Id constructDeterminantNode(Graph *g);

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
    { "Mat3", NodeConstructors::constructMat3Node },
    { "Mat4", NodeConstructors::constructMat4Node },
    { nullptr, nullptr }
};

static NodeConstructor nodeConstructors_comp[] = {
    { "Swizzle", NodeConstructors::constructSwizzleNode },
    { "Cast to Vec2", NodeConstructors::constructVec2CastNode },
    { "Cast to Vec3", NodeConstructors::constructVec3CastNode },
    { "Cast to Vec4", NodeConstructors::constructVec4CastNode },
    { "Cast to Mat3", NodeConstructors::constructMat3CastNode },
    { "Cast to Mat4", NodeConstructors::constructMat4CastNode },
    { "Combine into Vec2", NodeConstructors::constructVec2CombineNode },
    { "Combine into Vec3", NodeConstructors::constructVec3CombineNode },
    { "Combine into Vec4", NodeConstructors::constructVec4CombineNode },
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

static NodeConstructor nodeConstructors_vector[] = {
    { "Length", NodeConstructors::constructLengthNode },
    { "Distance", NodeConstructors::constructDistanceNode },
    { "Dot product", NodeConstructors::constructDotNode },
    { "Cross product", NodeConstructors::constructCrossNode },
    { "Normalize", NodeConstructors::constructNormalizeNode },
    { nullptr, nullptr }
};

static NodeConstructor nodeConstructors_matrix[] = {
    { "Transpose", NodeConstructors::constructTransposeNode },
    { "Inverse", NodeConstructors::constructInverseNode },
    { "Determinant", NodeConstructors::constructDeterminantNode },
    { nullptr, nullptr }
};

static NodeConstructorSet nodeConstructorSets[] = {
    { "Constant", nodeConstructors_const },
    { "Component", nodeConstructors_comp },
    { "Arithmetic", nodeConstructors_arith },
    { "Vector", nodeConstructors_vector },
    { "Matrix", nodeConstructors_matrix },
    { nullptr, nullptr }
};

#endif
