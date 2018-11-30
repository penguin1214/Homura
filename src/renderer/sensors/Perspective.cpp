/**
 * Created by Jinglei Yang on 8/18/18.
 * jinglei.yang.96@gmail.com
 * Copyright © 2018 Jinglei Yang. All rights reserved.
 */

#include "renderer/sensors/Perspective.h"

namespace Homura {
    PerspectiveSensor::PerspectiveSensor(const Homura::JsonObject json)
    : ProjectiveSensor(json) {
		json.getField("fov", _vfov);	/// for vertical window_size, TODO: degree to radius
        _cam2screen = Mat4f::perspective(_vfov);
        _raster2cam = _raster2screen * Mat4f::inverse(_cam2screen);
    }

    float PerspectiveSensor::generatePrimaryRay(const PixelSample &sample, Ray &r) const {
        Point3f pSample(sample._p_film.x(), sample._p_film.y(), 0);
        Point3f p_cam = pSample * _raster2cam;
		r._o = Point3f(0.0f) * _cam2world;
        r._d = Vec3f(Point3f(p_cam) * _cam2world).normalized();

        return 1.0f;
    }
}