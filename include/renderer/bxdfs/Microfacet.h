#ifndef HOMURA_MICROFACET_H_
#define HOMURA_MICROFACET_H_

#include "core/math/Geometry.h"
#include "renderer/bxdfs/BxDF.h"
#include "core/io/JsonObject.h"

namespace Homura {
	class Fresnel;

	class OrenNayar : public BxDF {};	/// TODO

	class MicrofacetDistribution {
	public:
		virtual float D(const Vec3f &w) const = 0;	/* return differential area of normal direction w. */
		virtual float lambda(const Vec3f &w) const = 0;		/* G1 auxiliary function */
		virtual float G1(const Vec3f &w) const { return 1.f / (1.f + lambda(w)); }
		virtual float G(const Vec3f &wo, const Vec3f &wi) const { return 1.f / (1.f + G1(wo) + G1(wi)); }
		
		virtual Vec3f sample_wh(const Vec3f &wo, const Point2f &u) const = 0;
		virtual float Pdf(const Vec3f &wo, const Vec3f &wh) const {
			return D(wh) * std::abs(wh.z());
		}

	protected:
		MicrofacetDistribution(const float &a) : _alpha(a) {}

		float _alpha;	// roughness
	};

	class BeckmannDistribution : public MicrofacetDistribution {
	public:
		BeckmannDistribution(const JsonObject &json) : MicrofacetDistribution(json["alpha"].getFloat()) {}

		float D(const Vec3f &w) const override;
		float lambda(const Vec3f &w) const override;

		Vec3f sample_wh(const Vec3f &wo, const Point2f &u) const override;
	};

	class MicrofacetReflection : public BxDF {
	public:
		MicrofacetReflection(const JsonObject &json);

		Vec3f f(const Vec3f &wo, const Vec3f &wi) const override;
		Vec3f sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample, float &pdf, BxDFType *sampled_type = nullptr, const TransportMode *mode = nullptr) const override;

	private:
		const Vec3f _R;
		std::unique_ptr<MicrofacetDistribution> _distribution;
		std::unique_ptr<Fresnel> _fresnel;
	};
}

#endif // !HOMURA_MICROFACET_H_
