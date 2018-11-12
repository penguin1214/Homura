#include "core/sampler/Sampling.h"

namespace Homura {
	void StratifiedSample1D(float *samples, int n_sample, RNG &rng, bool jitter) {
		float inv_n = 1.0f / (float)n_sample;
		for (int i = 0; i < n_sample; i++) {
			float delta = jitter ? rng.UniformFloat() : 0.5f;
			samples[i] = std::min((i + delta) * inv_n, OneMinusEpsilon);
		}
	}

	void StratifiedSample2D(Vec2f *samples, int n_x, int n_y, RNG &rng, bool jitter) {
		float inv_x = 1.0f / (float)n_x;
		float inv_y = 1.0f / (float)n_y;
		for (int y = 0; y < n_y; y++) {
			for (int x = 0; x < n_x; x++) {
				float delta_x = jitter ? rng.UniformFloat() : 0.5f;
				float delta_y = jitter ? rng.UniformFloat() : 0.5f;
				samples[y*n_x + x] = Vec2f(std::min((x+delta_x)*inv_x, OneMinusEpsilon)
											,std::min((y+delta_y)*inv_y, OneMinusEpsilon));
			}
		}
	}

	void LatinHypercube(float *samples, int n_sample, int ndim, RNG &rng) {
		float inv_n = 1.0f / (float)n_sample;
		for (int i = 0; i < n_sample; i++) {
			for (int j = 0; j < ndim; j++) {
				float s = (i + rng.UniformFloat())*inv_n;
				samples[i*ndim + j] = std::min(s, OneMinusEpsilon);
			}
		}
		for (int i = 0; i < ndim; i++) {
			for (int j = 0; j < n_sample; j++) {
				int other = i + rng.UniformUInt32(n_sample - i);
				std::swap(samples[j*ndim + i], samples[other*ndim + i]);
			}
		}
	}

	Vec3f uniformSampleHemisphereSolidAngle(const Point2f &u) {
		float z = u[0];
		float r = std::sqrt(std::max(0.f, 1.f - z * z));
		float phi = 2 * PI * u[1];
		return Vec3f(r*std::cos(phi), r*std::sin(phi), z);
	}

	Vec3f cosineSampleHemisphereSolidAngle(const Point2f &u) {
		float sqrt = std::sqrt(u[0]);
		float d = TWOPI * u[1];
		return Vec3f(std::cos(d)*sqrt, std::sin(d)*sqrt, std::sqrt(std::max(1.f - u[0], 0.f)));
	}
}