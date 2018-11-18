#ifndef HOMURA_MATERIAL_H_
#define HOMURA_MATERIAL_H_

#include "renderer/IntersectInfo.h"
#include "core/io/JsonObject.h"
#include "renderer/bxdfs/BxDF.h"

#include <unordered_map>

namespace Homura {
	class Scene;
    class Material {
    public:
		Material(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bxdfs);
		void computeScatteringFunction(IntersectInfo &isect_info) const;

		std::shared_ptr<BSDF> _bsdf;
    };

 //   class MatteMaterial : public Material {
 //   public:
	//	MatteMaterial(const JsonObject &json);
 //       void computeScatteringFunction(IntersectInfo &isect_info) const override;
	//private:
	//	Vec3f _R;
 //   };

	//class Mirror : public Material {
	//public:
	//	Mirror() = default;
	//	void computeScatteringFunction(IntersectInfo &isect_info) const override;
	//};

	//class GlassMaterial : public Material {
	//public:
	//	GlassMaterial(const JsonObject &json) : _R(json["R"].getVec3()), _T(json["T"].getVec3()), _eta(json["eta"].getFloat()) {}
	//	void computeScatteringFunction(IntersectInfo &isect_info) const override;

	//private:
	//	const Vec3f _R;
	//	const Vec3f _T;
	//	const float _eta;
	//};
}

#endif //HOMURA_MATERIAL_H_
