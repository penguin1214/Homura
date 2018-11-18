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
}

#endif //HOMURA_MATERIAL_H_
