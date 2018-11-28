#include "renderer/emitters/Emitter.h"
#include "core/Common.h"
#include "renderer/bxdfs/BxDF.h"
#include <fstream>

namespace Homura {

	Vec3f Emitter::evalDirect(std::shared_ptr<Scene> scene, const IntersectInfo &isect_info, const Point2f &u) const {
		Vec3f wi;
		float light_pdf;
		VisibilityTester vt;
		Vec3f Li_sampled = sample_Li(isect_info, wi, light_pdf, vt, u);
		Vec3f f = isect_info._bsdf->f(isect_info._wo, wi)*std::abs(isect_info._shading._n.dot(wi));
		return (vt.unoccluded(*scene)) ? (f*Li_sampled) / light_pdf : Vec3f(0.f);
	}

	PointEmitter::PointEmitter(const Homura::JsonObject &json) :
		Emitter(EmitterFlags::DeltaPosition, json["n_samples"].getInt()),
		_p(json["transform"]["translate"].getVec3()), _I(json["intensity"].getVec3()), _local2world(json["transform"].getTransform()), _world2local(Mat4f::inverse(_local2world)) {}

	Vec3f PointEmitter::sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u) const {
		//std::cout << "intersection point: " << isect_info._p << std::endl;
		wi = (_p - isect_info._p).normalized();
		//std::cout << "out direction: " << wi << std::endl;
		pdf = 1.f;
		vt = VisibilityTester(isect_info, IntersectInfo(_p));
		return _I / (_p - isect_info._p).squareLength();
	}

	Vec3f PointEmitter::power() const {
		return _I * 4.f * PI;
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	//SpotlightEmitter::SpotlightEmitter(const JsonObject &json) :
	//Emitter(EmitterFlags::DeltaPosition, json["transform"].getTransform(), json["transform"]["traslate"].getVec3(), json["n_samples"].getInt()),
	//_d(Vec3f(0,-1,0)/*default value*/*_local2world), _I(json["intensity"].getVec3()),
	//_cos_inner(std::cos(json["inner_cone"].getFloat())), _cos_outer(std::cos(json["outer_cone"].getFloat())) {}

	//float SpotlightEmitter::falloff(const Vec3f &wi) const {
	//	float cos_wi = wi.dot(_d);
	//	if (cos_wi < _cos_outer)	return 0.f;
	//	else if (cos_wi > _cos_inner)	return 1.f;
	//	else {
	//		float delta = (cos_wi - _cos_outer) / (_cos_inner - _cos_outer);
	//		return (delta*delta) * (delta*delta);	/// TODO: why?
	//	}
	//}

	//Vec3f SpotlightEmitter::sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u) const {
	//	wi = (_p - isect_info._p).normalized();
	//	pdf = 1.f;
	//	return _I * falloff(wi) / (_p - isect_info._p).squareLength();
	//}

	//Vec3f SpotlightEmitter::power() const {
	//	return _I * 2.f * PI * (1.f - 0.5f*(_cos_outer + _cos_inner));
	//}

	///////////////////////////////////////////////////////////////////////////////////////////

	DirectionalEmitter::DirectionalEmitter(const JsonObject &json) :
		Emitter(EmitterFlags::DeltaDirection, json["n_samples"].getInt()),
		_d(json["direction"].getVec3().normalized()), _L(json["intensity"].getVec3()) {}

	Vec3f DirectionalEmitter::sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u/*samples*/) const {
		wi = -_d;
		pdf = 1.f;
		Point3f p_out = isect_info._p - _d * (2 * _world_radius);
		vt = VisibilityTester(isect_info, IntersectInfo(p_out));
		return _L;
	}

	Vec3f DirectionalEmitter::power() const {
		return _L * PI * _world_radius * _world_radius;
	}


	void DirectionalEmitter::preprocess() {
		/// TODO: compute scene bound
		_world_center = Point3f(0.f);
		_world_radius = 20;
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	AreaEmitter::AreaEmitter(const JsonObject &json) :
	Emitter(EmitterFlags::Area, json["n_samples"].getInt()),
	_I(json["intensity"].getVec3()) {
		// _shape
		auto shape = json["shape"];
		auto type = shape["type"].getString();
		if (type == "sphere") {
			_shape = std::make_shared<Sphere>(shape);
		}
		else if (type == "quad") {
			_shape = std::make_shared<Quad>(shape);
		}

		_area = _shape->area();
	}

	DiffuseAreaEmitter::DiffuseAreaEmitter(const JsonObject &json) :
		AreaEmitter(json) {}

	Vec3f DiffuseAreaEmitter::sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u) const {
		IntersectInfo isect_emitter = _shape->sample(u);
		wi = (isect_emitter._p - isect_info._p).normalized();
		pdf = _shape->pdf();
		vt = VisibilityTester(isect_info, isect_emitter);
		return L(isect_emitter, -wi);
	}

	Vec3f DiffuseAreaEmitter::L(const IntersectInfo &isect_info, const Vec3f &w) const {
		return isect_info._normal.dot(w) > 0.f ? _I : Vec3f(0.f);
	}

	Vec3f DiffuseAreaEmitter::power() const {
		return _I * _area * PI;
	}

	///////////////////////////////////////////////////////////////////////////////////////////

	InfiniteEmitter::InfiniteEmitter(const JsonObject &json) :
	Emitter(EmitterFlags::Infinite, json["n_samples"].getInt()),
	_p(json["transform"]["translate"].getVec3()), _I(json["intensity"].getVec3()), _local2world(json["transform"].getTransform()), _world2local(Mat4f::inverse(_local2world)) {
		_Lmap.reset(new MIPMap<float, 3>(json["path"].getString(), json["wrap_mode"].getInt()));
	}

	void InfiniteEmitter::preprocess() {
		/// TODO: world boudn
		_world_center = Point3f(0.f);
		_world_radius = 20;
	}

	Vec3f InfiniteEmitter::sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u) const { return Vec3f(0.f); }

	Vec3f InfiniteEmitter::power() const {
		return _world_radius * _world_radius * 100.f * PI;	/// TODO
	}

	Vec3f InfiniteEmitter::Le(const Ray &r) const {
		Vec3f wo = (r._d*_world2local).normalized();
		Point2f st(sphericalPhi(wo)*INV2PI, sphericalTheta(wo)*INV2PI);
		return _Lmap->lookup(st);
	}

	bool VisibilityTester::unoccluded(const Scene &scene) const {
		return !(scene.intersectP(_I1.spawnRayTo(_I2)));
	}

	Vec3f VisibilityTester::Tr(const Scene &scene, Sampler &sampler) const {
		/// TODO
		return Vec3f(1.f);
	}

}