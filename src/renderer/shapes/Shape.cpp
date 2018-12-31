#include "renderer/shapes/Shape.h"
#include "core/sampler/Sampling.h"

namespace Homura {
	inline bool solveQuadratic(float a, float b, float c, float &t0, float &t1) {
		double discrim = (double)b*(double)b - 4.*(double)a*(double)c;
		if (discrim < 0.) return false;
		double sqrt_discrim = std::sqrt(discrim);
		float q;
		if ((float)b < 0.f)
			q = -.5 * (b - sqrt_discrim);
		else
			q = -.5 * (b + sqrt_discrim);

		t0 = q / a;
		t1 = c / q;
		
		if ((float)t0 > (float)t1) std::swap(t0, t1);
		return true;
	}

	bool Sphere::intersect(const Ray &r, float &hitt, IntersectInfo *isect_info) const {
		Ray ray = r * _world2local;	/// TODO
		float ox = ray._o.x(); float oy = ray._o.y(); float oz = ray._o.z();
		float dx = ray._d.x(); float dy = ray._d.y(); float dz = ray._d.z();
		float a = dx * dx + dy * dy + dz * dz;
		float b = 2 * (dx*ox + dy * oy + dz * oz);
		float c = ox * ox + oy * oy + oz * oz - _radius * _radius;

		float t0, t1;
		if (!(solveQuadratic(a, b, c, t0, t1)))
			return false;

		if (t0 < 0.f || t1 > ray._tmax)
			return false;
		//ray._tmax = t0;

		hitt = t0;

		Point3f phit = r._o + t0 * r._d;
		float phi = std::atan2(phit.y(), phit.x());
		if (phi < 0.f) phi += TWOPI;

		float u = phi / _phi_max;
		float theta = std::acos(clamp(phit.z() / _radius, -1.f, 1.f));
		float v = (theta - _theta_min) / (_theta_max - _theta_min);

		float zRadius = std::sqrt(phit.x() * phit.x() + phit.y() + phit.y());
		float invzradius = 1.f / zRadius;
		float cosPhi = phit.x() * invzradius;
		float sinPhi = phit.y() * invzradius;
		Vec3f dpdu(-_phi_max * phit.y(), _phi_max*phit.x(), 0);
		Vec3f dpdv = (_theta_max - _theta_min) * 
			Vec3f(phit.z() * cosPhi, phit.z() * sinPhi, -_radius * std::sin(theta));

		if (isect_info) {
			isect_info->_t = t0;
			isect_info->_wo = -r._d;
			isect_info->_p = phit;
			isect_info->_normal = (phit - _p).normalized();
			isect_info->_shading._n = isect_info->_normal;
			isect_info->_dpdu = dpdu;
			isect_info->_dpdv = dpdv;
			isect_info->_shading._tangent = dpdu.normalized();
			isect_info->_shading._bitangent = dpdv.normalized();
		}

		return true;
	}

	IntersectInfo Sphere::sample(const Point2f &u) const {
		Point3f p = Point3f(0, 0, 0) + _radius * uniformSampleSphereArea(u);
		IntersectInfo isect_info;
		isect_info._normal = Vec3f(p)*(_local2world.forNormal());
		isect_info._p = p * _local2world;
		return isect_info;
	}

	bool Quad::intersect(const Ray &r, float &hitt, IntersectInfo *isect_info) const {
		float NdotD = r._d.dot(_normal);
		if (std::abs(NdotD) < 1e-6f)
			return false;

		float t = (_normal.dot(_base - r._o)) / NdotD;

		if (t < 0.f || t > r._tmax)
			return false;

		Point3f p = r._o + t * r._d;

		Vec3f v = p - _base;
		float l0 = _edge0.normalized().dot(v);
		float l1 = _edge1.normalized().dot(v);

		if (l0 < 0.0f || l0 > _edge0.length() || l1 < 0.0f || l1 > _edge1.length() )
			return false;

		//r._tmax = t;

		hitt = t;

		if (isect_info) {
			isect_info->_t = t;
			isect_info->_wo = -r._d;
			isect_info->_p = p;
			isect_info->_normal = _normal;
			isect_info->_shading._n = _normal;
			isect_info->_shading._tangent = _edge0;
			isect_info->_shading._bitangent = _edge1;
			/// TODO: dpdu, dpdv??
		}

		return true;
	}

	IntersectInfo Quad::sample(const Point2f &u) const {
		Point3f p = _base + _edge0 * u[0] + _edge1 * u[1];
		IntersectInfo isect_info;
		isect_info._normal = _normal;
		isect_info._p = p;
		return isect_info;
	}
}