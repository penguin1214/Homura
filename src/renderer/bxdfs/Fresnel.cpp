#include "renderer/bxdfs/Fresnel.h"

namespace Homura {
    Vec3f FresnelSpecularReflection::sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample, float &pdf,
                                              Homura::BxDFType *sampled_type) const {
        wi = Vec3f(-wo.x(), -wo.y(), wo.z());
        pdf = 1.0f;
        return _fresnel->evaluate(CosTheta(wi)) * _R / AbsCosTheta(wi);
    }

    Vec3f FresnelSpecularTransmission::sample_f(const Vec3f &wi, Vec3f &wt, const Point2f &sample/*TODO*/, float &pdf, BxDFType *sampled_type) const {
        bool entering = CosTheta(wi) > 0.f;
        float etaI = entering ? _etaI : _etaT;
        float etaT = entering ? _etaT : _etaI;
        Vec3f normal = Vec3f(0,0,1).dot(wi)>0.f ? Vec3f(0,0,1) : -Vec3f(0,0,1);
        if (Refract(wi, normal, etaI/etaT, wt)) {
            return Vec3f(0.f);
        }
        pdf = 1.f;
        Vec3f Ft = _T * (Vec3f(1.f) - _fresnel->evaluate(CosTheta(wi)));
        /// TODO: from camera or light source
		/// Since haven't do bidirectional methods, just account for the energe transmitted
		Ft *= (etaI*etaI) / (etaT*etaT);
        return Ft / AbsCosTheta(wi);
    }
}
