/**
 * Created by Jinglei Yang on 3/12/18.
 * jinglei.yang.96@gmail.com
 * Copyright © 2018 Jinglei Yang. All rights reserved.
 */
#include "renderer/Ray.h"

namespace Homura {
    Ray::Ray() : _time(0.0f), _tmax(INFINITY), _is_primary(false), _medium(nullptr) {}
    //Ray::Ray(Vec3f o, Vec3f d, float t = 0.0f, float tMin = 0.0f, float tMax = INFINITY)
    //: _o(o), _d(d), _t(t), _tMin(tMin), _tMax(tMax) {}
	Ray::Ray(const Point3f &o, const Vec3f &d, float t, float tMax, std::shared_ptr<Medium> med)
    : _o(o), _d(d), _time(t), _tmax(tMax), _is_primary(false), _medium(med) {}
}
