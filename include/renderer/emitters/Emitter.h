#ifndef HOMURA_EMITTER_H_
#define HOMURA_EMITTER_H_

#include "core/Common.h"
#include "core/math/Geometry.h"
#include "core/math/Mat4f.h"
#include "renderer/IntersectInfo.h"
#include "core/io/JsonObject.h"
#include "renderer/textures/MIPMap.h"
#include "core/sampler/Sampler.h"
#include "renderer/shapes/Shape.h"

namespace Homura {
	class Scene;
	class Emitter;

	class VisibilityTester {
	public:
		VisibilityTester() = default;
		VisibilityTester(const IntersectInfo &isect1, const IntersectInfo &isect2) :
			_I1(isect1), _I2(isect2) {}

		const IntersectInfo &isect1() const { return _I1; }
		const IntersectInfo &isect2() const { return _I2; }

		bool unoccluded(const Scene &scene) const;
		bool unoccluded(const Scene &scene, std::shared_ptr<Emitter> evalemitter) const;
		Vec3f Tr(const Scene &scene, Sampler &sampler) const;

	private:
		IntersectInfo _I1, _I2;
	};

	enum EmitterFlags {
		DeltaPosition = 1 << 0,
		DeltaDirection = 1 << 1,
		Area = 1 << 2,
		Infinite = 1 << 3,
	};

    class Emitter : public std::enable_shared_from_this<Emitter>{
	public:
		Emitter(int flags, int n_samples=1)
			: _flags(flags), _n_samples(n_samples) {}


		/*
		@param isect_info: isect to be cast by emitter
		@param wi incident: direction from isect to emitter
		@param pdf: probability of wi being sampled
		@param vt: to be initialized as (isect_info, isect_emitter)
		*/
		virtual Vec3f sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u/*samples*/) const = 0;
		virtual float Pdf() const = 0;
		virtual Vec3f sample_Le(const Point2f &u1, const Point2f &u2, Ray &ray, Vec3f &normal, float &pdf_pos, float &pdf_dir) const { return Vec3f(0.f); }
		virtual void Pdf_Le(const Ray &r, const Vec3f &normal, float &pdf_pos, float &pdf_dir) const {}
		virtual Vec3f Le(const Ray &r) const { return Vec3f(0.f); }	// background radiance
		virtual Vec3f evalDirect(std::shared_ptr<Scene> scene, const IntersectInfo &isect_info, const Point2f &u);
		virtual Vec3f L(const IntersectInfo &isect_info, const Vec3f &w) const = 0;
		virtual Vec3f power() const = 0;
		virtual void preprocess() {}
		virtual bool isType(EmitterFlags flag) const;

		virtual Point3f pos() const = 0;

		inline bool isDelta() const {
			return (_flags & EmitterFlags::DeltaPosition) || (_flags & EmitterFlags::DeltaDirection);
		}

		const int _flags;
		const int _n_samples;
	};

	class PointEmitter : public Emitter {
	public:
		PointEmitter(const Point3f &p, const Vec3f &I, const Mat4f &local2world)
			: Emitter(EmitterFlags::DeltaPosition), _p(p), _I(I), _local2world(local2world), _world2local(Mat4f::inverse(local2world)) {}

		PointEmitter(const JsonObject &json);

		Vec3f sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u) const override;
		float Pdf() const override { return 1.f; }
		Vec3f L(const IntersectInfo &isect_info, const Vec3f &w) const override { return _I; }
		Vec3f power() const override;

		Point3f pos() const override { return _p; }

		const Point3f _p;
		const Vec3f _I;
		const Mat4f _local2world, _world2local;
	};

	class DirectionalEmitter : public Emitter {
	public:
		DirectionalEmitter(const JsonObject &json);

		void preprocess() override;	/// TODO: bounds of scene?
		Vec3f sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u/*samples*/) const override;
		float Pdf() const override { return 1.f; }
		Vec3f L(const IntersectInfo &isect_info, const Vec3f &w) const override { return _L; }
		Vec3f power() const override;

		Point3f pos() const override { return Point3f(0); }

		const Vec3f _d;
		const Vec3f _L;

	private:
		Point3f _world_center;
		float _world_radius;
	};

	class AreaEmitter : public Emitter {
	public:
		AreaEmitter::AreaEmitter(const JsonObject &json);
		AreaEmitter::AreaEmitter(const JsonObject &json, std::shared_ptr<Shape> pshape);

		virtual Vec3f L(const IntersectInfo &isect_info, const Vec3f &w) const override = 0;

		Point3f pos() const override { return _shape->_p; }

		std::shared_ptr<Shape> _shape;
		const Vec3f _I;
		float _area;
	};

	class DiffuseAreaEmitter : public AreaEmitter {
	public:
		DiffuseAreaEmitter(const JsonObject &json);
		DiffuseAreaEmitter(const JsonObject &json, std::shared_ptr<Shape> pshape);

		Vec3f sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u) const override;
		float Pdf() const override { return _shape->pdf(); }
		Vec3f sample_Le(const Point2f &u1, const Point2f &u2, Ray &ray, Vec3f &normal, float &pdf_pos, float &pdf_dir) const override;
		void Pdf_Le(const Ray &r, const Vec3f &normal, float &pdf_pos, float &pdf_dir) const override;

		Vec3f L(const IntersectInfo &isect_info, const Vec3f &w) const override;
		Vec3f power() const override;
	};

	//class DiffuseAreaEmitter: public AreaEmitter {
	//public:
	//	DiffuseAreaEmitter::DiffuseAreaEmitter(const JsonObject &json);

	//	Vec3f sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, const Point2f &u/*samples*/) const override;
	//	Vec3f power() const override;
	//	Vec3f L(const IntersectInfo &isect_info, const Vec3f &wo) const = 0;

	//protected:
	//	Vec3f _L;
	//	std::shared_ptr<Shape> _shapes;
	//	const float _area;
	//};

	class InfiniteEmitter : public Emitter {
	public:
		InfiniteEmitter::InfiniteEmitter(const JsonObject &json);

		void preprocess() override;
		Vec3f sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u) const override;
		Vec3f power() const override;
		Vec3f Le(const Ray &r) const override;

		Point3f pos() const override { return _p; }

		const Point3f _p;
		const Vec3f _I;
		const Mat4f _local2world, _world2local;
	private:
		std::unique_ptr<MIPMap<float, 3>> _Lmap;
		Point3f _world_center;
		float _world_radius;
	};

	//class SpotlightEmitter : public Emitter {
	//public:
	//	SpotlightEmitter(const JsonObject &json);

	//	Vec3f sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u) const override;
	//	Vec3f power() const override;

	//	const Vec3f _d;
	//	const Vec3f _I;
	//	const float _cos_inner, _cos_outer;

	//private:
	//	float falloff(const Vec3f &wi) const;
	//};
}

#endif //HOMURA_EMITTER_H_
