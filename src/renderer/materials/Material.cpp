#include "renderer/materials/Material.h"
#include "renderer/bxdfs/Lambert.h"

namespace Homura {
    void MatteMaterial::computeScatteringFunction(IntersectInfo &isect_info) const {
        isect_info._bsdf.reset((new BSDF(isect_info)));
        isect_info._bsdf->add(std::unique_ptr<BxDF>(new LambertReflection(0.8f/*TODO*/)));  /// TODO: textures
    }
}