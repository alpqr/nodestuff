#ifndef PORTDATA_H
#define PORTDATA_H

#include <variant>
#include <array>
#include <stdint.h>

struct PortDataEmpty { };
struct PortDataFloat { float v; };
struct PortDataVec2 { std::array<float, 2> v; };
struct PortDataVec3 { std::array<float, 3> v; };
struct PortDataVec4 { std::array<float, 4> v; };

using PortDataVar = std::variant<
    PortDataEmpty,
    PortDataFloat,
    PortDataVec2,
    PortDataVec3,
    PortDataVec4
>;

struct PortData
{
    PortDataVar d = PortDataEmpty { };
};

#endif
