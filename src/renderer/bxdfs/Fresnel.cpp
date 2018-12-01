#include "renderer/bxdfs/Fresnel.h"

namespace Homura {
	Vec3f FrDielectric(float cosI, const float &_etaI, const float &_etaT) {
		cosI = clamp(cosI, -1.0f, 1.0f);
		float etaI = _etaI, etaT = _etaT;
		if (cosI <= 0.0f) {
			std::swap(etaI, etaT);
			cosI = std::abs(cosI);
		}
		float sinI = std::sqrt(std::max(0.f, 1.f - cosI * cosI));
		float sinT = etaI / etaT * sinI;
		if (sinT >= 1.f)
			return Vec3f(1.f);
		float cosT = std::sqrt(std::max(0.f, 1.0f - sinT * sinT));
		float rParl = (etaT*cosI - etaI * cosT) / (etaT*cosI + etaI * cosT);
		float rPerp = (etaI*cosI - etaT * cosT) / (etaI*cosI + etaT * cosT);
		return Vec3f(0.5 * (rParl*rParl + rPerp * rPerp));
	}

	Vec3f FrConductor(float cosI, const float &_etaI, const float &_etaT, const float &_k) {
		cosI = clamp(cosI, -1.0f, 1.0f);
		float cosI2 = cosI * cosI;
		float sinI2 = 1.f - cosI2;
		float etaI = _etaI, etaT = _etaT;
		float eta = etaT / etaI;
		float eta2 = eta * eta;
		float etak = _k / etaI;
		float etak2 = etak * etak;
		float t0 = eta2 - etak2 - sinI2;
		float a2pb2 = std::sqrt(t0*t0 + 4 * eta2*etak2);
		float t1 = a2pb2 + cosI2;
		float a = std::sqrt(0.5f * (a2pb2 + t0));
		float t2 = 2.0f * cosI * a;
		float Rs = (t1 - t2) / (t1 + t2);
		float t3 = cosI2 * a2pb2 + sinI2 * sinI2;
		float t4 = t2 * sinI2;
		float Rp = Rs * (t3 - t4) / (t3 + t4);
		return Vec3f(0.5f * (Rs + Rp));
	}

    Vec3f FresnelSpecularReflection::sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample, float &pdf,
                                              Homura::BxDFType *sampled_type) const {
        wi = Vec3f(-wo.x(), -wo.y(), wo.z());
        pdf = 1.0f;
        return _fresnel->evaluate(CosTheta(wi)) * _R / AbsCosTheta(wi);
    }

    Vec3f FresnelSpecularTransmission::sample_f(const Vec3f &wi, Vec3f &wt, const Point2f &sample, float &pdf, BxDFType *sampled_type) const {
        bool entering = CosTheta(wi) > 0.f;
		//std::cout << "entering: " << entering << std::endl;
        float etaI = entering ? _etaI : _etaT;
        float etaT = entering ? _etaT : _etaI;
        Vec3f normal = Vec3f(0,0,1).dot(wi)>0.f ? Vec3f(0,0,1) : -Vec3f(0,0,1);
        if (!Refract(wi, normal, etaI/etaT, wt)) {
            return Vec3f(0.f);
        }

        pdf = 1.f;
        Vec3f Ft = _T * (Vec3f(1.f) - _fresnel->evaluate(CosTheta(wt)));
        /// TODO: from camera or light source
		/// Since haven't do bidirectional methods, just account for the energe transmitted
		Ft *= (etaI*etaI) / (etaT*etaT);
        return Ft / AbsCosTheta(wi);
    }

	Vec3f FresnelSpecular::sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample, float &pdf, BxDFType *sampled_type) const {
		float f = FrDielectric(CosTheta(wo), 1.f, _eta).max();
		//std::cout << "F: " << f << std::endl;
		if (sample[0] < f) {
			// reflection
			//std::cout << "reflection" << std::endl;
			*sampled_type = BxDFType(BSDF_SPECULAR | BSDF_REFLECTION);
			wi = Vec3f(-wo.x(), -wo.y(), wo.z());
			pdf = f;
			return _R * f / AbsCosTheta(wi);
		}
		else {
			// transmit
			// compute ray direction
			bool entering = CosTheta(wo) > 0.f;
			float etaI = entering ? 1.f : _eta;
			float etaT = entering ? _eta : 1.f;
			Vec3f normal = Vec3f(0, 0, 1).dot(wo) > 0.f ? Vec3f(0, 0, 1) : -Vec3f(0, 0, 1);
			//std::cout << "transmit wi: " << wo << std::endl;
			if (!Refract(wo, normal, etaI / etaT, wi)) {
				*sampled_type = BxDFType(BSDF_SPECULAR | BSDF_REFLECTION);
				wi = Vec3f(-wo.x(), -wo.y(), wo.z());
				pdf = 1.f;
				return Vec3f(1.f);
				//return Vec3f(0.f);
			}
			//std::cout << "transmission" << std::endl;
			//std::cout << "transmit wo: " << wi << std::endl;
			/// TODO: from camera or light source
			/// Since haven't do bidirectional methods, just account for the energe transmitted
			Vec3f ft = _T * (1.f - f);
			*sampled_type = BxDFType(BSDF_SPECULAR | BSDF_TRANSMISSION);
			pdf = 1.f - f;
			ft *= (etaI*etaI) / (etaT*etaT);
			//std::cout << "transmit BTDF" << (ft) << std::endl;
			return ft / AbsCosTheta(wi);
		}
	}
}
