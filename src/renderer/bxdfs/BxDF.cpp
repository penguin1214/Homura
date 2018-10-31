#include "renderer/bxdfs/BxDF.h"

namespace Homura {
	BxDF::BxDF(BxDFType type) : _type(type) {}

	BSDF::BSDF(IntersectInfo &isect_info, const float eta) :
		_eta(eta), _ng(isect_info._normal), _ns(isect_info._shading._n), _ss(isect_info._shading._ss), _ts(isect_info._shading._ts) {}

	BSDF::BSDF(const BSDF &origin) :
	_eta(origin._eta), _ng(origin._ng), _ns(origin._ns), _ts(origin._ts), _ss(origin._ss),
	_bxdfs(origin._bxdfs/*TODO*/) {}
}