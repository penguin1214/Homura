#ifndef HOMURA_SAMPLING_H_
#define HOMURA_SAMPLING_H_
#include "rng.h"
#include "../math/Geometry.h"
#include "core/Common.h"

namespace Homura {
	void StratifiedSample1D(float *samples, int n_sample, RNG &rng, bool jitter);
	void StratifiedSample2D(Vec2f *samples, int n_x, int n_y, RNG &rng, bool jitter);

	void LatinHypercube(float *samples, int n_sample, int ndim, RNG &rng);
	
	/// TODO: what other sampling methods need ndim other than 1?

	template <typename T>
	void Shuffle(T *samples, int count, int ndim, RNG &rng) {
		for (int i = 0; i < count; i++) {
			int other = i + rng.UniformUInt32(count - i);
			for (int j = 0; j < ndim; j++) {
				std::swap(samples[i*ndim + j], samples[other*ndim + j]);
			}
		}
	}

	Vec3f uniformSampleHemisphereSolidAngle(const Point2f &u);
}

#endif // !HOMURA_SAMPLING_H_