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

	SpherePrimitive::SpherePrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs)
	: Primitive(json, bsdfs) {
		_shape = std::unique_ptr<Sphere>(new Sphere(json["shape"]));
	}

	bool SpherePrimitive::intersect(const Ray &r, IntersectInfo &info) {
		if (_shape->intersect(r, nullptr, &info)) {
			info._primitive = getShared();
			return true;
		}
		else
			return false;
	}

	bool SpherePrimitive::intersectP(const Ray &r) const {
		return _shape->intersectP(r, nullptr, nullptr);
	}
}