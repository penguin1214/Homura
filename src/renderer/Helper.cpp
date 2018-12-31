#include "renderer/Helper.h"

namespace Homura {
	void draw_line(std::shared_ptr<Scene> sc, const Point3f &p1, const Point3f &p2, const float d, const Vec3f color) {
		Vec3f v(p2 - p1);
		float dist = v.length();
		Ray ray(p1, v.normalized(), 0.f, dist);
		int n = dist / d;
		for (int i = 0; i < n; i++) {
			Point3f p = ray._o + ray._d*i*d;
			Point2f p_raster = sc->_cam->pWorldToRaster(p);
			sc->_cam->_film->addSample(p_raster, color, 1.f);
		}
	}
}