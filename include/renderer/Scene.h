#ifndef HOMURA_SCENE_H_
#define HOMURA_SCENE_H_

#include "sensors/Orthographic.h"
#include "Ray.h"
#include "primitives/Primitive.h"
#include "core/io/JsonDocument.h"
#include <string>
#include <vector>
#include <memory>

namespace Homura {
	class Emitter;

	class Scene {
	public:
		std::shared_ptr<Sensor> _cam;
		std::vector<std::shared_ptr<Primitive>> _shapes;
		std::vector<std::shared_ptr<Emitter>> _emitters;
	public:
		//Scene(Sensor *cam);
		Scene(const JsonDocument &scene_document);

		bool intersect(Ray &r, IntersectInfo &isect_info) const;
		bool intersectP(const Ray &r) const;
};
}

#endif // !HOMURA_SCENE_H_
