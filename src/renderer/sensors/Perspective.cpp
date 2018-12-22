#include "renderer/sensors/Perspective.h"
#include "renderer/IntersectInfo.h"
#include "renderer/emitters/Emitter.h"

namespace Homura {
    PerspectiveSensor::PerspectiveSensor(const Homura::JsonObject json)	/* checked. */
    : ProjectiveSensor(json) {
		json.getField("fov", _vfov);	/// for vertical window_size, TODO: degree to radius
        _cam2screen = Mat4f::perspective(_vfov);
        _raster2cam = _raster2screen * Mat4f::inverse(_cam2screen);

		// compute area of filmed area
		Point3f p_cam_max = Point3f(_film->width(), _film->height(), -1)*_raster2cam;
		Point3f p_cam_min = Point3f(0, 0, -1)*_raster2cam;
		p_cam_max /= p_cam_max.z();	/// TODO: this means map to focal plane?
		p_cam_min /= p_cam_min.z();
		_area = std::abs((p_cam_max.x() - p_cam_min.x()) * (p_cam_max.y() - p_cam_min.y()));	/// TODO: why not use arae on raster plane? (because rays are sampled in terms of raster plane)
    }

    float PerspectiveSensor::generatePrimaryRay(const SensorSample &sample, Ray &r) const {
        Point3f pSample(sample._p_film.x(), sample._p_film.y(), 0);
        Point3f p_cam = pSample * _raster2cam;
		r._o = Point3f(0.0f) * _cam2world;
        r._d = Vec3f(Point3f(p_cam) * _cam2world).normalized();

        return 1.0f;
    }

	Vec3f PerspectiveSensor::We(const Ray &r, Point2f *praster) const {	/* checked. */
		// 1. check if same hemisphere with viewing direction
		float cos_theta = r._d.dot(Vec3f(0, 0, -1)*_cam2world);	/// TODO: (0,0,1)?
		if (cos_theta <= 0.f)
			return Vec3f(0.f);
		// 2. check if point in film region
		Point3f p_focus = r._o + (1.f / cos_theta)*r._d;	/// TODO: only consider pinhole now, apeture size to do.
		Point3f p_raster = p_focus * Mat4f::inverse(_cam2world)*Mat4f::inverse(_raster2cam);	/// TODO: check

		if (praster) *praster = Point2f(p_raster.x(), p_raster.y());

		/// TOOD: film bound ???

		float lens_area = 1.f;	/// TODO: realistic camera (finite lens area)

		float cos2 = cos_theta * cos_theta;
		return Vec3f(1.f / (_area*lens_area*cos2*cos2));
	}

	void PerspectiveSensor::Pdf_We(const Ray &r, float &pdf_pos, float &pdf_dir) const {	/* checked. */
		// 1. check if same hemisphere with viewing direction
		float cos_theta = r._d.dot(Vec3f(0, 0, -1)*_cam2world);
		if (cos_theta <= 0.f) {
			pdf_pos = 0.f;
			pdf_dir = 0.f;
		}

		// 2. check if point in film region
		Point3f p_focus = r._o + (1.f / cos_theta)*r._d;	/// TODO: only consider pinhole now, apeture size to do.
		Point3f p_raster = p_focus * Mat4f::inverse(_cam2world)*Mat4f::inverse(_raster2cam);

		/// TOOD: film bound

		float lens_area = 1.f;	/// TODO: realistic camera

		pdf_pos = 1.f / lens_area;	/// TODO: why not 1/A????
		pdf_dir = 1.f / (_area*lens_area*cos_theta*cos_theta*cos_theta);
	}

	/* Sample an incident intersection on camera. */
	Vec3f PerspectiveSensor::sample_Wi(const IntersectInfo &isect_info, const Point2f &u, Vec3f &wi, float &pdf, Point2f *p_raster, VisibilityTester *vt) const {
		// for now lens radius is 0 and thus no need to sample.
		/// TODO: sample on lens
		Point3f sample_lens_w = Point3f(0, 0, 0.f)*_cam2world;
		IntersectInfo lens_isect(sample_lens_w);
		lens_isect._normal = Vec3f(0, 0, -1)*(_cam2world.forNormal());
		*vt = VisibilityTester(isect_info, lens_isect);
		wi = (lens_isect._p - isect_info._p).normalized();
		pdf = 1.f;	/// TODO

		return We(lens_isect.spawnRay(-wi), p_raster);
	}
}