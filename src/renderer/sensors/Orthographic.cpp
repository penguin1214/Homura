/**
 * Created by Jinglei Yang on 3/13/18.
 * jinglei.yang.96@gmail.com
 * Copyright © 2018 Jinglei Yang. All rights reserved.
 */
#include "renderer/sensors/Orthographic.h"

namespace Homura {
    OrthographicSensor::OrthographicSensor(const Mat4f &ctw, Film *film, Bound3f screen_window)
            : ProjectiveSensor(ctw, Mat4f::orthographic(0,1/*TODO*/), film, screen_window) {
        // TODO: compute differential changes
        _dxSensor = Vec3f(1, 0, 0)*_raster2cam;
        _dySensor = Vec3f(0, 1, 0)*_raster2cam;
    }

	OrthographicSensor::OrthographicSensor(const JsonObject json)
	: ProjectiveSensor(json) {
		// TODO: dxSensor, dySensor, differential changes
		_cam2screen = Mat4f::orthographic(0, 1);
		_raster2cam = _raster2screen * Mat4f::inverse(_cam2screen);
	}

	//OrthographicSensor::OrthographicSensor(const char *json) {
	//	Buffer *buffer = new Buffer(100, 200);
	//	Film film(100, 200, buffer);
	//	Mat4f cam2world;
	//	// compute cam2world matrix according to lookat matrix
	//	cam2world = cam2world * cam2world.translate(Vec3f(0, 0, 1));
	//	cam2world = cam2world * cam2world.rotateXYZ(Vec3f(0, 0, 0));
	//	Bound2f screen_window(Point3f(-1, -1, 0), Point3f(1, 1, 0));
	//	ProjectiveSensor::ProjectiveSensor(cam2world, Mat4f::orthographic(0, 1/*TODO*/), screen_window, &film);
	//}

    float OrthographicSensor::generatePrimaryRay(const PixelSample &sample, Ray &r) const {
        Point3f pSample(sample._p_film.x(), sample._p_film.y(), 0);
		//std::cout << pSample << std::endl;
		//std::cout << _raster2screen * pSample << std::endl;
        Point3f p_cam = pSample*_raster2cam;
		//std::cout << p_cam << std::endl;
        r._o = p_cam*_cam2world;
        r._d = (Vec3f(0,0,-1)*_cam2world).normalized();    // ortho, right-hand coord

        return 1.0f;
    }
}
