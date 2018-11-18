#include "renderer/primitives/Primitive.h"
#include "renderer/materials/Material.h"

namespace Homura {
	Primitive::Primitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bxdfs) {
		if (auto mat = json["material"]) {
			/*if (mat["type"].getString() == "matte") {
				_material = std::unique_ptr<MatteMaterial>(new MatteMaterial(mat));
			}
			else if (mat["type"].getString() == "mirror") {
				_material = std::unique_ptr<Mirror>(new Mirror());
			}*/
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
}