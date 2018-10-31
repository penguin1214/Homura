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

		/// TODO: comprehensive test
		/// The code now is for left-handed system, we need right-handed.
		/*CoordinateSystem(Vec3f &v_) {
			assert(v_.length() != 0);
			u = Vec3f(v_);
			if (std::abs(u[0]) > std::abs(u[1])) {
				v = Vec3f(-v_.z(), 0, v_.x()); v.normalize();
			}
			else {
				v = Vec3f(0, v_.z(), -v_.y()); v.normalize();
			}
			w = u.cross(v);
		}*/

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
}
#endif	// !HOMURA_COORDINATE_SYSTEM_H_

