#include "renderer/bxdfs/BxDF.h"

namespace Homura {
	BxDF::BxDF(BxDFType type) : _type(type) {}

	//BSDF::BSDF(IntersectInfo &isect_info, const float eta) :
	//	_eta(eta), _ng(isect_info._normal), _ns(isect_info._shading._n), _ts(isect_info._shading._tangent), _bs(isect_info._shading._bitangent) {}

	BSDF::BSDF(const BSDF &origin) :
	_ng(origin._ng), _ns(origin._ns), _ts(origin._ts), _bs(origin._bs),
	_bxdfs(origin._bxdfs/*TODO*/) {}
}