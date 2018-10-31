#ifndef HOMURA_BOX_H_
#define HOMURA_BOX_H_

#include "../../core/math/Geometry.h"
#include "Ray.h"

namespace Homura {
	class Box {
	public:
		Point3f _min, _max;
	public:
		Box() {}
		~Box() {}

		bool intersect(Ray &r, float &t0, float &t1) {
			float tmin = 0.0; float tmax = r._tmax;
			for (unsigned i = 0; i < 3; i++) {
				float inv_d = 1.0f / r._d[i];
				float tmp_t0 = (_min.x - r._o[i]) * inv_d;
				float tmp_t1 = (_max.x - r._o[i]) * inv_d;
				if (tmp_t0 > tmp_t1) std::swap(tmp_t0, tmp_t1);

				if (tmp_t0 > tmin) tmin = tmp_t0;
				if (tmp_t1 < tmax) tmax = tmp_t1;
			}
			if (tmin > tmax) return false;

			t0 = tmin; t1 = tmax;
			return true;
		}

	private:
	};
}

#endif // HOMURA_BOX_H_
