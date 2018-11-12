#ifndef HOMURA_COMMON_H_
#define HOMURA_COMMON_H_

namespace Homura {

static constexpr float PI = 3.1415926536f;
static constexpr float TWOPI = 6.28318530718f;
static constexpr float INVPI = 0.31830988618379067154;
static constexpr float INV2PI = 0.15915494309189533577f;


    template <typename T, typename L, typename H>
    inline T clamp(T value, L low, H high) {
        if (value < low) return low;
        else if (value > high) return high;
        else return value;
    }

}
#endif	// !HOMURA_COMMON_H_
