#include "core/math/Mat4f.h"

#include "core/Common.h"

namespace Homura {
	Mat4f::Mat4f() {
		e11 = e22 = e33 = e44 = 1.0f;
		e12 = e13 = e14 = 0.0f;
		e21 = e23 = e24 = 0.0f;
		e31 = e32 = e34 = 0.0f;
		e41 = e42 = e43 = 0.0f;
	}

	Mat4f::Mat4f(
		float a11, float a12, float a13, float a14,
		float a21, float a22, float a23, float a24,
		float a31, float a32, float a33, float a34,
		float a41, float a42, float a43, float a44) {
		e11 = a11, e12 = a12, e13 = a13, e14 = a14;
		e21 = a21, e22 = a22, e23 = a23, e24 = a24;
		e31 = a31, e32 = a32, e33 = a33, e34 = a34;
		e41 = a41, e42 = a42, e43 = a43, e44 = a44;
	}

	Mat4f::Mat4f(Mat4f *m) {
		for (unsigned i = 0; i < 16; i++) {
			e[i] = m->e[i];
		}
	}

	// TODO: Gaussian-Jordan
	Mat4f Mat4f::inverse(const Mat4f &m) {
		int indxc[4], indxr[4];
		int ipiv[4] = { 0, 0, 0, 0 };
		float minv[4][4];
		memcpy(minv, m.e, 4 * 4 * sizeof(float));
		for (int i = 0; i < 4; i++) {
			int irow = 0, icol = 0;
			float big = 0.f;
			// Choose pivot
			for (int j = 0; j < 4; j++) {
				if (ipiv[j] != 1) {
					for (int k = 0; k < 4; k++) {
						if (ipiv[k] == 0) {
							if (std::abs(minv[j][k]) >= big) {
								big = float(std::abs(minv[j][k]));
								irow = j;
								icol = k;
							}
						}
						else if (ipiv[k] > 1)
							std::cerr << ("Singular matrix in MatrixInvert") << std::endl;
					}
				}
			}
			++ipiv[icol];
			// Swap rows _irow_ and _icol_ for pivot
			if (irow != icol) {
				for (int k = 0; k < 4; ++k) std::swap(minv[irow][k], minv[icol][k]);
			}
			indxr[i] = irow;
			indxc[i] = icol;
			if (minv[icol][icol] == 0.f)
				std::cerr << ("Singular matrix in MatrixInvert") << std::endl;

			// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
			float pivinv = 1. / minv[icol][icol];
			minv[icol][icol] = 1.;
			for (int j = 0; j < 4; j++) minv[icol][j] *= pivinv;

			// Subtract this row from others to zero out their columns
			for (int j = 0; j < 4; j++) {
				if (j != icol) {
					float save = minv[j][icol];
					minv[j][icol] = 0;
					for (int k = 0; k < 4; k++) minv[j][k] -= minv[icol][k] * save;
				}
			}
		}
		// Swap columns to reflect permutation
		for (int j = 3; j >= 0; j--) {
			if (indxr[j] != indxc[j]) {
				for (int k = 0; k < 4; k++)
					std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
			}
		}

		Mat4f ret;
		for (int y = 0; y < 4; y++) {
			for (int x = 0; x < 4; x++) {
				ret[x * 4 + y] = minv[x][y];
			}
		}
		return ret;
	}

	Mat4f Mat4f::transpose(const Mat4f &m) {
		return Mat4f(
			m.e11, m.e21, m.e31, m.e41,
			m.e12, m.e22, m.e32, m.e42,
			m.e13, m.e23, m.e33, m.e43,
			m.e14, m.e24, m.e34, m.e44
		);
	}

	Mat4f Mat4f::translate(Vec3f t) {
		Mat4f m;
		m.e41 = t.x(), m.e42 = t.y(), m.e43 = t.z();
		return m;
	}

	Mat4f Mat4f::scale(Vec3f s) {
		Mat4f m;
		m.e11 = s.x(), m.e22 = s.y(), m.e33 = s.z();
		return m;
	}

	/*
	//Mat4f Mat4f::rotateXYZ(Vec3f r) {
	//	Vec3f deg = r * PI / 180.0f;
	//	float c[] = { std::cos(deg.x()), std::cos(deg.y()), std::cos(deg.z()) };
	//	float s[] = { std::sin(deg.x()), std::sin(deg.y()), std::sin(deg.z()) };

	//	return Mat4f(
	//		c[1] * c[2], -c[0] * s[2] + s[0] * s[1] * c[2], s[0] * s[2] + c[0] * s[1] * c[2], 0.0f,
	//		c[1] * s[2], c[0] * c[2] + s[0] * s[1] * s[2], -s[0] * c[2] + c[0] * s[1] * s[2], 0.0f,
	//		-s[1], s[0] * c[1], c[0] * c[1], 0.0f,
	//		0.0f, 0.0f, 0.0f, 1.0f
	//	);
	//}
	*/

	Mat4f Mat4f::rotAxis(Vec3f &axis, float angle) {
		float deg = angle * PI / 180.0f;
		float ca = std::cos(deg);
		float sa = std::sin(deg);
		float K = 1.0f - ca;
		float a = axis.x(), b = axis.y(), c = axis.z();
		return Mat4f(
			a*a*K + ca, a*b*K + c * sa, a*c*K - b * sa, 0.0f,
			a*b*K - c * sa, b*b*K + ca, b*c*K + a * sa, 0.0f,
			a*c*K + b * sa, b*c*K - a * sa, c*c*K + ca, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	Mat4f Mat4f::orthographic(float near, float far) {
		return translate(Vec3f(0, 0, -near)) * scale(Vec3f(1.0f, 1.0f, 1.0f / (far - near)));
	}

	Mat4f Mat4f::perspective(float fov) {
		// consider screen space as a 2D space now!
		// thus z-value of projected points are not important!
		// also don't consider aspect ratio now.
		float scale = 1.0f / std::tan(deg2rad(0.5*fov));
		return Mat4f(
			scale, 0.0f, 0.0f, 0.0f,
			0.0f, scale, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f, -1.0f,
			0.0f, 0.0f, 1e-6, 0.0f);
		// after perspective divide, z=1.
		//However, it actually doesn't matter if z = 1 or -1,
		//since we then only deal with 2D.
		/*float scale = 1.0f / std::tan(deg2rad(0.5*fov));
		return Mat4f(
			scale, 0.0f, 0.0f, 0.0f,
			0.0f, scale, 0.0f, 0.0f,
			0.0f, 0.0f, far / (far - near), -1.0f,
			0.0f, 0.0f, -(far * near) /(far - near), 0.0f
		);*/
	}
}