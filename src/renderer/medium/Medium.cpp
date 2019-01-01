#include "renderer/medium/Medium.h"
#include "renderer/Ray.h"
#include "core/sampler/Sampler.h"
#include <cmath>

namespace Homura {
	Vec3f HomogeneousMedium::Tr(const Ray &r, std::shared_ptr<PixelSampler> sampler) const {
		Vec3f tmp = _sigma_t * std::min(r._tmax, std::numeric_limits<float>::max());
		return Vec3f(std::exp(tmp.x()), std::exp(tmp.y()), std::exp(tmp.z()));
	}

	Vec3f HomogeneousMedium::sample(const Ray &r, std::shared_ptr<PixelSampler> sampler, MediumIntersectInfo &mi) const {
		// sample a channel to do exponential distribution
		int c = std::min((int)std::floor(sampler->get1D() * 3), 2);

		// sample exponential distribution
		float sampled_dist = -std::log(1.f - sampler->get1D()) / _sigma_t[c];

		// if a valide intersection is sampled
		// initialize medium info
		sampled_dist = std::min(sampled_dist, r._tmax);
		bool is_valid_sample = sampled_dist < r._tmax;
		if (is_valid_sample) {
			mi = MediumIntersectInfo(r._o+r._d*sampled_dist, -r._d, _phase);
		}

		Vec3f tmp = -_sigma_t * std::min(sampled_dist, std::numeric_limits<float>::max());
		Vec3f Tr(std::exp(tmp.x()), std::exp(tmp.y()), std::exp(tmp.z()));

		// compute weight
		Vec3f p = is_valid_sample ? (_sigma_t * Tr) : (Tr);
		float pdf = 0.f;
		for (int i = 0; i < 3; i++) {
			pdf += p[i];
		}
		pdf /= 3;
		return is_valid_sample ? ((_sigma_t * Tr) / pdf) : ((Tr) / pdf);
	}
}