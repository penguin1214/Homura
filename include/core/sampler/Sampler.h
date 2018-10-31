#ifndef HOMURA_SAMPLER_H_
#define HOMURA_SAMPLER_H_

#include <cstdint>
#include <vector>
#include "../math/Geometry.h"
#include "../../renderer/sensors/Sensor.h"
#include "core/io/JsonDocument.h"
#include "rng.h"
#include "Sampling.h"

namespace Homura {

	//static const double OneMinusEpsilon = 0x1.fffffffffffffp-1;

	class Sampler {
	public:
		//Sampler(int64_t spp);
		Sampler(const JsonObject &json);
		//Sampler(Sampler&);

		virtual void startPixel(const Vec2i &p);
		virtual bool startNextSample();
		virtual bool setSampleIdx(int64_t idx);

		virtual float get1D() = 0;
		virtual Vec2f get2D() = 0;

		virtual int64_t currentSampleIdx() const;
		virtual void request1DArray(int n);
		virtual void request2DArray(int n);
		virtual const float* get1DArray(int n);
		virtual const Vec2f* get2DArray(int n);

		const int64_t _spp;

	protected:
		Sampler(const Sampler& origin);

		Vec2i _current_pixel;
		int64_t _current_pixel_sample_idx;

		std::vector<int> _samples1DArraySizes, _samples2DArraySizes;
		/*
		------------------------------------------------------------------------------------------------------------------------
		| sample arr 1 for pixel_1 | sample arr 1 for pixel_2 | ... | sample arr 1 for pixel_n | sample arr 2 for pixel_1 | ...|
		------------------------------------------------------------------------------------------------------------------------
		*/
		std::vector<std::vector<float>> _sampleArray1D;
		std::vector<std::vector<Vec2f>> _sampleArray2D;

		/* nth sample array. */
		size_t _array1DOffset, _array2DOffset;
	};

	/*
	Generate all samples at once.
	*/
	class PixelSampler : public Sampler {
	public:
		//PixelSampler(int64_t spp, int ndim);
		PixelSampler(const JsonObject &json);
		virtual void startPixel(const Vec2i &p) override = 0;
		bool startNextSample() override;
		bool setSampleIdx(int64_t idx) override;
		PixelSample getSensorSample(Vec2i p);

		float get1D() override;
		Vec2f get2D() override;
		void resetSeed(const int &seed);

		virtual std::unique_ptr<PixelSampler> clone(const int &seed) const = 0;

	protected:
		PixelSampler(const PixelSampler &origin);

		std::vector<std::vector<float>> _samples1D;	// precomputed samples for non-array usage
		std::vector<std::vector<Vec2f>> _samples2D;
		int _current_1D_dim_idx = 0, _current_2D_dim_idx = 0;
		RNG rng;
	};
}

#endif // !HOMURA_SAMPLER_H_