#ifndef HOMURA_LAMBERT_H_
#define HOMURA_LAMBERT_H_

#include "BxDF.h"
#include "core/sampler/Sampling.h"
#include <iostream>

namespace Homura {
    class LambertReflection : public BxDF {
    public:
        LambertReflection(const Vec3f &R, std::string name)
        : BxDF(BxDFType(BSDF_REFLECTION || BSDF_DIFFUSE), name), _R(R) {}

        Vec3f f(const Vec3f &wo, const Vec3f &wi) const override {
			/// TODO: should Lambertian reflectance also be cosine weighted?
			return _R * INVPI;
		}

		Vec3f sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample/*TODO*/, float &pdf, BxDFType *sampled_type = nullptr) const override {
			//wi = uniformSampleHemisphereSolidAngle(sample);
			wi = cosineSampleHemisphereSolidAngle(sample);
			if (wo.z() < 0.f) wi.z() = -wi.z();
			pdf = Pdf(wo, wi);
			return f(wo, wi);
		}

		float Pdf(const Vec3f &wo, const Vec3f &wi) const override {
			return (wo.z() * wi.z() > 0.f) ? (AbsCosTheta(wi))*INVPI : 0.f;
		}

        const Vec3f _R;	// reflectance
    };
}

#endif //HOMURA_LAMBERT_H_
