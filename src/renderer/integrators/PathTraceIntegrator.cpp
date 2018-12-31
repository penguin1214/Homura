#include "renderer/integrators/PathTraceIntegrator.h"
#include "renderer/IntersectInfo.h"
#include "renderer/bxdfs/BxDF.h"
#include "renderer/phase/PhaseFunction.h"
#include "renderer/emitters/Emitter.h"
#include <cmath>
#include <fstream>

namespace Homura {
	PathTraceIntegrator::PathTraceIntegrator(std::shared_ptr<Scene> sc, const Homura::JsonObject &json) :
		SamplerIntegrator(sc, json), _max_depth(json["max_depth"].getInt()), _rr_bounce(json["rr_bounce"].getInt()) {}

	Vec3f PathTraceIntegrator::Li(const Ray &r, std::shared_ptr<PixelSampler> sampler) const {
		Vec3f L(0.0f), throughput(1.0f);
		int depth = 0;
		Ray ray(r);	ray.setPrimary(true);

		bool specular_bounce = false;

		while (depth < _max_depth) {
			depth++;

			IntersectInfo isect_info;
			bool intersected = _scene->intersect(ray, isect_info);
			if (!intersected)
				break;

			MediumIntersectInfo mi;
			if (ray._medium)
				throughput *= ray._medium->sample(ray, sampler, mi);
			if (throughput.max() < 1e-6)
				break;
			//std::cout << "ray: " << ray._o << "->" << ray._d << std::endl;
			//std::cout << "hit point: " << isect_info._p << std::endl;


			//std::cout << "point: " << isect_info._p << std::endl;
			//std::cout << "Ng: " << isect_info._normal << std::endl;
			//std::cout << "Ns: " << isect_info._shading._n << std::endl;
			//std::cout << "depth" << depth << ": " << intersected << std::endl;

			if (mi.isValid()) {
				// trace ray in medium
				// compute direct illumination
					// sample a light
				L += throughput * evaluateDirect(mi, sampler);
				// sample phase function
				Vec3f wi;
				Vec3f wo = -ray._d;
				mi._phase->sample_p(wo, wi, sampler->get2D());
				// sample meidum
				ray = mi.spawnRay(wi);	/// TODO: override!
				specular_bounce = false;
			}
			else {
				// compute emitted light if first bounce / previous is specular.
				if (depth == 1 || specular_bounce) {
					if (intersected) {	// previous bounce is specular
						L += throughput * isect_info.Le(-ray._d);	/// TODO: if previous bouncing point is on specular objects, then light from light source is certainly not going into this path, then why should we still add the direct light here?
					}
					else {
						//for (auto _inf_light : _scene->_inf_emitters)
							//L += throughput * _inf_light.Le();
						//return L;	/// TODO: should return?
					}
				}

				isect_info.computeScatteringFunction();

				// Only account for direct light sampling for non-specular surfaces
				if (isect_info._bsdf->numComponents(BxDFType(BSDF_ALL & ~BSDF_SPECULAR)) > 0) {
					//if (depth > 1)
					L += throughput * evaluateDirect(isect_info, sampler);
				}
				//std::cout << "bouce" << depth << " L: " << L << std::endl;

				Vec3f wi;
				Vec3f wo = -ray._d;
				float bsdf_pdf;
				BxDFType sampled_bxdf_type_flags;
				//Vec3f f = isect_info._bsdf->sample_f(wo, wi, bsdf_pdf, 0.5f, Point2f(0.5f, 0.5f), BSDF_ALL, &sampled_bxdf_type_flags);
				Vec3f f = isect_info._bsdf->sample_f(wo, wi, bsdf_pdf, _sampler->get1D(), _sampler->get2D(), BSDF_ALL, &sampled_bxdf_type_flags);
				//std::cout << "bounce" << depth << " wi: " << wi << std::endl;
				//std::cout << "bouce" << depth << " f: " << f << std::endl;
				//std::cout << "bouce" << depth << " bsdf_pdf: " << bsdf_pdf << std::endl;

				if ((f.max() < 1e-6) || (bsdf_pdf < 1e-6)) break;

				throughput *= f * std::abs(wi.dot(isect_info._shading._n)) / bsdf_pdf;
				specular_bounce = (sampled_bxdf_type_flags & BSDF_SPECULAR) != 0;

				//std::cout << "bouce" << depth << " throughput: " << throughput << std::endl;

				if (throughput.max() < 1e-3)	break;	// too small contribution

				ray = isect_info.spawnRay(wi);
				//std::cout << "sampled direction: " << wi << std::endl;
			}
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