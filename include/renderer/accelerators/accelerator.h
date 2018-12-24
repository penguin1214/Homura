#ifndef HOMURA_ACCELERATOR_H_
#define HOMURA_ACCELERATOR_H_

#include <vector>
#include "renderer/primitives/Primitive.h"
#include "renderer/shapes/Bound.h"

namespace Homura {
	enum BVHSplitType {
		EQL_CNT, SAH, HLBVH
	};

	struct BVHNode {
		BVHNode() = default;
		BVHNode(const int &nprim, const int&offset, const Bound3f &bound)
			: _n_prims(nprim), _offset(offset), _bound(bound), _axis(-1), _left(nullptr), _right(nullptr) {}
		BVHNode(const int &axis, const Bound3f &bound, std::shared_ptr<BVHNode>left, std::shared_ptr<BVHNode>right)
			: _axis(axis), _bound(bound), _left(left), _right(right), _n_prims(-1), _offset(-1) {}

		bool intersectP(const Ray &r, const std::vector<std::shared_ptr<Primitive>> &ordered_prims);
		bool intersectP(const Ray &r, std::shared_ptr<Emitter> evalemitter, const std::vector<std::shared_ptr<Primitive>> &ordered_prims);
		bool intersect(Ray &r, const std::vector<std::shared_ptr<Primitive>> &ordered_prims, IntersectInfo &isect) const;

		const int _n_prims;
		const int _offset;
		const int _axis;
		const Bound3f _bound;
		std::shared_ptr<BVHNode> _left, _right;
	};

	struct BVHBoundInfo {
		BVHBoundInfo(int &i, Bound3f &bnd) : _idx(i), _bounding(bnd), _centroid((_bounding._min+_bounding._max)*.5f) {}

		int _idx;
		Bound3f _bounding;
		Point3f _centroid;
	};

	class BVHAccelerator :PrimitiveGroup {
	public:
		BVHAccelerator(std::vector<std::shared_ptr<Primitive>> &prims, const int &max_nodes = 1, const BVHSplitType &type = BVHSplitType::EQL_CNT);

		Bound3f worldBound() const override;
		bool intersect(const Ray &r, IntersectInfo &info);
		bool intersectP(const Ray &r) const;

	private:
		std::shared_ptr<BVHNode> buildBVHRecurse(std::vector<BVHBoundInfo> bounding_info, const int &start, const int &end, std::vector<std::shared_ptr<Primitive>> &ordered_prims, int &total_nodes);
		std::shared_ptr<BVHNode> createLeaf(const int &n_prim, const int &offset, const Bound3f &bound);
		std::shared_ptr<BVHNode> createInterior(const int &spliting_axis, std::shared_ptr<BVHNode> left, std::shared_ptr<BVHNode> right);
		/// TODO: flat bvh

		const int _max_obj_per_node;
		const BVHSplitType _split_type;
		int _n_total_nodes;
		std::shared_ptr<BVHNode> _root;
		//std::vector<std::shared_ptr<Primitive>> _primitives;
	};
}

#endif // !HOMURA_ACCELERATOR_H_
