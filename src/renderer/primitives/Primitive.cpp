#include "renderer/primitives/Primitive.h"
#include "renderer/materials/Material.h"
#include "renderer/shapes/Shape.h"

namespace Homura {
	Primitive::Primitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bxdfs) {
		if (auto mat = json["material"]) {
			_material = std::unique_ptr<Material>(new Material(mat, bxdfs));
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

	ShapePrimitive::ShapePrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs)
	: Primitive(json, bsdfs) {
		auto type = json["type"].getString();
		if (type == "sphere")
			_shape = std::unique_ptr<Sphere>(new Sphere(json["shape"]));
		else if (type == "quad")
			_shape = std::unique_ptr<Quad>(new Quad(json["shape"]));
		else
			;

	}

	bool ShapePrimitive::intersect(const Ray &r, IntersectInfo &info) {
		if (_shape->intersect(r, nullptr, &info)) {
			info._primitive = getShared();
			return true;
		}
		else
			return false;
	}

	bool ShapePrimitive::intersectP(const Ray &r) const {
		return _shape->intersectP(r, nullptr, nullptr);
	}

	SpherePrimitive::SpherePrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs)
	: ShapePrimitive(json, bsdfs) {}

	QuadPrimitive::QuadPrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs)
	: ShapePrimitive(json, bsdfs) {}
}