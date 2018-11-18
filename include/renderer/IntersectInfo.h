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

		/// TODO: not handled at present, not suitable for two adjacent specular object
		//float _etaI = 1.f;	// everytime rays inter a medium(transmission), update _etaI.
		std::shared_ptr<Primitive> _primitive;
		std::shared_ptr<BSDF> _bsdf;
		struct {
			Vec3f _n;
			Vec3f _tangent;
			Vec3f _bitangent;
		} _shading;
		Vec3f _dpdu, _dpdv;

		IntersectInfo() = default;
		IntersectInfo(const IntersectInfo &origin);
		IntersectInfo(const Point3f p);

		void computeScatteringFunction();
		Ray spawnRay(const Vec3f &wi) const;
		Ray spawnRayTo(const IntersectInfo &other) const;
	};

}

#endif // !HOMURA_INTERSECTINFO_H_
