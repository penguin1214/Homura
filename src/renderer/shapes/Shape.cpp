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

	bool Sphere::intersectP(const Ray &r, float *hitt, IntersectInfo *isect_info) const {
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
		/// TODO: intersection info
		return true;
	}

	IntersectInfo Sphere::sample(const Point2f &u) const {
		Point3f p = Point3f(0, 0, 0) + _radius * uniformSampleSphereArea(u);
		IntersectInfo isect_info;
		isect_info._normal = Vec3f(p)*(_local2world.forNormal());
		isect_info._p = p * _local2world;
		return isect_info;
	}

	bool Quad::intersectP(const Ray &r, float *hitt, IntersectInfo *isect_info) const {
		float NdotD = r._d.dot(_normal);
		if (std::abs(NdotD) < 1e-6f)
			return false;

		float t = (_normal.dot(_base - r._o)) / NdotD;

		if (t < 0.f || t > r._tmax)
			return false;

		Point3f p = r._o + t * r._d;
		/// TODO: intersect info

		r._tmax = t;
		return true;
	}

	IntersectInfo Quad::sample(const Point2f &u) const {
		Point3f p = _base + _edge0 * u[0] + _edge1 * u[1];
		IntersectInfo isect_info;
		isect_info._normal = _normal * (_local2world.forNormal());
		isect_info._p = p * _local2world;
		return isect_info;
	}
}