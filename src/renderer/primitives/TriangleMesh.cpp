#include "renderer/shapes/TriangleMesh.h"
#include "core/math/CoordinateSystem.h"
#include <iostream>

namespace Homura {
	//Triangle::Triangle(std::shared_ptr<TriangleMesh> m, Vec3i vertidx) {
	//	_mesh = m;
	//	_vertIdx = vertidx;
 //       // TODO
	//}
	Triangle::Triangle(Vec3i vertIdx, Vec3i normidx) {
		_vertIdx = vertIdx;
		_normalIdx = normidx;
	}

	void Triangle::getUV(Point2f uv[3]) const {
		/// TODO: texture uv
		uv[0] = Point2f(0, 0);
		uv[1] = Point2f(1, 0);
		uv[2] = Point2f(2, 0);
	}

	bool Triangle::intersect(const Ray &r, Point3f p0, Point3f p1, Point3f p2, const Vec3f &n0, const Vec3f &n1, const Vec3f n2, float &hitt, IntersectInfo &isect_info, bool smooth) const {
		// translate
		Point3f p0_ = p0 - r._o;
		Point3f p1_ = p1 - r._o;
		Point3f p2_ = p2 - r._o;

		// permute, to make sure z-dimension is not zero
		// permutation is just like to circular move right/left until z is largest dimemsion.
		int kz = r._d.maxDim();
		int kx = kz + 1; if (kx == 3) kx = 0;
		int ky = kx + 1; if (ky == 3) ky = 0;

		// swap coord
		Vec3f d_ = r._d.permute(kx, ky, kz);
		p0_ = p0_.permute(kx, ky, kz);
		p1_ = p1_.permute(kx, ky, kz);
		p2_ = p2_.permute(kx, ky, kz);
		// shear
		float Sx = -d_.x() / d_.z();
		float Sy = -d_.y() / d_.z();
		float Sz = 1.0f / d_.z();

		// perform transform (p.z doesn't matter now)
		p0_[0] += Sx * p0_[2];
		p0_[1] += Sy * p0_[2];
		p1_[0] += Sx * p1_[2];
		p1_[1] += Sy * p1_[2];
		p2_[0] += Sx * p2_[2];
		p2_[1] += Sy * p2_[2];

		// signed edge function to judge whether origin's projection in projected triangle
		/// ORDER MATTERS!!! ei should be edge function of (0,0) and other two points other than pi!
		/// This influences the computation of tScale and bi.
		float e0 = p1_.x()*p2_.y() - p2_.x()*p1_.y();   // p1 -> p2
		float e1 = p2_.x()*p0_.y() - p0_.x()*p2_.y();   // p2 -> p0
		float e2 = p0_.x()*p1_.y() - p1_.x()*p0_.y();   // p0 -> p1

		// perform high-precision at edges
		if (e0 == 0.0f || e1 == 0.0f || e2 == 0.0f) {
			double p0xp1y = (double)p0_.x() * (double)p1_.y();
			double p1xp0y = (double)p1_.x() * (double)p0_.y();
			e2 = (float)(p0xp1y - p1xp0y);
			double p1xp2y = (double)p1_.x() * (double)p2_.y();
			double p2xp1y = (double)p2_.x() * (double)p1_.y();
			e0 = (float)(p1xp2y - p2xp1y);
			double p2xp0y = (double)p2_.x() * (double)p0_.y();
			double p0xp2y = (double)p0_.x() * p2_.y();
			e1 = (float)(p2xp0y - p0xp2y);
		}

		// edge detection
		if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e2 > 0 || e1 > 0))
			return false;

		float det = e0 + e1 + e2;
		if (det == 0)	// on edge
			return false;

		// compute z value (i.e. t)
		p0_[2] *= Sz;
		p1_[2] *= Sz;
		p2_[2] *= Sz;

		float tScaled = e0 * p0_.z() + e1 * p1_.z() + e2 * p2_.z();
		if (det < 0 && (tScaled > 0 || tScaled < r._tmax*det)) return false;
		else if (det > 0 && (tScaled < 0 || tScaled > r._tmax*det)) return false;

		float invDet = 1.0f / det;
		float t = tScaled * invDet;

		//if (t > r._tmax) return false;
		//r._tmax = t;
		if (t > hitt) return false;
		hitt = t;

		/// TODO: compute barycentric coord for texture mapping
		float b0 = e0 * invDet;
		float b1 = e1 * invDet;
		float b2 = e2 * invDet;

		// compute partial derivative
		Vec3f dpdu, dpdv;
		Point2f uv[3];
		getUV(uv);
		Point2f uvHit = uv[0] * b0 + uv[1] * b1 + uv[2] * b2;
		Vec2f duv02 = uv[0] - uv[2];
		Vec2f duv12 = uv[1] - uv[2];
		Vec3f dp02 = p0 - p2;
		Vec3f dp12 = p1 - p2;
		float determinant = duv02[0] * duv12[1] - duv02[1] * duv12[0];
		if (determinant == 0) {
			ShadingCoordinateSystem(((p1 - p0).cross(p2 - p1)).normalized(), dpdu, dpdv);
		}
		else {
			float inv = 1.f / determinant;
			dpdu = (dp02*duv12[1] - dp12 * duv02[1])*inv;
			dpdv = (-dp02 * duv12[0] + dp12 * duv02[0])*inv;
		}
		// TODO: ensure computed t conservatively greater than zero
		// TODO: error bounds

		// TODO: intersect info class
		isect_info._t = t;
		//		isect_info._d = r._d;
		isect_info._wo = -r._d; /// TODO: do flip?
		isect_info._p = p0 * b0 + p1 * b1 + p2 * b2;	// use barycentric coordinate, more accurate.
		//isect_info._primitive = this->_mesh;

		isect_info._dpdu = dpdu;
		isect_info._dpdv = dpdv;
		isect_info._normal = isect_info._dpdu.cross(isect_info._dpdv).normalized();

		// smooth shading
		if (smooth)
			isect_info._shading._n = (b0 * n0 + b1 * n1 + b2 * n2).normalized();		/// TODO: geometry normal and shading normal
		else
			isect_info._shading._n = isect_info._normal;
		isect_info._shading._tangent = isect_info._dpdu.normalized();
		isect_info._shading._bitangent = isect_info._dpdv.normalized();

		// TODO: check orientation
		// TODO: backface
		return true;
	}

	bool Triangle::intersectP(const Ray &r, Point3f p0, Point3f p1, Point3f p2, float &hitt) const {
		// translate
		Point3f p0_ = p0 - r._o;
		Point3f p1_ = p1 - r._o;
		Point3f p2_ = p2 - r._o;

		// permute, to make sure z-dimension is not zero
		// permutation is just like to circular move right/left until z is largest dimemsion.
		int kz = r._d.maxDim();
		int kx = kz + 1; if (kx == 3) kx = 0;
		int ky = kx + 1; if (ky == 3) ky = 0;

		// swap coord
		Vec3f d_ = r._d.permute(kx, ky, kz);
		p0_ = p0_.permute(kx, ky, kz);
		p1_ = p1_.permute(kx, ky, kz);
		p2_ = p2_.permute(kx, ky, kz);
		// shear
		float Sx = -d_.x() / d_.z();
		float Sy = -d_.y() / d_.z();
		float Sz = 1.0f / d_.z();

		// perform transform (p.z doesn't matter now)
		p0_[0] += Sx * p0_[2];
		p0_[1] += Sy * p0_[2];
		p1_[0] += Sx * p1_[2];
		p1_[1] += Sy * p1_[2];
		p2_[0] += Sx * p2_[2];
		p2_[1] += Sy * p2_[2];

		// signed edge function to judge whether origin's projection in projected triangle
		/// ORDER MATTERS!!! ei should be edge function of (0,0) and other two points other than pi!
		/// This influences the computation of tScale and bi.
		float e0 = p1_.x()*p2_.y() - p2_.x()*p1_.y();   // p1 -> p2
		float e1 = p2_.x()*p0_.y() - p0_.x()*p2_.y();   // p2 -> p0
		float e2 = p0_.x()*p1_.y() - p1_.x()*p0_.y();   // p0 -> p1

		// perform high-precision at edges
		if (e0 == 0.0f || e1 == 0.0f || e2 == 0.0f) {
			double p0xp1y = (double)p0_.x() * (double)p1_.y();
			double p1xp0y = (double)p1_.x() * (double)p0_.y();
			e2 = (float)(p0xp1y - p1xp0y);
			double p1xp2y = (double)p1_.x() * (double)p2_.y();
			double p2xp1y = (double)p2_.x() * (double)p1_.y();
			e0 = (float)(p1xp2y - p2xp1y);
			double p2xp0y = (double)p2_.x() * (double)p0_.y();
			double p0xp2y = (double)p0_.x() * p2_.y();
			e1 = (float)(p2xp0y - p0xp2y);
		}

		// edge detection
		if ((e0 < 0 || e1 < 0 || e2 < 0) && (e0 > 0 || e2 > 0 || e1 > 0))
			return false;

		float det = e0 + e1 + e2;
		if (det == 0)	// on edge
			return false;

		// compute z value (i.e. t)
		p0_[2] *= Sz;
		p1_[2] *= Sz;
		p2_[2] *= Sz;

		float tScaled = e0 * p0_.z() + e1 * p1_.z() + e2 * p2_.z();
		if (det < 0 && (tScaled >= 0 || tScaled < r._tmax*det)) return false;
		else if (det > 0 && (tScaled <= 0 || tScaled > r._tmax*det)) return false;

		float invDet = 1.0f / det;
		float t = tScaled * invDet;

		//if (t > r._tmax) return false;
		//r._tmax = t;
		if (t > hitt) return false;
		hitt = t;

		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//TriangleMesh::TriangleMesh(std::vector<Point3f> verts, std::vector<Vec3i> idxs) : Shape(), _vertices(verts), _indecies(idxs) {}

	TriangleMesh::TriangleMesh(const JsonObject &json) :
		Shape(json["transform"].getTransform()) {
		std::string obj_path = json["path"].getString();
		ObjHandler::readObj(obj_path.c_str(), _vertices, _indecies, _normals, _normal_indecies);
		computeLocalBound();
	}

	bool TriangleMesh::intersect(const Ray &r, float &hitt, IntersectInfo *isect_info) const {
		bool flag = false;
		hitt = r._tmax;
		for (int i = 0; i < _indecies.size(); i++) {
			if (isect_info) {
				if (_normal_indecies.size() > 0) {
					Triangle t(_indecies[i], _normal_indecies[i]);
					if (t.intersect(r, _vertices[t._vertIdx[0]], _vertices[t._vertIdx[1]], _vertices[t._vertIdx[2]], _normals[t._normalIdx[0]], _normals[t._normalIdx[1]], _normals[t._normalIdx[2]], hitt, *isect_info, true)) {
						flag = true;
					}
				}
				else {
					Triangle t(_indecies[i], -1);
					const Vec3f tmp(-1);
					if (t.intersect(r, _vertices[t._vertIdx[0]], _vertices[t._vertIdx[1]], _vertices[t._vertIdx[2]], tmp, tmp, tmp, hitt, *isect_info, false)) {
						flag = true;
					}
				}
			}
			else {
				Triangle t(_indecies[i], (_normal_indecies.size() > 0) ? _normal_indecies[i] : -1);
				if (t.intersectP(r, _vertices[t._vertIdx[0]], _vertices[t._vertIdx[1]], _vertices[t._vertIdx[2]], hitt))
					flag = true;
			}
		}

		return flag;
	}

	//bool TriangleMesh::intersectP(const Ray &r) const {
	//	bool flag = false;
	//	for (int i = 0; i < _indecies.size(); i++) {
	//		Triangle t(_indecies[i], (_normal_indecies.size() > 0) ? _normal_indecies[i] : -1);
	//		if (t.intersectP(r, _vertices[t._vertIdx[0]], _vertices[t._vertIdx[1]], _vertices[t._vertIdx[2]]))
	//			flag = true;
	//	}
	//	return flag;
	//}

}