#ifndef HOMURA_SHAPE_H_
#define	HOMURA_SHAPE_H_

#include "core/Common.h"
#include "core/math/Geometry.h"
#include "core/math/Mat4f.h"
#include "core/io/JsonObject.h"
#include "renderer/Bound.h"
#include "renderer/Ray.h"
#include "renderer/IntersectInfo.h"

namespace Homura {
	inline bool solveQuadratic(float a, float b, float c, float &t0, float &t1);


	class Shape {
	public:
		Shape(const Mat4f &local2world) : _local2world(local2world), _world2local(Mat4f::inverse(_local2world)), _p(Point3f(0)*_local2world) {}

		virtual Bound3f localBound() const = 0;
		virtual Bound3f worldBound() const {
			return localBound() * _local2world;	/// TODO
		}

		virtual bool intersectP(const Ray &r, float *hitt, IntersectInfo *isect_info) const = 0;
		virtual bool intersect(const Ray &r, float *hitt, IntersectInfo *isect_info) const { return intersectP(r, nullptr, nullptr); }

		virtual float area() const = 0;

		virtual IntersectInfo sample(const Point2f &u) const = 0;
		virtual float pdf() const { return 1.f / area(); }
		//virtual float pdf_Li(const IntersectInfo &isect_info, const Vec3f &wi) const = 0;

		Point3f _p;
		const Mat4f _local2world, _world2local;
	};


	class Sphere : public Shape {
	public:
		Sphere(const JsonObject &json)
		: Shape(json["transform"].getTransform()),
		_radius(json["radius"].getFloat()) {}

		Bound3f localBound() const override {
			return Bound3f(Point3f(-_radius, -_radius, -_radius), Point3f(_radius, _radius, _radius));
		}

		bool intersectP(const Ray &r, float *hitt, IntersectInfo *isect_info) const override;

		float area() const override { return 4 * PI*_radius*_radius; }

		IntersectInfo sample(const Point2f &u) const override;

	private:
		const float _radius;
		/// TODO: partial sphere
		//const float _z_min, _z_max;
		//const float _theta_min, _theta_max, _phi_max;
	};
}

#endif // !HOMURA_SHAPE_H_