#ifndef HOMURA_INTEGRATOR_H_
#define HOMURA_INTEGRATOR_H_

#include "core/math/Geometry.h"
#include "renderer/Scene.h"
#include "core/sampler/Sampler.h"
#include "core/sampler/UniformSampler.h"
#include "core/sampler/StratifiedSampler.h"
#include "core/io/JsonDocument.h"

namespace Homura {
	class Integrator {
	public:
		Integrator() = default;
		~Integrator();

		virtual void render() = 0;
	};

	class SamplerIntegrator : public Integrator {
	public:
		SamplerIntegrator(std::shared_ptr<Scene> sc, const JsonObject &json);
		~SamplerIntegrator() = default;

		void render() override;
		virtual Vec3f Li(const Ray &r, std::unique_ptr<PixelSampler> sampler) const = 0;

		std::shared_ptr<Scene> _scene;
		std::unique_ptr<PixelSampler> _sampler;

	protected:
		virtual Vec3f evaluateDirect(const IntersectInfo &isect_info) const;
	};
}

#endif // !HOMURA_INTEGRATOR_H_
