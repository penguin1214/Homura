#ifndef HOMURA_MATERIAL_H_
#define HOMURA_MATERIAL_H_

#include "renderer/IntersectInfo.h"

namespace Homura {
    class Material {
    public:
        virtual void computeScatteringFunction(IntersectInfo &isect_info) const = 0;
    };

    class MatteMaterial : public Material {
    public:
        void computeScatteringFunction(IntersectInfo &isect_info) const override;
    };
}

#endif //HOMURA_MATERIAL_H_
