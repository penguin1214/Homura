#ifndef HOMURA_RAY_H_
#define HOMURA_RAY_H_

#include "../core/math/Geometry.h"

namespace Homura {
	class Medium;
	class Ray {
	public:
    	Point3f _o;
		Vec3f _d;
        float _time;
		//mutable float _tMin, _tMax;	/// TODO: init _tMin & _tMax to valid value.
		mutable float _tmax;	// can be changed even if const
		std::shared_ptr<Medium> _medium;	// medium the ray's origin is in /// TODO

		Ray();
        //Ray(Vec3f o, Vec3f d, float t, float tMin, float tMax);
		Ray(const Point3f &o, const Vec3f &d, float t = 0.0f, float tMax = INFINITY, std::shared_ptr<Medium> med = nullptr);

        void setPrimary(bool v) { _is_primary = v; }
        bool isPrimary() { return _is_primary; }

	private:
		bool _is_primary;
	};

    // TODO
    class RayDifferential : public Ray {};
}

#endif // !HOMURA_RAY_H_
