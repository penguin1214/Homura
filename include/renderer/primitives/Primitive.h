#ifndef HOMURA_PRIMITIVE_H_
#define HOMURA_PRIMITIVE_H_

#include "renderer/Ray.h"
#include "renderer/IntersectInfo.h"
#include "renderer/materials/Material.h"
#include "core/io/JsonObject.h"
#include <memory>

namespace Homura {
	class Primitive : public std::enable_shared_from_this<Primitive> {
	public:
		Primitive() = default;
		Primitive(const JsonObject &json);
		virtual std::shared_ptr<Primitive> getShared();
		virtual bool intersect(const Ray &r, IntersectInfo &info) = 0;
		virtual bool intersectP(const Ray &r) const = 0;
		virtual void computeScatteringFunction(IntersectInfo &isect) const;

	protected:
	    std::unique_ptr<Material> _material;
	};
}
#endif // HOMURA_PRIMITIVE_H_
