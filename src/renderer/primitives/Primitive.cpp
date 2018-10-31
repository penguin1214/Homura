#include "renderer/primitives/Primitive.h"
#include "renderer/materials/Material.h"

namespace Homura {
	Primitive::Primitive(const JsonObject &json) {
		if (auto mat = json["material"]) {
			if (mat["type"].getString() == "matte") {
				_material = std::unique_ptr<MatteMaterial>(new MatteMaterial());
			}
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
}