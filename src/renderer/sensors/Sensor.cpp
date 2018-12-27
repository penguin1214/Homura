#include "renderer/sensors/Sensor.h"

namespace Homura {
    //Film::Film(int w, int h, Buffer *cbuffer) : _width(w), _height(h), _cbuffer(cbuffer) {}
	Film::Film(Vec2u res) : _width(res[0]), _height(res[1]), _cbuffer(new Buffer(res[0], res[1])) {}

	void Film::addSample(const Point2f &pfilm, Vec3f L, float ray_weight) {
		int idx = int(pfilm.y()-0.5f)*_width + int(pfilm.x()-0.5f);
		_cbuffer->addSample(idx, L*ray_weight);
	}

	void Film::addSplat(const Point2f &pfilm, Vec3f L) {
		/// TODO:  splat
		int idx = int(pfilm.y()-0.5f)*_width + int(pfilm.x()-0.5f);
		_cbuffer->addSample(idx, L);
	}

	Vec3f Film::tonemap(Vec3f c) const {
		Vec3f x = c - 0.004f;
		for (unsigned i = 0; i < 3; i++)
			x[i] = std::max(x[i], 0.f);
		return (x*(x*6.2f + 0.5f)) / (x*(x*6.2f + 1.7f) + 0.06f);
	}

	void Film::writeColorBuffer(char *fn) {
		for (auto &v : _cbuffer->_data) {
			v = tonemap(v)*255;
			v[0] = clamp(v[0], 0.f, 255.f);
			v[1] = clamp(v[1], 0.f, 255.f);
			v[2] = clamp(v[2], 0.f, 255.f);
		}
		PPMHandler::writePPM(fn, _cbuffer->_data, _width, _height);
	}

    Sensor::Sensor(const Mat4f &ctw, Film *film) : _cam2world(ctw), _film(film) {}

	Sensor::Sensor(const JsonObject json) {
		Vec2u res;
		if (json.getField("resolution", res))
			_film = std::unique_ptr<Film>(new Film(res));
		_aspect_ratio = (float)res.x() / (float)res.y();
		// convert lookAt matrix to cam2world matrix
		Mat4f cam2World;
		if (json.getField("lookat", cam2World))
			_cam2world = cam2World;
		_p = Point3f(0, 0, 0)*_cam2world;
	}

	Point3f Sensor::p() const {
		return _p;
	}

    Sensor::~Sensor() {}

    /*
     * `screen_window` is same as canvas, defining the area visible on image plane.
     * By default, its value is [-1, 1] on both axes.
     */
	ProjectiveSensor::ProjectiveSensor(const Mat4f &ctw, const Mat4f &cts, Film *film, Bound3f screen_window)
    : Sensor(ctw, film), _cam2screen(cts) {
        _screen2raster =
                Mat4f::scale(Vec3f(_film->width(), _film->height(), 1)) * // NDC -> raster(final image)
                Mat4f::scale(Vec3f(1.0f/(screen_window._max.x()-screen_window._min.x()), -1.0f/(screen_window._max.y()-screen_window._min.y()), 1.0f)) *    // view -> NDC
                Mat4f::translate(Vec3f(-screen_window._min.x(), -screen_window._max.y(), -1));	// after translation, image plane lies on z=0, need to verify.

        _raster2screen = Mat4f::inverse(_screen2raster);
        _raster2cam = _raster2screen * Mat4f::inverse(cts);
    }

	ProjectiveSensor::ProjectiveSensor(JsonObject json)
	: Sensor(json) {
	    /// TODO: screen window aspect ratio should be same as film?
		json.getField("screen_window", _screen_window);
		Bound3f screen_window(Point3f(-_aspect_ratio*0.5f*_screen_window, -0.5f*_screen_window, 0), Point3f(_aspect_ratio*0.5f*_screen_window, 0.5f*_screen_window, 0));
		_raster_bound = Bound2f(Point2f(0, 0), Point2f(_film->width(), _film->height()));

		_screen2raster =
			Mat4f::translate(Vec3f(-screen_window._min.x(), -screen_window._max.y(), -1)) * // NDC -> raster(final image)
			Mat4f::scale(Vec3f(1.0f / (screen_window._max.x() - screen_window._min.x()), -1.0f / (screen_window._max.y() - screen_window._min.y()), 1.0f)) *
			Mat4f::scale(Vec3f(_film->width(), _film->height(), 1));	// after translation, image plane lies on z=0.


		_raster2screen = Mat4f::inverse(_screen2raster);
	}

	Vec3f ProjectiveSensor::We(const Ray &r, Point2f *p_raster) const {
		std::cerr << "NOT_IMPLEMENT_ERROR::WE()_ONLY_IMPLEMENTED_FOR_PERSPECTOVE_SENSOR" << std::endl;
		return Vec3f(0.f);
	}

	Point2f ProjectiveSensor::pWorldToRaster(const Point3f &p_w) const {
		Point3f praster = p_w * Mat4f::inverse(_cam2world)*Mat4f::inverse(_raster2cam);
		Point2f p_raster;
		p_raster.x() = praster.x();
		p_raster.y() = praster.y();
		return p_raster;
	}
}
