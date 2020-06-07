#ifndef PORTDATA_H
#define PORTDATA_H

#include <variant>
#include <stdint.h>

struct PortDataEmpty { };
struct PortDataFloat { float v; };
struct PortDataInt { int32_t v; };
struct PortDataUInt { uint32_t v; };

using PortDataVar = std::variant<
    PortDataEmpty,
    PortDataFloat,
    PortDataInt,
    PortDataUInt
>;

struct PortData
{
    PortDataVar d = PortDataEmpty { };
};

#endif
