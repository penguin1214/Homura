#include <renderer/sensors/Perspective.h>
#include "renderer/Scene.h"
#include "renderer/primitives/TriangleMesh.h"
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
				if (type == "matte")
					_bxdfs[bxdf["name"].getString()] = std::make_shared<LambertReflection>(bxdf["R"].getVec3());
				else if (type == "specref")
					_bxdfs[bxdf["name"].getString()] = std::make_shared<FresnelSpecularReflection>(bxdf["R"].getVec3(), bxdf["eta"].getFloat());
				else if (type == "spectrans")
					_bxdfs[bxdf["name"].getString()] = std::make_shared<FresnelSpecularTransmission>(bxdf["T"].getVec3(), bxdf["eta"].getFloat());
				else if (type == "specular")
					_bxdfs[bxdf["name"].getString()] = std::make_shared<FresnelSpecular>(bxdf["R"].getVec3(), bxdf["T"].getVec3(), bxdf["eta"].getFloat());
			}
		}

		if (auto primitives = scene_document["primitives"]) {
            for (unsigned i = 0; i < primitives.size(); i++) {
				JsonObject primitive = primitives[i];
				if (primitive["type"].getString() == "obj")
					_shapes.push_back(std::make_shared<TriangleMesh>(primitive, _bxdfs));
			}
		}

		if (auto emitters = scene_document["emitters"]) {
			for (unsigned i = 0; i < emitters.size(); i++) {
				JsonObject emitter = emitters[i];
				auto type = emitter["type"].getString();
				if (type == "point")
					_emitters.push_back(std::unique_ptr<PointEmitter>(new PointEmitter(emitter)));
				else if (type == "directional")
					_emitters.push_back(std::unique_ptr<DirectionalEmitter>(new DirectionalEmitter(emitter)));
				else if (type == "diffuse_area")
					_emitters.push_back(std::unique_ptr<DiffuseAreaEmitter>(new DiffuseAreaEmitter(emitter)));
				//else if (emitter["type"].getString() == "environment")
					//_emitters.push_back(std::unique_ptr<EnvironmentEmitter>(new EnvironmentEmitter(emitter)));
				else
					;
			}
		}

		// Preprocess
		for (auto light : _emitters)
			light->preprocess();
	}

    bool Scene::intersect(Ray &r, IntersectInfo &isect_info) const {
		bool flag = false;
        for (auto &shape_ptr : _shapes) {
			if (shape_ptr->intersect(r, isect_info))
				flag = true;
        }

		return flag;
    }

	bool Scene::intersectP(const Ray &r) const {
		bool flag = false;
		for (auto &shape_ptr : _shapes) {
			if (shape_ptr->intersectP(r))
				flag = true;
		}
		return flag;
	}
}