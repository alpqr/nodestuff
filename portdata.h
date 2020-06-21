#ifndef PORTDATA_H
#define PORTDATA_H

#include <string>
#include <variant>
#include "glm/gtc/type_ptr.hpp"

struct PortDataEmpty { };
struct PortDataFloat { float v; };
struct PortDataVec2 { glm::vec2 v; };
struct PortDataVec3 { glm::vec3 v; };
struct PortDataVec4 { glm::vec4 v; };
struct PortDataMat3 { glm::mat3 v; bool editAsRowMajor; };
struct PortDataMat4 { glm::mat4 v; bool editAsRowMajor; };
struct PortDataString { std::string v; };

using PortDataVar = std::variant<
    PortDataEmpty,
    PortDataFloat,
    PortDataVec2,
    PortDataVec3,
    PortDataVec4,
    PortDataMat3,
    PortDataMat4,
    PortDataString
>;

struct PortData
{
    PortDataVar d = PortDataEmpty { };
    std::string desc;

    static PortData notEnoughArgsResult() { return PortData { PortDataEmpty { }, "Not enough arguments" }; }
    static PortData invalidArgsResult() { return PortData { PortDataEmpty { }, "Invalid arguments" }; }
};

#endif
