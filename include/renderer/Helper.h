#ifndef HOMURA_HELPER_H_
#define	HOMURA_HELPER_H_

#include "core/math/Geometry.h"
#include "renderer/Scene.h"
#include "renderer/Ray.h"

namespace Homura {
	void draw_line(std::shared_ptr<Scene> sc, const Point3f &p1, const Point3f &p2, const float d = 0.05f, const Vec3f color = Vec3f(1, 0, 0));
}

#endif // !HOMURA_HELPER_H_
