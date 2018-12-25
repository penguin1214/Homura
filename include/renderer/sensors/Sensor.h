#ifndef HOMURA_SENSOR_H_
#define HOMURA_SENSOR_H_

#include "core/math/Geometry.h"
#include "core/math/Mat4f.h"
#include "core/io/PpmHandler.h"
#include "core/io/JsonObject.h"
#include "renderer/Ray.h"
#include "renderer/Buffer.h"
#include "renderer/shapes/Bound.h"
#include "renderer/IntersectInfo.h"
#include <memory>

namespace Homura {
	class VisibilityTester;

	struct SensorSample {
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
		void addSplat(const Vec2f &pfilm, Vec3f L);
		void writeColorBuffer(char *fn);
		int width() { return _width; }
		int height() { return _height; }
		Vec3f fetch(int x, int y) const { return tonemap(_cbuffer->_data[y*_width+x])*255; }

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
        virtual float generatePrimaryRay(const SensorSample &sample, Ray &r) const = 0;
	};

    class ProjectiveSensor: public Sensor {
	/*
	* Since the use of screen_window is not understood for now,
	* set screen_window to (Point3f(-1,-1,0), Point3f(1,1,0)) as default.
	*/
    public:
        ProjectiveSensor(const Mat4f &ctw, const Mat4f &cts, Film *film, Bound3f screen_window);
		ProjectiveSensor(const JsonObject json);

		virtual Vec3f We(const Ray &r, Point2f *p_raster = nullptr) const;
		virtual void Pdf_We(const Ray &r, float &pdf_pos, float &pdf_dir) const {}
		virtual Vec3f sample_Wi(const IntersectInfo &isect_info, const Point2f &u, Vec3f &wi, float &pdf, Point2f *p_raster, VisibilityTester *vt) const { return Vec3f(0.f); }

    protected:
		/* Note that _cam2screen and _raster2cam rely on type of sensor,
		 * thus when initializing from json, these two matrices are initialized by derived classes.
		*/
		float _lens_radius = 0.f;
		float _screen_window;
		Bound2f _raster_bound;
        Mat4f _cam2screen;
        Mat4f _raster2cam;
        Mat4f _screen2raster;
        Mat4f _raster2screen;
    };

}

#endif	//!HOMURA_SENSOR_H_
