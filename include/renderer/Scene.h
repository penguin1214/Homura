#ifndef HOMURA_SCENE_H_
#define HOMURA_SCENE_H_

#include "sensors/Orthographic.h"
#include "Ray.h"
#include "renderer/bxdfs/BxDF.h"
#include "primitives/Primitive.h"
#include "core/io/JsonDocument.h"
#include "accelerators/accelerator.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace Homura {
	class Emitter;

	class Scene {
	public:
		std::shared_ptr<ProjectiveSensor> _cam;
		std::unordered_map<std::string, std::shared_ptr<BxDF>> _bxdfs;
		std::unordered_map<std::string, std::shared_ptr<Medium>> _media;
		std::unordered_map<std::string, MediumInterface> _medium_interfaces;
		std::vector<std::shared_ptr<Primitive>> _primitives;
		std::vector<std::shared_ptr<Primitive>> _emitters;
		std::unique_ptr<BVHAccelerator> _bvh;
	public:
		//Scene(Sensor *cam);
		Scene(const JsonDocument &scene_document);

		bool intersect(Ray &r, IntersectInfo &isect_info) const;
		bool intersectP(const Ray &r) const;
		bool Scene::intersectP(const Ray &r, std::shared_ptr<Emitter> evalemitter) const;
		bool intersectTr() const;	/// TODO
};
}

#endif // !HOMURA_SCENE_H_
