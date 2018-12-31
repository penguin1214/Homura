#ifndef HOMURA_PHASEFUNCTION_H_
#define HOMURA_PHASEFUNCTION_H_

#include "core/Common.h"
#include "core/math/Geometry.h"
#include "core/math/CoordinateSystem.h"

namespace Homura {
	class PhaseFunction {
	public:
		PhaseFunction() = default;
		virtual float p(const Vec3f &wo, const Vec3f &wi) const = 0;	/// TODO: float or Vec3f?
		virtual float sample_p(const Vec3f &wo, Vec3f &wi, const Point2f &u) const = 0;
	};

	class HenyeyGreenstein : public PhaseFunction {
	public:
		HenyeyGreenstein(const float &g) : _g(g) {}

		float p(const Vec3f &wo, const Vec3f &wi) const override {
			float d = 1.f + _g * _g + 2 * _g*wo.dot(wi);
			return INV4PI * (1.f - _g * _g) / std::pow(d, 1.5f);
		}

		float sample_p(const Vec3f &wo, Vec3f &wi, const Point2f &u) const override {
			float cos_theta;
			if (std::abs(_g) < 1e-6)
				cos_theta = 1 - 2 * u[0];
			else {
				float tmp = (1.f - _g * _g) / (1.f - _g + 2.f * _g * u[0]);
				cos_theta = (1.f + _g*_g - tmp * tmp) / (2.f*_g);
			}

			float sin_theta = std::sqrt(std::max(0.f, 1.f - cos_theta * cos_theta));
			float phi = 2.f * PI * u[1];
			Vec3f v1, v2;
			ShadingCoordinateSystem(wo, v1, v2);	/// TODO: shading coord?
			wi = sphericalDirection(sin_theta, cos_theta, phi, v1, v2, wo);	/// TODO: -wo?
		}

	private:
		float _g;
	};
}

#endif // !HOMURA_PHASEFUNCTION_H_
