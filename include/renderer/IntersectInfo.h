#ifndef HOMURA_INTERSECTINFO_H_
#define HOMURA_INTERSECTINFO_H_

#include "core/math/Geometry.h"
#include "renderer/Ray.h"

namespace Homura {
	class Primitive;
	class BSDF;
	class ProjectiveSensor;
	class Emitter;

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
		Vec3f Le(const Vec3f &wi) const;
		Ray spawnRay(const Vec3f &wi) const;
		Ray spawnRayTo(const IntersectInfo &other) const;
	};

	struct EndpointInfo : public IntersectInfo {
		EndpointInfo() {}
		EndpointInfo(const EndpointInfo &other) {}	/// TODO: copy constructor with union member

		EndpointInfo(const std::shared_ptr<ProjectiveSensor> s, const Ray &ray) : IntersectInfo(ray._o), _sensor(s) {}
		EndpointInfo(const std::shared_ptr<Emitter> e, const Ray &ray, const Vec3f &n_light) : IntersectInfo(ray._o), _emitter(e) {
			_normal = n_light;
		}
		~EndpointInfo() {}

		/* Stores information of path endpoint, i.e., on sensor or light. */
		union {
			const std::shared_ptr<ProjectiveSensor> _sensor;
			const std::shared_ptr<Emitter> _emitter;
		};
	};

}

#endif // !HOMURA_INTERSECTINFO_H_
