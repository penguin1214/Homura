#include <renderer/sensors/Perspective.h>
#include "renderer/Scene.h"
#include "renderer/shapes/TriangleMesh.h"
#include "renderer/emitters/Emitter.h"
#include "renderer/bxdfs/Lambert.h"
#include "renderer/bxdfs/Fresnel.h"

namespace Homura {
    //Scene::Scene(Sensor *cam) :_cam(cam) {}

	Scene::Scene(const JsonDocument &scene_document) {
		/// TODO: cast string to class type!

		if (auto sensor = scene_document["sensor"]) {
			if (sensor["type"].getString() == "ortho") {
				_cam = std::make_shared<OrthographicSensor>(sensor);
			}
			else if (sensor["type"].getString() == "perspective") {
				_cam = std::make_shared<PerspectiveSensor>(sensor);
			}
			else
			    std::cerr << "ERROR::SCENE::SENSOR_CREATE_ERROR" << std::endl;
		}

		if (auto bxdfs = scene_document["bxdfs"]) {
			for (unsigned i = 0; i < bxdfs.size(); i++) {
				JsonObject bxdf = bxdfs[i];
				std::string type = bxdf["type"].getString();
				std::string name = bxdf["name"].getString();
				if (type == "matte")
					_bxdfs[name] = std::make_shared<LambertReflection>(bxdf["R"].getVec3(), name);
				else if (type == "specref")
					_bxdfs[name] = std::make_shared<FresnelSpecularReflection>(bxdf["R"].getVec3(), bxdf["eta"].getFloat(), name);
				else if (type == "spectrans")
					_bxdfs[name] = std::make_shared<FresnelSpecularTransmission>(bxdf["T"].getVec3(), bxdf["eta"].getFloat(), name);
				else if (type == "specular")
					_bxdfs[name] = std::make_shared<FresnelSpecular>(bxdf["R"].getVec3(), bxdf["T"].getVec3(), bxdf["eta"].getFloat(), name);
			}
		}

		if (auto primitives = scene_document["primitives"]) {
            for (unsigned i = 0; i < primitives.size(); i++) {
				JsonObject primitive = primitives[i];
				auto type = primitive["type"].getString();
				if (type == "obj")
					_primitives.push_back(std::make_shared<TriangleMeshPrimitive>(primitive, _bxdfs));
				else if (type == "sphere" || type == "quad")
					_primitives.push_back(std::make_shared<Primitive>(primitive, _bxdfs));
			}
		}

		//if (auto emitters = scene_document["emitters"]) {
		//	for (unsigned i = 0; i < emitters.size(); i++) {
		//		JsonObject emitter = emitters[i];
		//		auto type = emitter["type"].getString();
		//		if (type == "point")
		//			_emitters.push_back(std::unique_ptr<PointEmitter>(new PointEmitter(emitter)));
		//		else if (type == "directional")
		//			_emitters.push_back(std::unique_ptr<DirectionalEmitter>(new DirectionalEmitter(emitter)));
		//		else if (type == "diffuse_area")
		//			_emitters.push_back(std::unique_ptr<DiffuseAreaEmitter>(new DiffuseAreaEmitter(emitter)));
		//		else
		//			;
		//	}
		//}
		for (const auto &prim : _primitives) {
			if (prim->isEmitter())
				_emitters.push_back(prim);
		}

		// Preprocess
		for (auto light : _emitters)
			light->getEmitter()->preprocess();
	}

    bool Scene::intersect(Ray &r, IntersectInfo &isect_info) const {
		bool flag = false;
        for (auto &shape_ptr : _primitives) {
			if (shape_ptr->intersect(r, isect_info))
				flag = true;
        }

		return flag;
    }

	bool Scene::intersectP(const Ray &r) const {
		bool flag = false;
		for (auto &shape_ptr : _primitives) {
			std::shared_ptr<Primitive> hitprim = shape_ptr->intersectP(r);
			if (hitprim != nullptr && (hitprim->getEmitter() == nullptr) )
				flag = true;
		}
		return flag;
	}
}