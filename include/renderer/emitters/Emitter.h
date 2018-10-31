#ifndef HOMURA_EMITTER_H_
#define HOMURA_EMITTER_H_

#include "core/Common.h"
#include "core/math/Geometry.h"
#include "core/math/Mat4f.h"
#include "renderer/IntersectInfo.h"
#include "core/io/JsonObject.h"
#include "renderer/textures/MIPMap.h"
#include "renderer/Scene.h"
#include "core/sampler/Sampler.h"

namespace Homura {

	class VisibilityTester {
	public:
		VisibilityTester() = default;
		VisibilityTester(const IntersectInfo &isect1, const IntersectInfo &isect2) :
			_I1(isect1), _I2(isect2) {}

		const IntersectInfo &isect1() const { return _I1; }
		const IntersectInfo &isect2() const { return _I2; }

		bool unoccluded(const Scene &scene) const;
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

    class Emitter {
	public:
		Emitter(int flags, const Mat4f &local2world, const Vec3f p = Vec3f(0.f), int n_samples=1)
			: _flags(flags), _local2world(local2world), _world2local(Mat4f::inverse(local2world)), _p(p), _n_samples(n_samples) {}

		virtual Vec3f sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u/*samples*/) const = 0;
		virtual Vec3f Le(const Ray &r) const { return Vec3f(0.f); }	// background radiance
		virtual Vec3f evalDirect(std::shared_ptr<Scene> scene, const IntersectInfo &isect_info, const Point2f &u) const;
		virtual Vec3f power() const = 0;
		virtual void preprocess() {}

		inline bool isDelta() const {
			return (_flags & EmitterFlags::DeltaPosition) || (_flags & EmitterFlags::DeltaDirection);
		}

		const Point3f _p;
		const int _flags;
		const int _n_samples;
		const Mat4f _local2world, _world2local;
	};

	class PointEmitter : public Emitter {
	public:
		PointEmitter(const Mat4f &local2world, const Vec3f &I)
			: Emitter(EmitterFlags::DeltaPosition, local2world, Vec3f(0.f)), _I(I) {}

		PointEmitter(const JsonObject &json);

		Vec3f sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u) const override;
		Vec3f evalDirect(std::shared_ptr<Scene> scene, const IntersectInfo &isect_info, const Point2f &u) const override;
		Vec3f power() const override;

		const Vec3f _I;
	};

	class SpotlightEmitter : public Emitter {
	public:
		SpotlightEmitter(const JsonObject &json);

		Vec3f sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u) const override;
		Vec3f power() const override;

		const Vec3f _d;
		const Vec3f _I;
		const float _cos_inner, _cos_outer;

	private:
		float falloff(const Vec3f &wi) const;
	};

	class DirectionalEmitter : public Emitter {
	public:
		DirectionalEmitter(const JsonObject &json);

		void preprocess() override;	/// TODO: bounds of scene?
		Vec3f sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u/*samples*/) const override;
		Vec3f evalDirect(std::shared_ptr<Scene> scene, const IntersectInfo &isect_info, const Point2f &u) const override;
		Vec3f power() const override;

		const Vec3f _d;
		const Vec3f _L;

	private:
		Point3f _world_center;
		float _world_radius;
	};

	class AreaEmitter : public Emitter {
	public:
		AreaEmitter::AreaEmitter(const JsonObject &json);

		virtual Vec3f L(const IntersectInfo &isect_info, const Vec3f &wo) const = 0;
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

	private:
		std::unique_ptr<MIPMap<float, 3>> _Lmap;
		Point3f _world_center;
		float _world_radius;
	};
}

#endif //HOMURA_EMITTER_H_
