#include "renderer/medium/Medium.h"
#include <cmath>

namespace Homura {
	Vec3f HomogeneousMedium::Tr(const Ray &r, std::shared_ptr<Sampler> sampler) const {
		return std::exp(_sigma_t * std::min(r.tmax, std::numeric_limits<float>::max()));
	}

	Vec3f HomogeneousMedium::sample(const Ray &r, std::shared_ptr<Sampler> sampler, MediumInterface &mi) const {
		// sample a channel to do exponential distribution
		int c = std::min(std::floor(sample->get1D() * 3), 2);
		// sample exponential distribution
		float sampled_dist = -std::log(1.f - sampler->get1D()) / _sigma_t[c];
		// if a valide intersection is sampled
		// initialize medium info
		sampled_dist = std::min(sampled_dist, ray._tMax);
		bool is_valid_sample = sampled_dist < r.tMax;
		if (is_valid_sample) {
			mi = MediumIntersectInfo();	/// TODO
		}
		Vec3f Tr = std::exp(-_sigma_t * std::min(sampled_dist, std::numeric_limits<float>::max()));

		// compute weight
		Vec3f p = is_valid_sample ? (_sigma_t * Tr) : (Tr);
		float pdf = 0.f;
		for (int i = 0; i < 3; i++) {
			pdf += p[i];
		}
		pdf /= 3;
		return is_valid_sample ? (_sigma_t * Tr) / pdf : (Tr) / pdf;
	}
}