#ifndef PORTDATA_H
#define PORTDATA_H

#include <variant>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"

struct PortDataEmpty { };
struct PortDataFloat { float v; };
struct PortDataVec2 { glm::vec2 v; };
struct PortDataVec3 { glm::vec3 v; };
struct PortDataVec4 { glm::vec4 v; };
struct PortDataMat3 { glm::mat3 v; };
struct PortDataMat4 { glm::mat4 v; };

using PortDataVar = std::variant<
    PortDataEmpty,
    PortDataFloat,
    PortDataVec2,
    PortDataVec3,
    PortDataVec4,
    PortDataMat3,
    PortDataMat4
>;

struct PortData
{
    PortDataVar d = PortDataEmpty { };
};

#endif
