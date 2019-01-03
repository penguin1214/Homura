#include "core/Common.h"
#include "renderer/bxdfs/Microfacet.h"
#include "renderer/bxdfs/Fresnel.h"
#include "core/math/CoordinateSystem.h"

namespace Homura {
	float BeckmannDistribution::D(const Vec3f &w) const {
		float cos2 = w.z() * w.z();
		float tan2 = (1.f - cos2) / cos2;
		if (std::isinf(tan2))
			return 0.f;

		float cos4 = cos2 * cos2;
		float sin_theta = std::sqrt(std::max(0.f, 1.f - cos2));
		float cos_phi = (sin_theta == 0.f) ? (1.f) : (clamp(w.x() / sin_theta, -1.f, 1.f));
		float cos2_phi = cos_phi * cos_phi;
		float sin2_phi = 1.f - cos2_phi;
		float a2 = _alpha * _alpha;
		return std::exp(-tan2 * (cos2_phi / a2 + sin2_phi / a2)) /
			(PI * a2 * cos4);
	}

	float BeckmannDistribution::lambda(const Vec3f &w) const {
		float cos2 = w.z() * w.z();
		float abstan = std::abs(std::sqrt((1.f - cos2) / cos2));
		if (std::isinf(abstan))
			return 0.f;

		float a2 = _alpha * _alpha;
		float sin_theta = std::sqrt(std::max(0.f, 1.f - cos2));
		float cos_phi = (sin_theta == 0.f) ? (1.f) : (clamp(w.x() / sin_theta, -1.f, 1.f));
		float cos2_phi = cos_phi * cos_phi;
		float sin2_phi = 1.f - cos2_phi;
		float alpha = std::sqrt(cos2_phi*a2 + sin2_phi * a2);
		float a = 1.f / (alpha * abstan);
		if (a >= 1.6f)
			return 0.f;

		return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
	}

	Vec3f BeckmannDistribution::sample_wh(const Vec3f &wo, const Point2f &u) const {
		// sample full distribution for now
		float log = std::log(1.f - u[0]);
		float tan2theta = -_alpha * _alpha * log;
		float phi = u[1] * 2.f * PI;
		float cos_theta = 1.f / std::sqrt(1.f + tan2theta);
		float sin_theta = std::sqrt(std::max(0.f, 1.f - cos_theta * cos_theta));

		Vec3f wh = sphericalDirection(sin_theta, cos_theta, phi, Vec3f(1,0,0), Vec3f(0,0,1), Vec3f(0,1,0));	/// TODO: check
		return (wo.z()*wh.z() > 0.f) ? (wh) : (-wh);
	}

	MicrofacetReflection::MicrofacetReflection(const JsonObject &json)
		: BxDF(BxDFType(BSDF_REFLECTION|BSDF_GLOSSY),
			json["name"].getString()), _R(json["R"].getVec3()) {
		auto dist = json["distribution"];
		std::string dist_type = dist["type"].getString();
		if (dist_type == "Beckmann")
			_distribution = std::unique_ptr<BeckmannDistribution>(new BeckmannDistribution(dist));

		_fresnel = std::unique_ptr<FresnelNoop>(new FresnelNoop());
	}

	Vec3f MicrofacetReflection::f(const Vec3f &wo, const Vec3f &wi) const {
		float cos_i = std::abs(wi.z());
		float cos_o = std::abs(wo.z());
		if (cos_i < 1e-6 || cos_o < 1e-6)
			return Vec3f(0.f);

		Vec3f wh = (wi + wo).normalized();
		if (wh.x() == 0.f && wh.y() == 0.f && wh.z() == 0.f)
			return Vec3f(0.f);

		Vec3f reflect = _fresnel->evaluate(wh.dot(wi));

		return _R * reflect * (_distribution->D(wh)) * _distribution->G(wo, wi) /
			(4.f * cos_i * cos_o);
	}

	Vec3f MicrofacetReflection::sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample, float &pdf, BxDFType *sampled_type, const TransportMode *mode) const {
		if (wo.z() == 0.f)
			return Vec3f(0.f);

		Vec3f wh = _distribution->sample_wh(wo, sample);
		wi = Vec3f(-wo.x(), -wo.y(), wo.z());
		if (!(wo.z() * wo.y() > 0))
			return Vec3f(0.f);
		
		pdf = _distribution->Pdf(wo, wh) / (4.f * wo.dot(wh));	/// TODO: check
		return f(wo, wi);
	}
}