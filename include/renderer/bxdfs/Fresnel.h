#ifndef HOMURA_FRESNEL_H_
#define HOMURA_FRESNEL_H_

#include "BxDF.h"

namespace Homura {
    class Fresnel {
        /* Fresnel abstract class interfaces for computing Fresnel reflectance. */
    public:
        virtual Vec3f evaluate(float cosI) const = 0;   // compute reflectance
    };

    class FresnelDielectric : public Fresnel {
    public:
        FresnelDielectric(const float &etaI, const float &etaT) : _etaI(etaI), _etaT(etaT) {}
        Vec3f evaluate(float cosI) const override {
            cosI = clamp(cosI, -1.0f, 1.0f);
            float etaI = _etaI, etaT = _etaT;
            if (cosI < 0.0f) {
                std::swap(etaI, etaT);
                cosI = std::abs(cosI);
            }
            float sinI = std::sqrt(std::max(0.f, 1.f - cosI*cosI));
            float sinT = etaI / (etaT * sinI);
            if (sinT >= 1.f)
                return Vec3f(1.f);
            float cosT = std::sqrt(std::max(0.f, 1.0f - sinT*sinT));
            float rParl = (etaT*cosI - etaI*cosT) / (etaT*cosI + etaI*cosT);
            float rPerp = (etaI*cosI - etaT*cosT) / (etaI*cosI + etaT*cosT);
            return Vec3f(0.5 * (rParl*rParl + rPerp * rPerp));
        }

    private:
        float _etaI, _etaT;
    };

    class FresnelConductor : public Fresnel {
    public:
        FresnelConductor(const float &etaI, const float &etaT, const float &k) : _etaI(etaI), _etaT(etaT), _k(k) {}
        Vec3f evaluate(float cosI) const override {
            cosI = clamp(cosI, -1.0f, 1.0f);
            float cosI2 = cosI * cosI;
            float sinI2 = 1.f - cosI2;
            float etaI = _etaI, etaT = _etaT;
            float eta = etaT / etaI;
            float eta2 = eta*eta;
            float etak = _k / etaI;
            float etak2 = etak * etak;
            float t0 = eta2 - etak2 - sinI2;
            float a2pb2 = std::sqrt(t0*t0 + 4*eta2*etak2);
            float t1 = a2pb2 + cosI2;
            float a = std::sqrt(0.5f * (a2pb2 + t0));
            float t2 = 2.0f * cosI * a;
            float Rs = (t1 - t2) / (t1+t2);
            float t3 = cosI2 * a2pb2 + sinI2*sinI2;
            float t4 = t2 * sinI2;
            float Rp = Rs * (t3-t4) / (t3+t4);
            return Vec3f(0.5f * (Rs+Rp));
        }

    private:
        float _etaI, _etaT, _k;
        /// TODO: Vec3f _k/*absorption*/;
    };

	class FresnelNoop : public Fresnel {
	public:
		FresnelNoop() = default;
		Vec3f evaluate(float cosI) const override { return Vec3f(1.f); }
	};

    class FresnelSpecularReflection : public BxDF {
    public:
        FresnelSpecularReflection(const Vec3f &R, std::unique_ptr<Fresnel> fresnel)
        : BxDF(BxDFType(BSDF_REFLECTION|BSDF_SPECULAR)), _R(R), _fresnel(std::move(fresnel)) {}

        Vec3f f(const Vec3f &wo, const Vec3f &wi) const override { return Vec3f(0.f); };
		Vec3f sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample/*TODO*/, float &pdf, BxDFType *sampled_type = nullptr) const override;

    private:
        const Vec3f _R;  // incoming radiance
        const std::unique_ptr<Fresnel> _fresnel;
    };

    class FresnelSpecularTransmission: public BxDF {
    public:
        FresnelSpecularTransmission(Vec3f T, float etaI, float etaT)
        : BxDF(BxDFType(BSDF_TRANSMISSION|BSDF_SPECULAR)), _T(T), _etaI(etaI), _etaT(etaT), _fresnel(new FresnelDielectric(etaI, etaT)) {}

        Vec3f f(const Vec3f &wo, const Vec3f &wi) const override { return Vec3f(0.f); };
		Vec3f sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample/*TODO*/, float &pdf, BxDFType *sampled_type = nullptr) const override;
		//float Pdf(const Vec3f &wo, const Vec3f &wi) const override;

    private:
        Vec3f _T;
        const float _etaI, _etaT;
        const std::unique_ptr<FresnelDielectric> _fresnel;
    };

    class FresnelSpecular : public BxDF {}; /// TODO
}

#endif //HOMURA_FRESNEL_H_
