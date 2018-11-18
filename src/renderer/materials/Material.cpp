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
		_bsdf->update_isect(isect_info);
		isect_info._bsdf = _bsdf;
		/// TODO: prepare bxdfs for this intersection
	}
	/*
	MatteMaterial::MatteMaterial(const JsonObject &json) : _R(json["R"].getVec3()) {
		_bsdf = std::shared_ptr<BSDF>();
	}

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

	void GlassMaterial::computeScatteringFunction(IntersectInfo &isect_info) const {
	}*/
}