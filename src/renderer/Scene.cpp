#include <renderer/sensors/Perspective.h>
#include "renderer/Scene.h"
#include "renderer/shapes/TriangleMesh.h"
#include "renderer/emitters/Emitter.h"
#include "renderer/bxdfs/Lambert.h"
#include "renderer/bxdfs/Fresnel.h"
#include "renderer/bxdfs/Microfacet.h"
#include "renderer/medium/Medium.h"

#define USE_BVH 1

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
				else if (type == "microfacet")
					_bxdfs[name] = std::make_shared<MicrofacetReflection>(bxdf);
			}
		}

		if (auto phases = scene_document["phase_function"]) {
			for (unsigned i = 0; i < phases.size(); i++) {
				JsonObject phase = phases[i];
				std::string type = phase["type"].getString();
				std::string name = phase["name"].getString();
				if (type == "HG")
					_phase_funcs[name] = std::make_shared<HenyeyGreenstein>(phase);
			}
		}

		if (auto media = scene_document["medium"]) {
			for (unsigned i = 0; i < media.size(); i++) {
				JsonObject medium = media[i];
				std::string name = medium["name"].getString();
				std::string type = medium["type"].getString();
				if (type == "homogeneous")
					_media[name] = std::make_shared<HomogeneousMedium>
						(medium["sigma_a"].getVec3(), medium["sigma_s"].getVec3(), _phase_funcs[medium["phase"].getString()]);
			}
		}

		if (auto medium_interfaces = scene_document["medium_interface"]) {
			for (unsigned i = 0; i < medium_interfaces.size(); i++) {
				JsonObject mi = medium_interfaces[i];
				std::string name = mi["name"].getString();

				std::string inside_key = mi["inside"].getString();
				std::shared_ptr<Medium> inside =
					(_media.find(inside_key) != _media.end()) ? (_media[inside_key]) : (nullptr);
				std::string outside_key = mi["outside"].getString();
				std::shared_ptr<Medium> outside =
					(_media.find(outside_key) != _media.end()) ? (_media[outside_key]) : (nullptr);

				_medium_interfaces[name] = MediumInterface(inside, outside);
			}
		}

		if (auto primitives = scene_document["primitives"]) {
            for (unsigned i = 0; i < primitives.size(); i++) {
				JsonObject primitive = primitives[i];
				auto type = primitive["type"].getString();
				if (type == "obj")
					_primitives.push_back(std::make_shared<TriangleMeshPrimitive>(primitive, _bxdfs, _medium_interfaces));
				else if (type == "sphere" || type == "quad")
					_primitives.push_back(std::make_shared<Primitive>(primitive, _bxdfs, _medium_interfaces));
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

#if USE_BVH
		// build bvh
		_bvh.reset(new BVHAccelerator(_primitives));
#endif
	}

    bool Scene::intersect(Ray &r, IntersectInfo &isect_info) const {
#if USE_BVH
		return _bvh->intersect(r, isect_info);
#else
		bool flag = false;
        for (auto &shape_ptr : _primitives) {
			if (shape_ptr->intersect(r, isect_info))
				flag = true;
        }

		return flag;
#endif
    }

	bool Scene::intersectP(const Ray &r) const {
#if USE_BVH
		return _bvh->intersectP(r);
#else
		bool flag = false;
		for (auto &shape_ptr : _primitives) {
			std::shared_ptr<Primitive> hitprim = shape_ptr->intersectP(r);	/// TODO: don't need to check emitter?
			if (hitprim != nullptr && (hitprim->getEmitter() == nullptr) )
				flag = true;
		}
		return flag;
#endif
	}

	bool Scene::intersectP(const Ray &r, std::shared_ptr<Emitter> evalemitter) const {
#if USE_BVH
		return _bvh->intersectP(r, evalemitter);
#else
		bool flag = false;
		for (auto &shape_ptr : _primitives) {
			std::shared_ptr<Primitive> hitprim = shape_ptr->intersectP(r);
			if (hitprim != nullptr && (hitprim->getEmitter() != evalemitter))
				flag = true;
		}
		return flag;
#endif
	}
}