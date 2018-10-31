#ifndef HOMURA_STRATIFIEDSAMPLER_H_
#define HOMURA_STRATIFIEDSAMPLER_H_
#include "Sampler.h"

namespace Homura {
	class StratifiedSampler : public PixelSampler {
	public:
		StratifiedSampler(const JsonObject &json);
		//StratifiedSampler(int xSamples, int ySamples, bool jitterSamples, int ndim);
		void startPixel(const Vec2i &p) override;
		std::unique_ptr<PixelSampler> clone(const int &seed) const override;

	private:
		const int _x_pixel_samples, _y_pixel_samples;
		const bool _jitter;
	};
}

#endif // !HOMURA_STRATIFIEDSAMPLER_H_