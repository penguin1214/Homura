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
		Shape(const Mat4f &local2world) : _local2world(local2world), _world2local(Mat4f::inverse(_local2world))/*, _p(Point3f(0.f)*_local2world)*/ {
			_p = Point3f(0.f)*_local2world;
		}

		virtual Bound3f localBound() const = 0;
		virtual Bound3f worldBound() const {
			return localBound() * _local2world;	/// TODO
		}

		virtual bool intersectP(const Ray &r, float *hitt, IntersectInfo *isect_info) const { return intersect(r, nullptr, nullptr); };
		virtual bool intersect(const Ray &r, float *hitt, IntersectInfo *isect_info) const = 0;

		virtual float area() const = 0;
		virtual inline Vec3f normal(const IntersectInfo &ref) const = 0;

		virtual IntersectInfo sample(const Point2f &u) const = 0;
		virtual float pdf() const { return 1.f / area(); }
		//virtual float pdf_Li(const IntersectInfo &isect_info, const Vec3f &wi) const = 0;

		Vec3f _p;
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

		bool intersect(const Ray &r, float *hitt, IntersectInfo *isect_info) const override;

		float area() const override { return 4 * PI*_radius*_radius; }
		inline Vec3f normal(const IntersectInfo &ref) const override { return Vec3f(ref._p).normalized(); }

		IntersectInfo sample(const Point2f &u) const override;

	private:
		const float _radius;
		/// TODO: partial sphere
		//const float _z_min, _z_max;
		const float _theta_min = 0, _theta_max = PI, _phi_max = TWOPI;
	};

	class Quad : public Shape {
	public:
		Quad(const JsonObject &json)
		: Shape(json["transform"].getTransform()),
		_base(json["transform"]["translate"].getVec3()), _edge0(json["edge0"].getVec3()), _edge1(json["edge1"].getVec3()), _normal(_edge0.cross(_edge1).normalized()) {
			if (std::abs(_edge0.dot(_edge1)) > 1e-6f)
				std::cout << "QUAD::EDGE_NOT_ORTHO_ERROR" << std::endl;
		}

		Bound3f localBound() const override {
			return Bound3f(_base, _base+_edge0+_edge1);
		}

		bool intersect(const Ray &r, float *hitt, IntersectInfo *isect_info) const override;
		IntersectInfo sample(const Point2f &u) const override;

		float area() const override { return _edge0.length() * _edge1.length(); }
		inline Vec3f normal(const IntersectInfo &ref) const override { return _normal; }

		const Point3f _base;
		const Vec3f _edge0, _edge1;
		const Vec3f _normal;
	};
}

#endif // !HOMURA_SHAPE_H_