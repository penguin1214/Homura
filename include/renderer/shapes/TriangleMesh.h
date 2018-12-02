#ifndef HOMURA_TRIANGLE_MESH_H_
#define HOMURA_TRIANGLE_MESH_H_

#include "renderer/shapes/Shape.h"
#include "core/math/Geometry.h"
#include "core/io/ObjHandler.h"
#include "core/io/JsonObject.h"
#include <vector>

namespace Homura {
	struct Triangle {
		//std::shared_ptr<TriangleMesh> _mesh;
		Vec3i _vertIdx;
		Vec3i _normalIdx;

		//Triangle(std::shared_ptr<TriangleMesh> m, Vec3i vertidx);
		Triangle(Vec3i vertidx, Vec3i normidx);
		//inline bool intersect(const Ray &r, Point3f p0, Point3f p1, Point3f p2, IntersectInfo &isect_info) const;
		inline bool intersect(const Ray &r, Point3f p0, Point3f p1, Point3f p2, const Vec3f &n0, const Vec3f &n1, const Vec3f n2, IntersectInfo &isect_info, bool smooth) const;
		//inline bool intersectP(const Ray &r, Point3f p0, Point3f p1, Point3f p2) const;
		inline bool intersectP(const Ray &r, Point3f p0, Point3f p1, Point3f p2) const;

	private:
		void getUV(Point2f uv[3]) const;
	};

	class TriangleMesh :public Shape {
    public:
        std::vector<Point3f> _vertices;
        std::vector<Vec3i> _indecies;
		std::vector<Vec3f> _normals;
		std::vector<Vec3i> _normal_indecies;

		//TriangleMesh(std::vector<Point3f> verts, std::vector<Vec3i> idxs);
		TriangleMesh(const JsonObject &json);

		Bound3f localBound() const override {
			return Bound3f(Point3f(0.f), Point3f(1.f));	/// TODO
		}

		bool intersect(const Ray &r, float *hitt, IntersectInfo *isect_info) const override;

		float area() const override { return 1.f; }	/// TODO
		inline Vec3f normal(const IntersectInfo &ref) const override { return Vec3f(0,1,0); }	/// TODO

		IntersectInfo sample(const Point2f &u) const override { return IntersectInfo(); }	/// TODO
	};
}
#endif	//!HOMURA_TRANGLE_MESH_H_
