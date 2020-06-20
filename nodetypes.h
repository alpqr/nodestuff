#ifndef GRAPHSEM_H
#define GRAPHSEM_H

enum class NodeType
{
    Invalid = 0,

    Float,
    Vec2,
    Vec3,
    Vec4,
    Mat3,
    Mat4,

    Plus,
    Minus,
    Mul,
    Div,
    Negate
};

#endif
