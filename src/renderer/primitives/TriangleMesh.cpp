#include "renderer/primitives/TriangleMesh.h"
#include <iostream>
#include "renderer/primitives/Primitive.h"

namespace Homura {
	//Triangle::Triangle(std::shared_ptr<TriangleMesh> m, Vec3i vertidx) {
	//	_mesh = m;
	//	_vertIdx = vertidx;
 //       // TODO
	//}
	Triangle::Triangle(Vec3i vertIdx) {
		_vertIdx = vertIdx;
	}

    bool Triangle::intersect(const Ray &r, Point3f p0, Point3f p1, Point3f p2, IntersectInfo &isect_info) const {
        // translate
        Point3f p0_ = p0 - r._o;
        Point3f p1_ = p1 - r._o;
        Point3f p2_ = p2 - r._o;

        // permute, to make sure z-dimension is not zero
		// permutation is just like to circular move right/left until z is largest dimemsion.
        int kz = r._d.maxDim();
        int kx = kz+1; if (kx == 3) kx = 0;
        int ky = kx+1; if (ky == 3) ky = 0;

        // swap coord
        Vec3f d_ = r._d.permute(kx, ky, kz);
        p0_ = p0_.permute(kx, ky, kz);
        p1_ = p1_.permute(kx, ky, kz);
        p2_ = p2_.permute(kx, ky, kz);
        // shear
        float Sx = -d_.x()/d_.z();
        float Sy = -d_.y()/d_.z();
        float Sz = 1.0f/d_.z();

        // perform transform (p.z doesn't matter now)
        p0_[0] += Sx*p0_[2];
        p0_[1] += Sy*p0_[2];
        p1_[0] += Sx*p1_[2];
        p1_[1] += Sy*p1_[2];
        p2_[0] += Sx*p2_[2];
        p2_[1] += Sy*p2_[2];

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
        if ( (e0 < 0 || e1 < 0 ||  e2 < 0) && (e0 > 0 || e2 > 0 || e1 > 0) )
            return false;

        float det = e0 + e1 + e2;
        if (det == 0)	// on edge
			return false;

        // compute z value (i.e. t)
        p0_[2] *= Sz;
        p1_[2] *= Sz;
        p2_[2] *= Sz;

        float tScaled = e0*p0_.z() + e1*p1_.z() + e2*p2_.z();
        if (det < 0 && (tScaled > 0 || tScaled < r._tmax*det) ) return false;
        else if (det > 0 && (tScaled < 0 || tScaled > r._tmax*det) ) return false;

        float invDet = 1.0f / det;
        float t = tScaled * invDet;
		/// TODO: compute barycentric coord for texture mapping
        //float b0 = e0 * invDet;
        //float b1 = e1 * invDet;
        //float b2 = e2 * invDet;

        // TODO: ensure computed t conservatively greater than zero
        // TODO: error bounds
        // TODO: partial derivative
        // TODO: UV value
		r._tmax = t;
        // TODO: intersect info class
		isect_info._t = t;
//		isect_info._d = r._d;
        isect_info._wo = -r._d; /// TODO: do flip?
		isect_info._p = r._o + r._d*t;
		//isect_info._primitive = this->_mesh;

        // TODO: shading geometry here may be not correct! tangent vector should be defined by dpdu???
		isect_info._shading._n = ((p1 - p0).cross(p2 - p1)).normalized();	isect_info._normal = isect_info._shading._n;	/// TODO: geometry normal and shading normal
        isect_info._shading._ss = (p1-p0).normalized();
        isect_info._shading._ts = (isect_info._shading._n.cross(isect_info._shading._ss)).normalized();
        // TODO: check orientation
        // TODO: backface
        return true;
    }

	bool Triangle::intersectP(const Ray &r, Point3f p0, Point3f p1, Point3f p2) const {
        // translate
        Point3f p0_ = p0 - r._o;
        Point3f p1_ = p1 - r._o;
        Point3f p2_ = p2 - r._o;

        // permute, to make sure z-dimension is not zero
		// permutation is just like to circular move right/left until z is largest dimemsion.
        int kz = r._d.maxDim();
        int kx = kz+1; if (kx == 3) kx = 0;
        int ky = kx+1; if (ky == 3) ky = 0;

        // swap coord
        Vec3f d_ = r._d.permute(kx, ky, kz);
        p0_ = p0_.permute(kx, ky, kz);
        p1_ = p1_.permute(kx, ky, kz);
        p2_ = p2_.permute(kx, ky, kz);
        // shear
        float Sx = -d_.x()/d_.z();
        float Sy = -d_.y()/d_.z();
        float Sz = 1.0f/d_.z();

        // perform transform (p.z doesn't matter now)
        p0_[0] += Sx*p0_[2];
        p0_[1] += Sy*p0_[2];
        p1_[0] += Sx*p1_[2];
        p1_[1] += Sy*p1_[2];
        p2_[0] += Sx*p2_[2];
        p2_[1] += Sy*p2_[2];

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
        if ( (e0 < 0 || e1 < 0 ||  e2 < 0) && (e0 > 0 || e2 > 0 || e1 > 0) )
            return false;

        float det = e0 + e1 + e2;
        if (det == 0)	// on edge
			return false;

        // compute z value (i.e. t)
        p0_[2] *= Sz;
        p1_[2] *= Sz;
        p2_[2] *= Sz;

        float tScaled = e0*p0_.z() + e1*p1_.z() + e2*p2_.z();
        if (det < 0 && (tScaled > 0 || tScaled < r._tmax*det) ) return false;
        else if (det > 0 && (tScaled < 0 || tScaled > r._tmax*det) ) return false;

		float invDet = 1.0f / det;
		float t = tScaled * invDet;
		r._tmax = t;

        return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TriangleMesh::TriangleMesh(std::vector<Point3f> verts, std::vector<Vec3i> idxs) : Primitive(), _vertices(verts), _indecies(idxs) {}

    TriangleMesh::TriangleMesh(const JsonObject &json) :
	Primitive(json) {
        std::string obj_path = json["path"].getString();
        ObjHandler::readObj(obj_path.c_str(), _vertices, _indecies, _normals);
    }

	bool TriangleMesh::intersect(const Ray &r, IntersectInfo &info) {
		bool flag = false;
        for (int i = 0; i < _indecies.size(); i++) {
            Triangle t(_indecies[i]);
			if (t.intersect(r, _vertices[t._vertIdx[0]], _vertices[t._vertIdx[1]], _vertices[t._vertIdx[2]], info)) {
				info._primitive = getShared();
				flag = true;
			}
        }

		return flag;
	}

	bool TriangleMesh::intersectP(const Ray &r) const {
		bool flag = false;
		for (int i = 0; i < _indecies.size(); i++) {
			Triangle t(_indecies[i]);
			if (t.intersectP(r, _vertices[t._vertIdx[0]], _vertices[t._vertIdx[1]], _vertices[t._vertIdx[2]]))
				flag = true;
		}
		return flag;
	}

}