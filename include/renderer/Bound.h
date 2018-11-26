#ifndef HOMURA_BOUND_H
#define HOMURA_BOUND_H

#include "core/math/Geometry.h"
#include "renderer/Ray.h"

namespace Homura {
	class Bound2f {
	public:
		Point2f _min, _max;

		Bound2f(Point2f min, Point2f max) : _min(min), _max(max) {}
	};

	////////////////////////////////////////////////////////

	class Bound3f {
	public:
		Bound3f() {
			float minNum = std::numeric_limits<float>::lowest();
			float maxNum = std::numeric_limits<float>::max();
			_max = Point3f(maxNum, maxNum, maxNum);
			_min = Point3f(minNum, minNum, minNum);
		}

		Bound3f(const Point3f &min, const Point3f &max) : _min(min), _max(max) {}

		inline bool operator==(const Bound3f &b) const { return (b._min == _min) && (b._max == _max); }

		inline bool intersectP(const Ray &ray, float &hitt0, float &hitt1) const {
			float tmin = 0.0f; float tmax = r._tmax;
			for (unsigned i = 0; i < 3; i++) {
				float inv_d = 1.0f / r._d[i];
				float tmp_t0 = (_min.x - r._o[i]) * inv_d;
				float tmp_t1 = (_max.x - r._o[i]) * inv_d;
				if (tmp_t0 > tmp_t1) std::swap(tmp_t0, tmp_t1);

				if (tmp_t0 > tmin) tmin = tmp_t0;
				if (tmp_t1 < tmax) tmax = tmp_t1;
			}
			if (tmin > tmax) return false;

			hitt0 = tmin; hitt1 = tmax;
			/// TODO: update ray information?
			return true;
		}

		Point3f _min, _max;
	};
}

#endif //HOMURA_BOUND_H
