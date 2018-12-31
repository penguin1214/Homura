#ifndef HOMURA_PATHTRACEINTEGRATOR_H_
#define HOMURA_PATHTRACEINTEGRATOR_H_

#include "Integrator.h"

namespace Homura {
	class PathTraceIntegrator : public SamplerIntegrator {
	public:
		PathTraceIntegrator(std::shared_ptr<Scene> sc, const JsonObject &json);

		virtual Vec3f Li(const Ray &r, std::shared_ptr<PixelSampler> sampler) const override;

	private:

		const int _max_depth;
		const int _rr_bounce;
	};
}

#endif // !HOMURA_PATHTRACEINTEGRATOR_H_
