#ifndef HOMURA_INTERSECTINFO_H_
#define HOMURA_INTERSECTINFO_H_

#include "core/math/Geometry.h"
#include "renderer/Ray.h"

namespace Homura {
	class Primitive;
	class BSDF;

	struct IntersectInfo {
		Point3f _p;
		Vec3f _wo;
		float _t = INFINITY;
		Vec3f _normal;
		std::shared_ptr<Primitive> _primitive;
		std::shared_ptr<BSDF> _bsdf;
		struct {
			Vec3f _n;
			Vec3f _ss;	// tangent
			Vec3f _ts;	// bitangent
		} _shading;

		IntersectInfo() = default;
		IntersectInfo(const IntersectInfo &origin);
		IntersectInfo(const Point3f p);

		void computeScatteringFunction();
		Ray spawnRay(const Vec3f &wi) const;
		Ray spawnRayTo(const IntersectInfo &other) const;
	};

}

#endif // !HOMURA_INTERSECTINFO_H_
