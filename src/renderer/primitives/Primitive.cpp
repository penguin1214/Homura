#include "renderer/primitives/Primitive.h"
#include "renderer/materials/Material.h"
#include "renderer/shapes/Shape.h"
#include "renderer/shapes/TriangleMesh.h"

namespace Homura {
	Primitive::Primitive(
		const JsonObject &json, std::unordered_map<std::string, 
		std::shared_ptr<BxDF>> &bxdfs, std::unordered_map<std::string, MediumInterface> &medium_interfaces) {

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

		if (auto mi = json["medium_interface"]) {
			_medium_interface = MediumInterface(medium_interfaces[mi["name"].getString()]);
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
			if (_medium_interface.isTransition()) {
				info._medium_interface._inside = _medium_interface._inside;
				info._medium_interface._outside = _medium_interface._outside;
			}
			else
				info._medium_interface._inside = info._medium_interface._outside = r._medium;	// if the ray's origin is in a medium, and the hitted "surface" does not imply a medium transition, then the hitted point is in the same medium as the ray's origin.
			return true;
		}
		else
			return false;
	}

	/// TODO: does medium intersection count?
	std::shared_ptr<Primitive> Primitive::intersectP(const Ray &r) {
		if (_shape->intersectP(r, nullptr, nullptr))
			return getShared();
		else
			return nullptr;
	}
	TriangleMeshPrimitive::TriangleMeshPrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs, std::unordered_map<std::string, MediumInterface> &medium_interfaces)
	: Primitive(json, bsdfs, medium_interfaces) {}

	SpherePrimitive::SpherePrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs, std::unordered_map<std::string, MediumInterface> &medium_interfaces)
	: Primitive(json, bsdfs, medium_interfaces) {}

	QuadPrimitive::QuadPrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs, std::unordered_map<std::string, MediumInterface> &medium_interfaces)
	: Primitive(json, bsdfs, medium_interfaces) {}
}