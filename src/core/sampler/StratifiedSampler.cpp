#include "core/sampler/StratifiedSampler.h"

namespace Homura {
	//StratifiedSampler::StratifiedSampler(int xSamples, int ySamples, bool jitterSamples, int ndim) 
	//	: PixelSampler(xSamples*ySamples, ndim), _x_pixel_samples(xSamples), _y_pixel_samples(ySamples), _jitter(jitterSamples) {}
	StratifiedSampler::StratifiedSampler(const JsonObject &json)
		: PixelSampler(json), _x_pixel_samples(std::sqrt(_spp)), _y_pixel_samples(std::sqrt(_spp)), _jitter(json["jitter"].getBool()) {}

	void StratifiedSampler::startPixel(const Vec2i &p) {
		// generate samples and fill _samples1D, _samples2D.
		for (int i = 0; i < _samples1D.size(); i++) {
			StratifiedSample1D(&_samples1D[i][0], _spp, rng, _jitter);
			Shuffle(&_samples1D[i][0], _spp, 1, rng);
		}
		for (int i = 0; i < _samples2D.size(); i++) {
			StratifiedSample2D(&_samples2D[i][0], _x_pixel_samples, _y_pixel_samples, rng, _jitter);
			Shuffle(&_samples2D[i][0], _spp, 1, rng);
		}
		// generate sample arrays
		for (int i = 0; i < _samples1DArraySizes.size(); i++) {
			for (int64_t j = 0; j < _spp; j++) {
				int cnt = _samples1DArraySizes[i];
				StratifiedSample1D(&_sampleArray1D[i][j*cnt], cnt, rng, _jitter);
				Shuffle(&_sampleArray1D[i][j*cnt], cnt, 1, rng);
			}
		}
		for (int i = 0; i < _samples2DArraySizes.size(); i++) {
			for (int64_t j = 0; j < _spp; j++) {
				int cnt = _samples2DArraySizes[i];
				LatinHypercube(&_sampleArray2D[i][j*cnt][0], cnt, 2, rng);
			}
		}
		Sampler::startPixel(p);
	}

	std::shared_ptr<PixelSampler> StratifiedSampler::clone(const int &seed) const {
		auto cloned = std::make_shared<StratifiedSampler>(*this);
		cloned->resetSeed(seed);
		return cloned;
	}
}