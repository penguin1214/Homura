#include "renderer/integrators/Whitted.h"
#include "renderer/IntersectInfo.h"
#include "renderer/bxdfs/BxDF.h"
#include "renderer/emitters/Emitter.h"

namespace Homura {
	Whitted::Whitted(std::shared_ptr<Scene> sc, const JsonObject &json)
		: SamplerIntegrator(sc, json), _max_depth(json["max_depth"].getInt()) {}

	Vec3f Whitted::Li(const Ray &r, std::shared_ptr<PixelSampler> sampler) const {
		Vec3f L(0.0f);
		// intersection
		IntersectInfo isect_info;
		Ray ray(r);
		if (_scene->intersect(ray, isect_info)) {
		    // init variables, what about bsdf coord?
		    const Vec3f &normal = isect_info._shading._n;	/// why use reference?
		    const Vec3f &wo = isect_info._wo;

			isect_info.computeScatteringFunction();	// compute surface attributes
			/// 1. Le
			//L += isect_info.Le();	// geometry-irrelevent light source
			/// 2. compute direct light radiance
			///		- compute light from intersect point to light wi, sample Li (shadow ray test)
			///		- sample BSDF using wi
			//for (const auto &light : _scene->_emitters) {
			for (size_t i = 0; i < _scene->_emitters.size(); i++) {
				std::shared_ptr<Emitter> light = _scene->_emitters[i]->getEmitter();
				Vec3f wi;
				float pdf;
				/// TODO: visibility tester?
				VisibilityTester vt(isect_info, IntersectInfo(light->pos()));
				Vec3f Li = light->sample_Li(isect_info, wi, pdf, vt, Point2f(1,1)/*TODO*/);
				Vec3f f = isect_info._bsdf->f(wo, wi);
				L += f*Li*(std::abs(wi.dot(normal)))/pdf;
			}
			/// 3. reflect/transmit ray to compute wo(to next intersect point), why??? What is this element in the rendering equation
			return L;
		}
		else {
			// compute back ground radiance
//			for (auto &l : _scene._emitters)
//				L += l->Le(r);	/// background radiance?
			return L;
		}
	}

	Whitted::~Whitted() {}

}