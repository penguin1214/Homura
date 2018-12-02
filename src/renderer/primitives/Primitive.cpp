#include "renderer/primitives/Primitive.h"
#include "renderer/materials/Material.h"
#include "renderer/shapes/Shape.h"
#include "renderer/shapes/TriangleMesh.h"

namespace Homura {
	Primitive::Primitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bxdfs) {
		if (auto mat = json["material"]) {
			_material = std::unique_ptr<Material>(new Material(mat, bxdfs));
		}

		auto type = json["type"].getString();
		if (type == "obj")
			_shape = std::make_shared<TriangleMesh>(json["shape"]);
		else if (type == "sphere")
			_shape = std::make_shared<Sphere>(json["shape"]);
		else if (type == "quad")
			_shape = std::make_shared<Quad>(json["shape"]);
		else
			;

		if (auto emitter = json["emitter"]) {
			auto type = emitter["type"].getString();
			if (type == "point")
				_emitter = std::make_shared<PointEmitter>(emitter);
			else if (type == "directional")
				_emitter = std::make_shared<DirectionalEmitter>(emitter);
			else if (type == "diffuse_area")
				_emitter = std::make_shared<DiffuseAreaEmitter>(emitter, _shape);
		}
	}

	std::shared_ptr<Primitive> Primitive::getShared() {
		return this->shared_from_this();
	}

    void Primitive::computeScatteringFunction(IntersectInfo &isect) const {
        if (_material) {
            _material->computeScatteringFunction(isect);
        }
    }

	bool Primitive::intersect(const Ray &r, IntersectInfo &info) {
		if (_shape->intersect(r, nullptr, &info)) {
			info._primitive = getShared();
			return true;
		}
		else
			return false;
	}

	std::shared_ptr<Primitive> Primitive::intersectP(const Ray &r) {
		if (_shape->intersectP(r, nullptr, nullptr))
			return getShared();
		else
			return nullptr;
	}
	TriangleMeshPrimitive::TriangleMeshPrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs)
	: Primitive(json, bsdfs) {}

	SpherePrimitive::SpherePrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs)
	: Primitive(json, bsdfs) {}

	QuadPrimitive::QuadPrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs)
	: Primitive(json, bsdfs) {}
}