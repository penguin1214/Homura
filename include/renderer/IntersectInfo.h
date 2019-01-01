#ifndef HOMURA_INTERSECTINFO_H_
#define HOMURA_INTERSECTINFO_H_

#include "core/math/Geometry.h"
#include "renderer/Ray.h"
#include "renderer/medium/Medium.h"

namespace Homura {
	class Primitive;
	class BSDF;
	class PhaseFunction;
	class ProjectiveSensor;
	class Emitter;
	enum TransportMode :short;

	struct IntersectInfo {
		Point3f _p;
		Vec3f _wo;
		float _t = INFINITY;
		Vec3f _normal;
		TransportMode _transport_mode;

		/// TODO: not handled at present, not suitable for two adjacent specular object
		//float _etaI = 1.f;	// everytime rays inter a medium(transmission), update _etaI.
		std::shared_ptr<Primitive> _primitive;
		std::shared_ptr<BSDF> _bsdf;
		MediumInterface _medium_interface;
		struct strct_shading {
			Vec3f _n;
			Vec3f _tangent;
			Vec3f _bitangent;
		} _shading;
		Vec3f _dpdu, _dpdv;

		IntersectInfo() {}
		IntersectInfo(const IntersectInfo &origin);
		IntersectInfo(const Point3f p);
		IntersectInfo(const Point3f &p, const Vec3f &wo) : _p(p), _wo(wo) {}
		IntersectInfo(const Point3f &p, const Vec3f &wo, const float &t, const Vec3f &normal, const TransportMode &mode, std::shared_ptr<Primitive> prim, std::shared_ptr<BSDF> bsdf,
			const strct_shading &shad, const Vec3f &dpdu, const Vec3f &dpdv) :
			_p(p), _wo(wo), _t(t), _normal(normal), _transport_mode(mode), _primitive(prim), _bsdf(bsdf), _shading(shad), _dpdu(dpdu), _dpdv(dpdv) {}

		void computeScatteringFunction();
		Vec3f Le(const Vec3f &wi) const;
		Ray spawnRay(const Vec3f &wi) const;
		Ray spawnRayTo(const IntersectInfo &other) const;

		bool isSurfaceIntersect() const { return _normal != Vec3f(); }

		std::shared_ptr<Medium> getMedium(const Vec3f &wi) const {
			return (wi.dot(_normal) > 0.f) ? (_medium_interface._outside) : (_medium_interface._inside);
		}

		std::shared_ptr<Medium> getMedium() const {
			return _medium_interface._inside;
		}
	};

	/// TODO
	struct MediumIntersectInfo : public IntersectInfo {
		MediumIntersectInfo() : _phase(nullptr) {}
		MediumIntersectInfo(const Point3f &p, const Vec3f &wo, std::shared_ptr<PhaseFunction> phase) : IntersectInfo(p, wo), _phase(phase) {}

		bool isValid() const { return _phase != nullptr; }

		std::shared_ptr<PhaseFunction> _phase;
	};

	struct EndpointInfo : public IntersectInfo {
		EndpointInfo() {}
		explicit EndpointInfo(const EndpointInfo &other)
			: IntersectInfo(other._p, other._wo, other._t, other._normal, other._transport_mode, other._primitive, other._bsdf, other._shading, other._dpdu, other._dpdv),
			_emitter(other._emitter) {}	/// TODO: union copy

		EndpointInfo(const std::shared_ptr<ProjectiveSensor> s, const Ray &ray) : IntersectInfo(ray._o), _sensor(s) {}
		EndpointInfo(const std::shared_ptr<Emitter> e, const Ray &ray, const Vec3f &n_light) : IntersectInfo(ray._o), _emitter(e) {
			_normal = n_light;
		}
		EndpointInfo(const IntersectInfo &isect, std::shared_ptr<Emitter> e) : IntersectInfo(isect), _emitter(e) {}
		EndpointInfo(const IntersectInfo &isect, std::shared_ptr<ProjectiveSensor> s) : IntersectInfo(isect), _sensor(s) {}
		EndpointInfo(const Ray &ray) : IntersectInfo(ray._o), _emitter(nullptr) {}	/* for special handling of infinite light (escaped ray) */

		~EndpointInfo() {}

		/* Stores information of path endpoint, i.e., on sensor or light. */
		union {
			const std::shared_ptr<ProjectiveSensor> _sensor;
			const std::shared_ptr<Emitter> _emitter;
		};
	};

}

#endif // !HOMURA_INTERSECTINFO_H_
