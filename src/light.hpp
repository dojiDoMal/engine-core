#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <cstdint>

enum class LightType : uint8_t {
    DIRECTIONAL = 0,
    POINT = 1,
    SPOT = 2
};

struct Light {
    LightType type;
    float direction[3];
    //float position[3];
};

#endif
