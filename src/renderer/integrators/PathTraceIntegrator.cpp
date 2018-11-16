#include "renderer/integrators/PathTraceIntegrator.h"
#include "renderer/IntersectInfo.h"
#include "renderer/bxdfs/BxDF.h"
#include "renderer/emitters/Emitter.h"
#include <cmath>
#include <fstream>

namespace Homura {
	PathTraceIntegrator::PathTraceIntegrator(std::shared_ptr<Scene> sc, const Homura::JsonObject &json) :
	SamplerIntegrator(sc, json), _max_depth(json["max_depth"].getInt()), _rr_bounce(json["rr_bounce"].getInt()) {}

	Vec3f PathTraceIntegrator::Li(const Ray &r, std::unique_ptr<PixelSampler> sampler) const {
		Vec3f L(0.0f), throughput(1.0f);
		int depth = 0;
		Ray ray(r);	ray.setPrimary(true);
		//ray._d = Vec3f(-0.100815, -0.0894974, -0.990872);

		bool specular_bounce = false;

		while (depth < _max_depth) {
			depth++;
			IntersectInfo isect_info;
			bool intersected = _scene->intersect(ray, isect_info);

			//isect_info._normal = Vec3f(0.826997, 0.0954478, 0.554044);
			//isect_info._shading._n = Vec3f(0.826997, 0.0954478, 0.554044);
			//std::cout << "Ng: " << isect_info._normal << std::endl;
			//std::cout << "Ns: " << isect_info._shading._n << std::endl;

			if (!intersected)
				break;

#define PT_DBG 0
#if PT_DBG
#endif
			// compute emitted light if first bounce / previous is specular.
			if (depth == 1 || specular_bounce) {
				if (intersected) {	// previous bounce is specular
					//L += throughput * isect.Le();	/// TODO: if previous bouncing point is on specular objects, then light from light source is certainly not going into this path, then why should we still add the direct light here?
				}
				else {
					//for (auto _inf_light : _scene->_inf_emitters)
						//L += throughput * _inf_light.Le();
					//return L;	/// TODO: should return?
				}
			}

			isect_info.computeScatteringFunction();

			// Only account for direct light sampling for non-specular surfaces
			if (isect_info._bsdf->numComponents(BxDFType(BSDF_ALL & ~BSDF_SPECULAR)) > 0)
				L += throughput * evaluateDirect(isect_info);

			Vec3f wi;
			Vec3f wo = -ray._d;
			float bsdf_pdf;
			BxDFType sampled_bxdf_type_flags;
			Vec3f f = isect_info._bsdf->sample_f(wo, wi, bsdf_pdf, _sampler->get2D(), sampled_bxdf_type_flags);
			throughput *= f * std::abs(wi.dot(isect_info._shading._n)) / bsdf_pdf;
			specular_bounce = (sampled_bxdf_type_flags & BSDF_SPECULAR) != 0;

			if (throughput.max() < 1e-3)	break;	// too small contribution

			specular_bounce = (sampled_bxdf_type_flags & BSDF_SPECULAR);
			ray = isect_info.spawnRay(wi);

			if (depth > _rr_bounce) {
				float q = std::max(0.05f/*rr*/, 1.f - throughput.y()/*?*/);
				if (_sampler->get1D() < q)
					break;
				throughput /= (1.f - q);
			}
		}
		//std::cout << "original L: " << L << std::endl;
		return L;
	}
}