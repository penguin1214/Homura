#ifndef HOMURA_FRESNEL_H_
#define HOMURA_FRESNEL_H_

#include "BxDF.h"

namespace Homura {
	Vec3f FrDielectric(float cosI, const float &_etaI, const float &_etaT);
	Vec3f FrConductor(float cosI, const float &_etaI, const float &_etaT, const float &_k);

    class Fresnel {
        /* Fresnel abstract class interfaces for computing Fresnel reflectance. */
    public:
        virtual Vec3f evaluate(float cosI) const = 0;   // compute reflectance
    };

    class FresnelDielectric : public Fresnel {
    public:
        FresnelDielectric(const float &etaI, const float &etaT) : _etaI(etaI), _etaT(etaT) {}
        Vec3f evaluate(float cosI) const override {
			return FrDielectric(cosI, _etaI, _etaT);
        }

    private:
        float _etaI, _etaT;
    };

    class FresnelConductor : public Fresnel {
    public:
        FresnelConductor(const float &etaI, const float &etaT, const float &k) : _etaI(etaI), _etaT(etaT), _k(k) {}
        Vec3f evaluate(float cosI) const override {
			return FrConductor(cosI, _etaI, _etaT, _k);
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
        FresnelSpecularReflection(const Vec3f &R, const float &eta, std::string name)
        : BxDF(BxDFType(BSDF_REFLECTION|BSDF_SPECULAR), name), _R(R), _eta(eta) {
			if (_R.max() > (1.f - 1e-6))
				_fresnel = std::unique_ptr<FresnelNoop>(new FresnelNoop());
			else {
				_fresnel = std::unique_ptr<FresnelDielectric>(new FresnelDielectric(_eta, 1.0f));
			}
		}

		//void prepareForRender(const IntersectInfo &isect_info) override;

        Vec3f f(const Vec3f &wo, const Vec3f &wi) const override { return Vec3f(0.f); };
		Vec3f sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample, float &pdf, BxDFType *sampled_type = nullptr, TransportMode *mode = nullptr) const override;

    private:
		const float _eta;
        const Vec3f _R;
        std::unique_ptr<Fresnel> _fresnel;
    };

    class FresnelSpecularTransmission: public BxDF {
    public:
        FresnelSpecularTransmission(const Vec3f &T, const float &eta, std::string name)
        //: BxDF(BxDFType(BSDF_TRANSMISSION|BSDF_SPECULAR)), _T(T), _eta(eta) {}
        : BxDF(BxDFType(BSDF_TRANSMISSION|BSDF_SPECULAR), name), _T(T), _etaT(eta), _etaI(1.f),
		_fresnel(new FresnelDielectric(_etaI, _etaT)) {}

		//void prepareForRender(const IntersectInfo &isect_info) override {
		//	_fresnel = std::unique_ptr<FresnelDielectric>(new FresnelDielectric(_eta, 1.0f));
		//}

        Vec3f f(const Vec3f &wo, const Vec3f &wi) const override { return Vec3f(0.f); };
		Vec3f sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample, float &pdf, BxDFType *sampled_type = nullptr, TransportMode *mode = nullptr) const override;
		//float Pdf(const Vec3f &wo, const Vec3f &wi) const override;

    private:
        Vec3f _T;
        const float _etaI, _etaT;
        std::unique_ptr<FresnelDielectric> _fresnel;
    };

    class FresnelSpecular : public BxDF {
	public:
		FresnelSpecular(const Vec3f &R, const Vec3f &T, const float &eta, std::string name)
		: BxDF(BxDFType(BSDF_REFLECTION|BSDF_TRANSMISSION|BSDF_SPECULAR), name),
		_R(R), _T(T), _eta(eta), _fresnel(new FresnelDielectric(1.f, _eta)) {}

		//void prepareForRender(const IntersectInfo &isect_info) override;

		Vec3f f(const Vec3f &wo, const Vec3f &wi) const override { return Vec3f(0.f); };
		Vec3f sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample, float &pdf, BxDFType *sampled_type = nullptr, TransportMode *mode = nullptr) const override;

	private:
		const float _eta;
		Vec3f _R;
		Vec3f _T;
		std::unique_ptr<Fresnel> _fresnel;
	};
}

#endif //HOMURA_FRESNEL_H_
