#ifndef HOMURA_SENSOR_H_
#define HOMURA_SENSOR_H_

#include "core/math/Geometry.h"
#include "core/math/Mat4f.h"
#include "core/math/Bound.h"
#include "core/io/PpmHandler.h"
#include "core/io/JsonObject.h"
#include "renderer/Ray.h"
#include "renderer/Buffer.h"
#include <memory>

namespace Homura {
	struct PixelSample {
		Vec2f _p_film;
		Vec2f _p_lens;
		float time;
	};

    class Film {
		int _width;
		int _height;
		Point3f _left_bottom_corner;
        Point3f _left_up_corner;
        Buffer *_cbuffer;

	public:
        //Film(int w, int h, Buffer *cbuffer);
		Film(Vec2u res);
		void addSample(const Vec2f &pfilm, Vec3f L, float ray_weight);
		void writeColorBuffer(char *fn);
		int width() { return _width; }
		int height() { return _height; }

	private:
		Vec3f tonemap(Vec3f c) const;
	};

	class Sensor {
	public:
        std::unique_ptr<Film> _film;
    protected:
        Mat4f _cam2world;   // TODO: Animated Transform
        float _aspect_ratio;

        Sensor(const Mat4f &ctw, Film *film);
		Sensor(JsonObject json);
        ~Sensor();

	public:
        virtual float generatePrimaryRay(const PixelSample &sample, Ray &r) const = 0;
	};

    class ProjectiveSensor: public Sensor {
	/*
	* Since the use of screen_window is not understood for now,
	* set screen_window to (Point3f(-1,-1,0), Point3f(1,1,0)) as default.
	*/
    public:
        ProjectiveSensor(const Mat4f &ctw, const Mat4f &cts, Film *film, Bound2f screen_window);
		ProjectiveSensor(const JsonObject json);

    protected:
		/* Note that _cam2screen and _raster2cam rely on type of sensor,
		 * thus when initializing from json, these two matrices are initialized by derived classes.
		*/
		float _screen_window;
        Mat4f _cam2screen;
        Mat4f _raster2cam;
        Mat4f _screen2raster;
        Mat4f _raster2screen;
    };

}

#endif	//!HOMURA_SENSOR_H_
