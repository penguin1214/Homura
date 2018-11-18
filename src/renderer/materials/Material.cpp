#include "renderer/materials/Material.h"
#include "renderer/bxdfs/Lambert.h"
#include "renderer/bxdfs/Fresnel.h"
#include "renderer/Scene.h"

namespace Homura {
	Material::Material(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bxdfs) {
		std::vector<std::string> bxdf_names = json["bsdf"].getVector<std::string>();
		_bsdf = std::make_shared<BSDF>();
		for (unsigned i = 0; i < bxdf_names.size(); i++) {
			_bsdf->add(bxdfs[bxdf_names[i]]);
		}
	}

	void Material::computeScatteringFunction(IntersectInfo &isect_info) const {
		_bsdf->prepareForRender(isect_info);
		isect_info._bsdf = _bsdf;
	}
}