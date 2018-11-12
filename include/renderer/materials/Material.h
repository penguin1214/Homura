#ifndef HOMURA_MATERIAL_H_
#define HOMURA_MATERIAL_H_

#include "renderer/IntersectInfo.h"

namespace Homura {
    class Material {
    public:
		Material(const JsonObject &json) : _R(json["R"].getVec3()) {}
        virtual void computeScatteringFunction(IntersectInfo &isect_info) const;

	protected:
		Vec3f _R;
    };

    class MatteMaterial : public Material {
    public:
		MatteMaterial(const JsonObject &json) : Material(json) {}
        void computeScatteringFunction(IntersectInfo &isect_info) const override;
    };

	class Mirror : public Material {
	public:
		Mirror(const JsonObject &json) : Material(json) {}
		void computeScatteringFunction(IntersectInfo &isect_info) const override;
	};
}

#endif //HOMURA_MATERIAL_H_
