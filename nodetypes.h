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

    Vec2Cast,
    Vec3Cast,
    Vec4Cast,
    Mat3Cast,
    Mat4Cast,
    Vec2Combine,
    Vec3Combine,
    Vec4Combine,
    Swizzle,

    Plus,
    Minus,
    Mul,
    Div,
    Negate,

    Length,
    Distance,
    Dot,
    Cross,
    Normalize,

    Transpose,
    Inverse,
    Determinant
};

#endif
