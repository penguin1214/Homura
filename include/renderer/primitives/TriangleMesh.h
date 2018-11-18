#ifndef HOMURA_TRIANGLE_MESH_H_
#define HOMURA_TRIANGLE_MESH_H_

#include "Primitive.h"
#include "core/math/Geometry.h"
#include "core/io/ObjHandler.h"
#include "core/io/JsonObject.h"
#include <vector>

namespace Homura {
	class TriangleMesh;
	struct Triangle {
		//std::shared_ptr<TriangleMesh> _mesh;
		Vec3i _vertIdx;

		//Triangle(std::shared_ptr<TriangleMesh> m, Vec3i vertidx);
		Triangle(Vec3i vertidx);
		inline bool intersect(const Ray &r, Point3f p0, Point3f p1, Point3f p2, IntersectInfo &isect_info) const;
		inline bool intersectP(const Ray &r, Point3f p0, Point3f p1, Point3f p2) const;

	private:
		void getUV(Point2f uv[3]) const;
	};

	class TriangleMesh :public Primitive {
    public:
        std::vector<Point3f> _vertices;
        std::vector<Vec3i> _indecies;
		std::vector<Vec3f> _normals;

        TriangleMesh(std::vector<Point3f> verts, std::vector<Vec3i> idxs);
		TriangleMesh(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs);

		bool intersect(const Ray &r, IntersectInfo &info) override;
		bool intersectP(const Ray &r) const override;
	};
}
#endif	//!HOMURA_TRANGLE_MESH_H_
