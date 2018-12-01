#ifndef HOMURA_PRIMITIVE_H_
#define HOMURA_PRIMITIVE_H_

#include "renderer/Ray.h"
#include "renderer/IntersectInfo.h"
#include "renderer/materials/Material.h"
#include "core/io/JsonObject.h"
#include <memory>
#include <unordered_map>

namespace Homura {
	class Scene;
	class Shape;
	class Sphere;
	class Quad;

	class Primitive : public std::enable_shared_from_this<Primitive> {
	public:
		Primitive() = default;
		Primitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bxdfs);
		virtual std::shared_ptr<Primitive> getShared();
		virtual bool intersect(const Ray &r, IntersectInfo &info) = 0;
		virtual bool intersectP(const Ray &r) const = 0;
		virtual void computeScatteringFunction(IntersectInfo &isect) const;

	protected:
	    std::unique_ptr<Material> _material;
	};

	class ShapePrimitive : public Primitive {
	public:
		ShapePrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs);

		virtual bool intersect(const Ray &r, IntersectInfo &info) override;
		virtual bool intersectP(const Ray &r) const override;

	private:
		std::unique_ptr<Shape> _shape;
	};

	class SpherePrimitive : public ShapePrimitive {
	public:
		SpherePrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs);
	};

	class QuadPrimitive : public ShapePrimitive {
	public:
		QuadPrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs);
	};
}
#endif // HOMURA_PRIMITIVE_H_
