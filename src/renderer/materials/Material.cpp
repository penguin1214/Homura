#include "renderer/materials/Material.h"
#include "renderer/bxdfs/Lambert.h"
#include "renderer/bxdfs/Fresnel.h"

namespace Homura {
    void MatteMaterial::computeScatteringFunction(IntersectInfo &isect_info) const {
        isect_info._bsdf.reset((new BSDF(isect_info)));
        isect_info._bsdf->add(std::unique_ptr<BxDF>(new LambertReflection(_R)));  /// TODO: textures
    }

	void Mirror::computeScatteringFunction(IntersectInfo &isect_info) const {
		isect_info._bsdf.reset((new BSDF(isect_info)));
		isect_info._bsdf->add(
			std::unique_ptr<BxDF>(new FresnelSpecularReflection(
				1.0f, std::unique_ptr<Fresnel>(new FresnelNoop()))));
	}
}