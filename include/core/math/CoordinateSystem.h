#ifndef HOMURA_COORDINATE_SYSTEM_H_
#define HOMURA_COORDINATE_SYSTEM_H_

#include "Geometry.h"

namespace Homura {
	class CoordinateSystem {
	public:
		Vec3f _right, _up, _forward;

		CoordinateSystem() {
			_right = Vec3f(1.f, 0.f, 0.f);
			_up = Vec3f(0.f, 1.f, 0.f);
			_forward = Vec3f(0.f, 0.f, 1.f);
		}

		CoordinateSystem(const Vec3f &pos, const Vec3f &lookat, const Vec3f &up) {
			//Vec3f forward = (lookat - pos).normalized();
			_forward = (pos - lookat).normalized();	// camera facing opposite of z-axis of its coordinate system
			_right = (up.cross(_forward)).normalized();
			_up = (_forward.cross(_right)).normalized();
		}

//		Vec3f operator() (const Vec3f &v) const {}	/// TODO

		friend inline std::ostream& operator<<(std::ostream &os, const CoordinateSystem& frame) {
			os << "right: " << frame._right << std::endl << "up: " << frame._up << std::endl << "forward: " << frame._forward << std::endl;
			return os;
		}
	};

	class ShadingCoordinateSystem : public CoordinateSystem {
	public:
		/// TODO: comprehensive test
		ShadingCoordinateSystem(const Vec3f &n, Vec3f &t, Vec3f &bi) {
			float sign = copysignf(1.0f, n.z());
			const float a = -1.0f / (sign + n.z());
			const float b = n.x()*n.y()*a;
			t = Vec3f(1.0f + sign * n.x()*n.x()*a, sign*b, -sign * n.x());
			bi = n.cross(t);
		}
	};

	inline Vec3f sphericalDirection(const float &sin_theta, const float &cos_theta, const float phi, const Vec3f &x, const Vec3f &y, const Vec3f z) {
		return (sin_theta*std::cos(phi)*x + sin_theta * std::sin(phi)*y + cos_theta * z);
	}
}
#endif	// !HOMURA_COORDINATE_SYSTEM_H_

