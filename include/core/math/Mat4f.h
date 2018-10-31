#ifndef HOMURA_MAT4F_H_
#define HOMURA_MAT4F_H_

#include <iostream>
#include "Geometry.h"

namespace Homura {

	class Mat4f {
	private:
		union {
			struct {
				float e11, e12, e13, e14;
				float e21, e22, e23, e24;
				float e31, e32, e33, e34;
				float e41, e42, e43, e44;
			};
			float e[16];
		};
	public:
		Mat4f();
		Mat4f(
			float a11, float a12, float a13, float a14,
			float a21, float a22, float a23, float a24,
			float a31, float a32, float a33, float a34,
			float a41, float a42, float a43, float a44);
		Mat4f(Mat4f *m);

		static Mat4f transpose(const Mat4f &m);
		static Mat4f inverse(const Mat4f &m);
		static Mat4f translate(Vec3f t);
		static Mat4f scale(Vec3f s);
		//static Mat4f rotateXYZ(Vec3f r);
		static Mat4f rotAxis(Vec3f &axis, float angle);
		static Mat4f orthographic(float near, float far);
		static Mat4f perspective(float fov);
		//static Mat4f perspective(float near, float far, float fov);

		float& operator[](unsigned i) { return e[i]; }
		float operator[](unsigned i) const { return e[i]; }
		float& operator()(unsigned i, unsigned j) { return e[i * 4 + j]; }

		Mat4f operator +(const Mat4f &m) {
			Mat4f ret(*this);
			for (unsigned i = 0; i < 16; i++) { ret.e[i] += m.e[i]; }
			return ret;
		}

		Mat4f operator -(const Mat4f &m) {
			Mat4f ret(*this);
			for (unsigned i = 0; i < 16; i++) { ret.e[i] -= m.e[i]; }
			return ret;
		}

		Mat4f operator*(float a) {
			Mat4f ret(*this);
			for (unsigned i = 0; i < 16; i++) {
				ret.e[i] *= a;
			}
			return ret;
		}

		Mat4f operator/(float a) {
			assert(a != 0);
			Mat4f ret(*this);
			for (unsigned i = 0; i < 16; i++) {
				ret.e[i] /= a;
			}
			return ret;
		}

		Mat4f operator*(const Mat4f &other) {
			Mat4f ret(*this);
			for (unsigned i = 0; i < 4; i++) {
				for (unsigned j = 0; j < 4; j++) {
					ret(i, j) =
						e[i * 4 + 0] * other.e[0 * 4 + j] +
						e[i * 4 + 1] * other.e[1 * 4 + j] +
						e[i * 4 + 2] * other.e[2 * 4 + j] +
						e[i * 4 + 3] * other.e[3 * 4 + j];
				}
			}
			return ret;
		}

		friend Mat4f operator*(const Mat4f &a, const Mat4f &b) {
			Mat4f ret;
			for (unsigned i = 0; i < 4; i++) {
				for (unsigned j = 0; j < 4; j++) {
					ret[i * 4 + j] =
						a[i * 4 + 0] * b[0 * 4 + j] +
						a[i * 4 + 1] * b[1 * 4 + j] +
						a[i * 4 + 2] * b[2 * 4 + j] +
						a[i * 4 + 3] * b[3 * 4 + j];
				}
			}
			return ret;
		}

		/// TODO: whether the usage of below two multiplication functions still needs to be checked!
		friend Vec4f operator*(const Vec4f &b, const Mat4f &a) {
			return Vec4f(
				a.e11*b.x() + a.e21*b.y() + a.e31*b.z() + a.e41*b.w(),
				a.e12*b.x() + a.e22*b.y() + a.e32*b.z() + a.e42*b.w(),
				a.e13*b.x() + a.e23*b.y() + a.e33*b.z() + a.e43*b.w(),
				a.e14*b.x() + a.e24*b.y() + a.e34*b.z() + a.e44*b.w());
		}

		friend Vec4f operator*(const Mat4f &a, const Vec4f &b) {
			return Vec4f(
				a.e11*b.x() + a.e12*b.y() + a.e13*b.z() + a.e14*b.w(),
				a.e21*b.x() + a.e22*b.y() + a.e23*b.z() + a.e24*b.w(),
				a.e31*b.x() + a.e32*b.y() + a.e33*b.z() + a.e34*b.w(),
				a.e41*b.x() + a.e42*b.y() + a.e43*b.z() + a.e44*b.w());
		}

		friend Point3f operator*(const Point3f b, const Mat4f &a) {
			float xp = a.e11*b.x() + a.e21*b.y() + a.e31*b.z() + a.e41;
			float yp = a.e12*b.x() + a.e22*b.y() + a.e32*b.z() + a.e42;
			float zp = a.e13*b.x() + a.e23*b.y() + a.e33*b.z() + a.e43;
			float wp = a.e14*b.x() + a.e24*b.y() + a.e34*b.z() + a.e44;
			/// TODO: under what circumstances would homogenous coord wp be other than 1?
			if (wp == 1)
				return Point3f(xp, yp, zp);
			else {
				float inv_wp = 1.0f / wp;
				return Point3f(xp * inv_wp, yp * inv_wp, zp * inv_wp);
			}
		}

		friend Vec3f operator*(const Vec3f v, const Mat4f &a) {
			return Vec3f(
				a.e11*v.x() + a.e21*v.y() + a.e31*v.z(),
				a.e12*v.x() + a.e22*v.y() + a.e32*v.z(),
				a.e13*v.x() + a.e32*v.y() + a.e33*v.z());
		}

		friend std::ostream& operator<<(std::ostream &os, const Mat4f &m) {
			std::cout << "(" << std::endl;
			for (unsigned i = 0; i < 4; i++) {
				os << m[i * 4 + 0] << "," << m[i * 4 + 1] << "," << m[i * 4 + 2] << "," << m[i * 4 + 3] << std::endl;
			}
			std::cout << ")" << std::endl;
			return os;
		}
	};

}

#endif // !HOMURA_MAT4F_H_