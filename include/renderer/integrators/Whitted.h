#ifndef HOMURA_WHITTED_H_
#define HOMURA_WHITTED_H_

#include"Integrator.h"

namespace Homura {
	class Whitted : public SamplerIntegrator {
	public:
		Whitted(std::shared_ptr<Scene> sc, const JsonObject &json);
		~Whitted();

		Vec3f Li(const Ray &r, std::unique_ptr<PixelSampler> sampler) const override;

	private:
		const int _max_depth;
	};

}
#endif // HOMURA_WHITTED_H_