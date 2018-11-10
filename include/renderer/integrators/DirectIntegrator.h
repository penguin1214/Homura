#ifndef HOMURA_DIRECTINTEGRATOR_H_
#define HOMURA_DIRECTINTEGRATOR_H_

#include "Integrator.h"

namespace Homura {
	class DirectIntegrator : public SamplerIntegrator {
	public:
		DirectIntegrator(std::shared_ptr<Scene> sc, const JsonObject &json);

		virtual Vec3f Li(const Ray &r, std::unique_ptr<PixelSampler> sampler) const override;

		Vec3f evaluateDirect(const IntersectInfo &isect_info) const;

	private:
		//const int _max_depth;	// for specular
	};
}
#endif // !HOMURA_DIRECTINTEGRATOR_H_
