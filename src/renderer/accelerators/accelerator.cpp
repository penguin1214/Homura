#include "renderer/accelerators/accelerator.h"

namespace Homura {
	bool BVHNode::intersect(Ray &r, const std::vector<std::shared_ptr<Primitive>> &ordered_prims, IntersectInfo &isect) const {
		bool flag = false;
		for (int i = _offset; i < _offset + _n_prims; i++) {
			if (ordered_prims[i]->intersect(r, isect))
				flag = true;
		}
		return flag;
	}
	
	bool BVHNode::intersectP(const Ray &r, std::shared_ptr<Emitter> evalemitter, const std::vector<std::shared_ptr<Primitive>> &ordered_prims) {
		if (_bound.intersectP(r, nullptr, nullptr)) {
			if (_left != nullptr && _right != nullptr) {
				// interior
				return (_left->intersectP(r, ordered_prims) || _right->intersectP(r, ordered_prims));
			}
			else {
				// leaf, test all primitives
				bool flag = false;
				for (int i = _offset; i < _offset + _n_prims; i++) {
					std::shared_ptr<Primitive> hitprim = ordered_prims[i]->intersectP(r);
					if (hitprim != nullptr && (hitprim->getEmitter() != evalemitter))
						flag = true;
				}
				return flag;
			}
		}
		else
			return false;
	}

	bool BVHNode::intersectP(const Ray &r, const std::vector<std::shared_ptr<Primitive>> &ordered_prims) {
		if (_bound.intersectP(r, nullptr, nullptr)) {
			if (_left != nullptr && _right != nullptr) {
				// interior
				return (_left->intersectP(r, ordered_prims) || _right->intersectP(r, ordered_prims));
			}
			else {
				// leaf, test all primitives
				bool flag = false;
				for (int i = _offset; i < _offset + _n_prims; i++) {
					if (ordered_prims[i]->intersectP(r))
						flag = true;
				}
				return flag;
			}
		}
		else
			return false;
	}

	bool BVHAccelerator::intersectP(const Ray &r) const {
		if (_root == nullptr)
			return false;

		if (_root->_bound.intersectP(r, nullptr, nullptr))
			return (_root->_left->intersectP(r, _primitives) || _root->_right->intersectP(r, _primitives));
	}

	bool BVHAccelerator::intersect(const Ray &r, IntersectInfo &info) {
		if (_root == nullptr)
			return false;

		if (_root->_bound.intersectP(r, nullptr, nullptr)) {
			return (_root->_left->intersectP(r, _primitives) || (_root->_right->intersectP(r, _primitives)));
				//(_root->_left == nullptr) ? (false) : (_root->_left->intersectP()) ||
				//(_root->_right == nullptr) ? (false) : (_root->_right->intersectP()));
		}
		else
			return false;
	}

	Bound3f BVHAccelerator::worldBound() const {
		return (_n_total_nodes) ? (_root->_bound) : (Bound3f());
	}

	BVHAccelerator::BVHAccelerator(std::vector<std::shared_ptr<Primitive>> &prims, const int &max_nodes, const BVHSplitType &type)
	: _max_obj_per_node(max_nodes), _split_type(type) {
		if (prims.empty())
			return;

		_primitives = std::move(prims);

		std::vector<BVHBoundInfo> bounding_info;
		bounding_info.reserve(_primitives.size());
		for (int i = 0; i < _primitives.size(); i++) {
			bounding_info[i] = { i, _primitives[i]->worldBound() };
		}

		std::vector<std::shared_ptr<Primitive>> ordered_prims;
		ordered_prims.reserve(_primitives.size());
		if (_split_type == BVHSplitType::EQL_CNT) {
			_root = buildBVHRecurse(bounding_info, 0, _primitives.size(), ordered_prims, _n_total_nodes);
		}
		else {
			std::cerr << "ERROR::NOT_IMPLEMENT_ERROR" << std::endl;
		}

		_primitives.swap(ordered_prims);
	}

	std::shared_ptr<BVHNode> BVHAccelerator::createLeaf(const int &n_prim, const int &offset, const Bound3f &bound) {
		return std::make_shared<BVHNode>(n_prim, offset, bound);
	}

	std::shared_ptr<BVHNode> BVHAccelerator::createInterior(const int &spliting_axis, std::shared_ptr<BVHNode> left, std::shared_ptr<BVHNode> right) {
		return std::make_shared<BVHNode>(spliting_axis, Bound3f::Union(left->_bound, right->_bound), left, right);
	}

	std::shared_ptr<BVHNode> BVHAccelerator::buildBVHRecurse(std::vector<BVHBoundInfo> bounding_info, const int &start, const int &end, std::vector<std::shared_ptr<Primitive>> &ordered_prims, int &total_nodes) {
		Bound3f bound;
		for (int i = start; i < end; i++)
			bound = Bound3f::Union(bound, bounding_info[i]._bounding);

		total_nodes++;

		int n_prim = end - start;
		if (n_prim == 1) {
			// terminal condition
			int offset = ordered_prims.size();
			for (int i = start; i < end; i++) {
				int prim_id = bounding_info[i]._idx;
				ordered_prims.push_back(_primitives[prim_id]);
			}
			return createLeaf(n_prim, offset, bound);
		}
		else {
			// compute centroid in order to select spliting axis
			Bound3f centroid_bound;
			for (int i = start; i < end; i++) {
				centroid_bound = Bound3f::Union(centroid_bound, bounding_info[i]._centroid);
			}
			int spliting_axis = centroid_bound.maxDim();

			int mid = (start + end) / 2;
			switch (_split_type) {
			case BVHSplitType::EQL_CNT: {
				std::nth_element(&bounding_info[start], &bounding_info[mid], &bounding_info[end - 1] + 1,
					[spliting_axis](const BVHBoundInfo &a, const BVHBoundInfo &b) {
					return a._centroid[spliting_axis] < b._centroid[spliting_axis];
				});
				break;
			}
			default:
				break;
			}

			return createInterior(spliting_axis,
				buildBVHRecurse(bounding_info, start, mid, ordered_prims, total_nodes),
				buildBVHRecurse(bounding_info, mid, end, ordered_prims, total_nodes));
		}
	}
}