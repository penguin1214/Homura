#ifndef HOMURA_MEDIUM_H_
#define HOMURA_MEDIUM_H_

namespace Homura {
	class PixelSampler;
	struct MediumIntersectInfo;

	class Medium {
	public:
		/* Assume ray is unoccluded and fully-contained in the medium! */
		virtual Vec3f Tr(const Ray &r, std::shared_ptr<PixelSampler> sampler) const = 0;
		virtual Vec3f sample(const Ray &r, std::shared_ptr<PixelSampler> sampler, MediumIntersectInfo &mi) const = 0;
	};

	class MediumInterface {
	public:
		MediumInterface() : _inside(nullptr), _outside(nullptr) {}
		MediumInterface(const MediumInterface &other) : _inside(other._inside), _outside(other._outside) {}
		MediumInterface(std::shared_ptr<Medium> in, std::shared_ptr<Medium> out) : _inside(in), _outside(out) {}

		bool isTransition() const { return _inside != _outside; }
		bool isValid() const { return _inside != nullptr && _outside != nullptr; }

		std::shared_ptr<Medium> _inside;
		std::shared_ptr<Medium> _outside;
	};

	class HomogeneousMedium : public Medium {
	public:
		HomogeneousMedium(const Vec3f &sa, const Vec3f &ss, const float &g) : _sigma_a(sa), _sigma_s(ss), _sigma_t(_sigma_a + _sigma_s), _HGg(g) {}

		Vec3f Tr(const Ray &r, std::shared_ptr<PixelSampler> sampler) const override { return Vec3f(1.f); }	/// TODO
		Vec3f sample(const Ray &r, std::shared_ptr<PixelSampler> sampler, MediumIntersectInfo &mi) const override { return Vec3f(1.f); }	/// TODO

	private:
		const Vec3f _sigma_a, _sigma_s, _sigma_t;
		const float _HGg;
	};
}

#endif // !HOMURA_MEDIUM_H_
