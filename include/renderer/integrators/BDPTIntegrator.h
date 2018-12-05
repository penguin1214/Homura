#ifndef HOMURA_BDPTINTEGRATOR_H_
#define HOMURA_BDPTINTEGRATOR_H_

#include "Integrator.h"

namespace Homura {
	class BDPTIntegrator : public SamplerIntegrator {
	public:
		PathTraceIntegrator(std::shared_ptr<Scene> sc, const JsonObject &json);

	private:
		float correctShadingNormal() const;
	};
}

#endif // !HOMURA_BDPTINTEGRATOR_H_
