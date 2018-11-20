#ifndef HOMURA_GEOMETRY_H_
#define  HOMURA_GEOMETRY_H_

#include <array>
#include <vector>
#include <ostream>
#include <cmath>
#include <algorithm>
#include <cassert>
#include "core/Common.h"

namespace Homura {
	template<typename T, unsigned Size>
	class Point;

	template<typename ElementType, unsigned Size>
	class Vector {
	private:
		std::array<ElementType, Size> _v;
	public:
		static const unsigned size = Size;    // Each class template instantiation has its own copy of any static data members.

		Vector() = default;

		Vector(const ElementType a) {
			for (unsigned i = 0; i < Size; i++) _v[i] = a;
		}

		Vector(const ElementType a, const ElementType b) {
			_v[0] = a;
			_v[1] = b;
		}

		Vector(const ElementType a, const ElementType b, const ElementType c) {
			_v[0] = a;
			_v[1] = b;
			_v[2] = c;
		}

		Vector(const ElementType a, const ElementType b, const ElementType c, const ElementType d) {
			_v[0] = a;
			_v[1] = b;
			_v[2] = c;
			_v[3] = d;
		}

		Vector(const ElementType *a) {
			for (unsigned i = 0; i < Size; i++) _v[i] = a[i];
		}

		Vector(const Point<ElementType, Size> &p) {
			for (unsigned i = 0; i < Size; i++) _v[i] = p[i];
		}

		//~Vector();


		inline ElementType &x() { return _v[0]; }
		inline const ElementType &x() const { return _v[0]; }

		inline ElementType &y() { return _v[1]; }
		inline const ElementType &y() const { return _v[1]; }

		inline ElementType &z() { return _v[2]; }
		inline const ElementType &z() const { return _v[2]; }

		inline ElementType &w() { return _v[3]; }
		inline const ElementType &w() const { return _v[3]; }

		float length() {
			float l = 0;
			for (unsigned i = 0; i < Size; i++) {
				l += _v[i] * _v[i];
			}
			return std::sqrt(l);
		}

		float squareLength() const {
			float l = 0;
			for (unsigned i = 0; i < Size; i++) {
				l += _v[i] * _v[i];
			}
			return l;
		}

		inline void normalize() {
			float len = 0.0f;
			for (unsigned i = 0; i < Size; i++) {
				len += _v[i] * _v[i];
			}
			len = std::sqrt(len);
			/// TODO: divide check
			for (unsigned i = 0; i < Size; i++) {
				_v[i] /= len;
			}
		}

		inline Vector normalized() const {
			Vector ret;
			float len = 0.0f;
			for (unsigned i = 0; i < Size; i++) {
				len += _v[i] * _v[i];
			}
			len = std::sqrt(len);
			/// TODO: divide check
			for (unsigned i = 0; i < Size; i++) {
				ret._v[i] = _v[i] / len;
			}
			return ret;
		}

		inline ElementType dot(Vector other) const {
			ElementType ret = _v[0] * other._v[0];
			for (unsigned i = 1; i < Size; i++) {
				ret += _v[i] * other._v[i];
			}
			return ret;
		}

		inline Vector cross(Vector other) const {
			static_assert(Size == 3, "Cross product operation only defined in three dimension space!");
			return Vector(
				y() * other.z() - z() * other.y(),
				z() * other.x() - x() * other.z(),
				x() * other.y() - y() * other.x()
			);
		}

		inline ElementType &operator[](unsigned i) {
			return _v[i];
		}

		inline const ElementType &operator[](unsigned i) const {
			return _v[i];
		}

		inline int maxDim() const {
			int max_idx = 0;
			Vector v;
			for (int i = 0; i < Size; i++)
				v[i] = std::abs(_v[i]);

			for (int i = 0; i < Size; i++) {
				if (v[i] > v[max_idx]) max_idx = i;
			}
			return max_idx;
		}

		inline ElementType max() const {
			ElementType m = _v[0];
			for (unsigned i = 0; i < Size; i++) {
				if (_v[i] > m)
					m = _v[i];
			}
			return m;
		}

		inline ElementType min() const {
			ElementType m = _v[0];
			for (unsigned i = 0; i < Size; i++) {
				if (_v[i] < m)
					m = _v[i];
			}
			return m;
		}

		inline Vector permute(int kx, int ky, int kz) const {
			return Vector(_v[kx], _v[ky], _v[kz]);
		}

		inline Vector operator-() const {
			Vector ret;
			for (unsigned i = 0; i < Size; i++) {
				ret._v[i] = -_v[i];
			}
			return ret;
		}

		inline Vector operator+(const ElementType &a) const {
			Vector ret;
			for (unsigned i = 0; i < Size; i++) {
				ret._v[i] = _v[i] + a;
			}
			return ret;
		}

		inline Vector operator-(const ElementType &a) const {
			Vector ret;
			for (unsigned i = 0; i < Size; i++) {
				ret._v[i] = _v[i] - a;
			}
			return ret;
		}

		inline Vector operator*(const ElementType &a) const {
			Vector ret;
			for (unsigned i = 0; i < Size; i++) {
				ret._v[i] = _v[i] * a;
			}
			return ret;
		}

		inline Vector operator/(const ElementType &a) const {
			assert(a != 0);
			Vector ret;
			for (unsigned i = 0; i < Size; i++) {
				ret._v[i] = _v[i] / a;
			}
			return ret;
		}

		inline Vector operator+(const Vector &other) const {
			Vector ret;
			for (unsigned i = 0; i < Size; i++) {
				ret._v[i] = _v[i] + other._v[i];
			}
			return ret;
		}

		inline Vector operator-(const Vector &other) const {
			Vector ret;
			for (unsigned i = 0; i < Size; i++) {
				ret._v[i] = _v[i] - other._v[i];
			}
			return ret;
		}

		inline Vector operator*(const Vector &other) const {
			Vector ret;
			for (unsigned i = 0; i < Size; i++) {
				ret._v[i] = _v[i] * other._v[i];
			}
			return ret;
		}

		inline Vector operator/(const Vector &other) const {
			Vector ret;
			for (unsigned i = 0; i < Size; i++) {
				//ASSERT(other._v[i] != 0);
				ret._v[i] = _v[i] / other._v[i];    // do I need to handle exception if divide zero?
			}
			return ret;
		}

		inline void operator+=(const Vector &other) {
			for (unsigned i = 0; i < Size; i++) {
				_v[i] += other._v[i];
			}
		}

		inline void operator-=(const Vector &other) {
			for (unsigned i = 0; i < Size; i++) {
				_v[i] -= other._v[i];
			}
		}

		inline void operator *=(const Vector &other) {
			for (unsigned i = 0; i < Size; i++)
				_v[i] *= other._v[i];
		}

		inline void operator /=(const Vector &other) {
			for (unsigned i = 0; i < Size; i++)
				_v[i] /= other._v[i];
		}

		inline void operator*=(const ElementType &a) {
			for (unsigned i = 0; i < Size; i++)
				_v[i] *= a;
		}

		inline void operator/=(const ElementType &a) {
			for (unsigned i = 0; i < Size; i++) {
				_v[i] /= a;
			}
		}

		friend std::ostream &operator<<(std::ostream &os, const Vector &v) {
			os << '(';
			for (unsigned i = 0; i < Size; i++) {
				os << v[i] << (i == Size - 1 ? ')' : ',');
			}
			return os;
		}
	};

	typedef Vector<double, 4> Vec4d;
	typedef Vector<double, 3> Vec3d;
	typedef Vector<double, 2> Vec2d;

	typedef Vector<float, 4> Vec4f;
	typedef Vector<float, 3> Vec3f;
	typedef Vector<float, 2> Vec2f;

	typedef Vector<int, 4> Vec4i;
	typedef Vector<int, 3> Vec3i;
	typedef Vector<int, 2> Vec2i;

	typedef Vector<unsigned, 2> Vec2u;

	template<typename ElementType, unsigned Size>
	std::vector<Vector<ElementType, Size>> extractVector(const std::vector<ElementType> &v) {
		if (v.size() % Size)
			std::cerr << "initialize Vector from std::vector fails! Size don't match." << std::endl;

		std::vector<Vector<ElementType, Size>> ret(v.size() / Size);
		for (int i = 0; i < ret.size(); i++) {
			int offset = i * Size;
			for (int j = 0; j < Size; j++)
				ret[i][j] = v[offset + j];
		}
		return ret;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename T, unsigned Size>
	class Point {
	private:
		std::array<T, Size> _p;
	public:
		static const unsigned size = Size;
		
		Point() = default;

		Point(T v) {
			for (int i = 0; i < Size; i++) _p[i] = v;
		}

		Point(T x, T y) {
			_p[0] = x, _p[1] = y;
		}

		Point(T x, T y, T z) {
			_p[0] = x, _p[1] = y, _p[2] = z;
		}

		Point(const Vector<T, Size> &v) {
			for (unsigned i = 0; i < Size; i++)
				_p[i] = v[i];
		}

		inline T &x() { return _p[0]; }
		inline const T &x() const { return _p[0]; }
		inline T &y() { return _p[1]; }
		inline const T &y() const { return _p[1]; }
		inline T &z() { return _p[2]; }
		inline const T &z() const { return _p[2]; }

		inline Point permute(int kx, int ky, int kz) const {
			return Point(_p[kx], _p[ky], _p[kz]);
		}

		inline T &operator[](unsigned i) { return _p[i]; }
		inline const T& operator[](unsigned i) const { return _p[i]; }

		inline Point operator*(const float s) const {
			Point ret;
			for (int i = 0; i < Size; i++)
				ret._p[i] = _p[i] * s;
			return ret;
		}

		inline Point operator+(const Point &other) const {
			Point ret;
			for (int i = 0; i < Size; i++) {
				ret._p[i] = _p[i] + other[i];
			}
			return ret;
		}

		inline void operator+=(const Point &other) {
			for (int i = 0; i < Size; i++) {
				_p[i] += other._p[i];
			}
		}

		inline Point operator+(const Vector<T, Size> &v) const {
			Point ret;
			for (int i = 0; i < Size; i++) ret[i] = _p[i] + v[i];
			return ret;
		}

		//inline Point operator-(const Point &other) {
		//	Point ret;
		//	for (int i = 0; i < Size; i++) {
		//		ret._p[i] = _p[i] - other._p[i];
		//	}
		//	return ret;
		//}

		inline Vector<T, Size> operator-(const Point &other) const {
			Vector<T, Size> ret;
			for (int i = 0; i < Size; i++) ret[i] = _p[i] - other[i];
			return ret;
		}

		inline Vector<T, Size> operator-(const Vector<T, Size> &v) const {
			Vector<T, Size> ret;
			for (int i = 0; i < Size; i++) ret[i] = _p[i] - v[i];
			return ret;
		}

		inline void operator-=(const Point &other) {
			for (int i = 0; i < Size; i++) {
				_p[i] -= other._p[i];
			}
		}

		friend std::ostream &operator<<(std::ostream &os, const Point &p) {
			os << '(';
			for (unsigned i = 0; i < Size; i++) {
				os << p[i] << (i == Size - 1 ? ')' : ',');
			}
			return os;
		}
	};

	typedef Point<int, 2> Point2i;
	typedef Point<int, 3> Point3i;
	typedef Point<float, 2> Point2f;
	typedef Point<float, 3> Point3f;

	inline float rad2deg(float rad) { return rad*180.f/3.1415926536f; }
	inline float deg2rad(float deg) { return deg*3.1415926536f/180.f; }

	inline float sphericalPhi(const Vec3f &v) {
		return std::acos(v.z());	// v should be normalized
	}

	inline float sphericalTheta(const Vec3f &v) {
		float p = std::atan2(v.y(), v.x());
		return (p < 0) ? (p + 2 * PI) : p;
	}
}

#endif // !HOMURA_GEOMETRY_H_