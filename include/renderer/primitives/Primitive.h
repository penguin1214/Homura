#ifndef HOMURA_PRIMITIVE_H_
#define HOMURA_PRIMITIVE_H_

#include "renderer/Ray.h"
#include "renderer/IntersectInfo.h"
#include "renderer/materials/Material.h"
#include "renderer/emitters/Emitter.h"
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
		virtual bool intersect(const Ray &r, IntersectInfo &info);
		virtual std::shared_ptr<Primitive> intersectP(const Ray &r);
		virtual void computeScatteringFunction(IntersectInfo &isect) const;

		virtual bool isEmitter() const { return (_emitter != nullptr); }
		virtual std::shared_ptr<Emitter> getEmitter() const { return _emitter; }

		virtual Bound3f worldBound() const { return _shape->worldBound(); }

	protected:
	    std::unique_ptr<Material> _material;
		std::shared_ptr<Emitter> _emitter;
		std::shared_ptr<Shape> _shape;
	};

	class TriangleMeshPrimitive : public Primitive {
	public:
		TriangleMeshPrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs);
	};

	class SpherePrimitive : public Primitive {
	public:
		SpherePrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs);
	};

	class QuadPrimitive : public Primitive {
	public:
		QuadPrimitive(const JsonObject &json, std::unordered_map<std::string, std::shared_ptr<BxDF>> &bsdfs);
	};

	class PrimitiveGroup : public Primitive {
	public:
		PrimitiveGroup() = default;
		//std::shared_ptr<PrimitiveGroup> getShared() override;
		void computeScatteringFunction(IntersectInfo &isect) const;

		bool isEmitter() const { return (_emitter != nullptr); }
		std::shared_ptr<Emitter> getEmitter() const { return _emitter; }

		std::vector<std::shared_ptr<Primitive>> _primitives;
	};

}
#endif // HOMURA_PRIMITIVE_H_
