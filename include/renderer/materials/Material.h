#ifndef HOMURA_MATERIAL_H_
#define HOMURA_MATERIAL_H_

#include "renderer/IntersectInfo.h"
#include "core/io/JsonObject.h"

namespace Homura {
    class Material {
    public:
		virtual void computeScatteringFunction(IntersectInfo &isect_info) const = 0;
    };

    class MatteMaterial : public Material {
    public:
		MatteMaterial(const JsonObject &json) : _R(json["R"].getVec3()) {}
        void computeScatteringFunction(IntersectInfo &isect_info) const override;
	private:
		Vec3f _R;
    };

	class Mirror : public Material {
	public:
		Mirror() = default;
		void computeScatteringFunction(IntersectInfo &isect_info) const override;
	};
}

#endif //HOMURA_MATERIAL_H_
