#include "renderer/integrators/DirectIntegrator.h"
#include "renderer/emitters/Emitter.h"
#include "renderer/bxdfs/BxDF.h"
#include "renderer/IntersectInfo.h"

namespace Homura {
	DirectIntegrator::DirectIntegrator(std::shared_ptr<Scene> sc, const JsonObject &json) :
	SamplerIntegrator(sc, json) {
		/// TODO: compute rounded sample count for every light
		///	TODO: request sample arrays from sampler in terms of the rounded sample count
	}

	Vec3f DirectIntegrator::Li(const Ray &r, std::unique_ptr<PixelSampler> sampler) const {
		Vec3f L(0.0f);
		Ray ray(r);

		IntersectInfo isect_info;
		bool intersected = _scene->intersect(ray, isect_info);

		if (!intersected) {
			/// TODO: compute environment light
			return L;
		}

		isect_info.computeScatteringFunction();
		
		/// TODO: add self-emitted light by intersected object

		L += evaluateDirect(isect_info);

		return L;
	}

	Vec3f DirectIntegrator::evaluateDirect(const IntersectInfo &isect_info) const {
		Vec3f L(0.0f);
		for (auto light : _scene->_emitters) {
			L += light->evalDirect(_scene, isect_info, _sampler->get2D());
		}
		return L;
	}
}