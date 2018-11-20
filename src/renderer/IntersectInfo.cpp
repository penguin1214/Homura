#include "renderer/IntersectInfo.h"
#include "renderer/primitives/Primitive.h"
#include "renderer/bxdfs/BxDF.h"

namespace Homura {
	IntersectInfo::IntersectInfo(const IntersectInfo &origin) :
	_p(origin._p), _wo(origin._wo), _t(origin._t), _normal(origin._normal),
	_shading(origin._shading) {
		if (origin._bsdf)
			_bsdf = std::make_shared<BSDF>(*origin._bsdf);
		if (origin._primitive)
			_primitive = origin._primitive->getShared();
	}

	IntersectInfo::IntersectInfo(const Point3f p) {
		IntersectInfo();
		_p = p;
	}

	void IntersectInfo::computeScatteringFunction() {
		/// TODO: compute differential ray for anti-aliasing
		_primitive->computeScatteringFunction(*this);
	}

	Ray IntersectInfo::spawnRay(const Vec3f &wi) const {
		return Ray(_p + ((_normal.dot(wi)>0.f) ? (_normal*1e-5):(-_normal*1e-5) ), wi);
	}

	Ray IntersectInfo::spawnRayTo(const IntersectInfo &other) const {
		Vec3f d = (other._p - _p).normalized();
		return Ray(_p + ((_normal.dot(d) > 0.f) ? (_normal*1e-5) : (-_normal * 1e-5)), d, 0.f, (other._p-_p).length());
	}
}