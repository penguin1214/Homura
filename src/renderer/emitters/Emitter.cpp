#include "renderer/emitters/Emitter.h"
#include "core/Common.h"
#include "renderer/bxdfs/BxDF.h"
#include "renderer/phase/PhaseFunction.h"
#include "renderer/Scene.h"
#include "core/math/CoordinateSystem.h"
#include <fstream>

namespace Homura {

	/*
	@param isect_info: isect to be  cast by emitter
	*/
	Vec3f Emitter::evalDirect(std::shared_ptr<Scene> scene, const IntersectInfo &isect_info, std::shared_ptr<PixelSampler> sampler) {
		Vec3f wi;
		float light_pdf = 0.f, scatter_pdf = 0.f;
		VisibilityTester vt;
		Vec3f Li_sampled = sample_Li(isect_info, wi, light_pdf, vt, sampler->get2D());
		if (light_pdf > 1e-6 && Li_sampled.max() > 1e-6) {
			Vec3f f;
			if (isect_info.isSurfaceIntersect()) {
				f = isect_info._bsdf->f(isect_info._wo, wi);
				f *= std::abs(isect_info._shading._n.dot(wi));
				scatter_pdf = isect_info._bsdf->Pdf(isect_info._wo, wi);	/// TODO: needed?
			}
			else {
				const MediumIntersectInfo &mi = static_cast<const MediumIntersectInfo&>(isect_info);
				scatter_pdf = mi._phase->p(mi._wo, wi);
				f = Vec3f(scatter_pdf);
			}

			if (f.max() > 1e-6) {
				Li_sampled *= vt.Tr(scene, shared_from_this(), sampler);
				return f * Li_sampled / light_pdf;
			}
			else
				return Vec3f(0.f);
		}
		else
			return Vec3f(0.f);
		//return (vt.unoccluded(*scene, shared_from_this())) ? (f*Li_sampled) / light_pdf : Vec3f(0.f);
		/// TODO: MIS
	}

	bool Emitter::isType(EmitterFlags flag) const {
		return (_flags & flag) != 0;
	}

	PointEmitter::PointEmitter(const Homura::JsonObject &json) :
		Emitter(EmitterFlags::DeltaPosition, json["n_samples"].getInt()),
		_p(json["transform"]["translate"].getVec3()), _I(json["intensity"].getVec3()), _local2world(json["transform"].getTransform()), _world2local(Mat4f::inverse(_local2world)) {}

	Vec3f PointEmitter::sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u) const {
		//std::cout << "intersection point: " << isect_info._p << std::endl;
		wi = (_p - isect_info._p).normalized();
		//std::cout << "out direction: " << wi << std::endl;
		pdf = Pdf();
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
		pdf = Pdf();
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

	AreaEmitter::AreaEmitter(const JsonObject &json, std::shared_ptr<Shape> pshape)
	: Emitter(EmitterFlags::Area, json["n_samples"].getInt()),
	_I(json["intensity"].getVec3()) {
		_shape = pshape;
		_area = _shape->area();
	}

	DiffuseAreaEmitter::DiffuseAreaEmitter(const JsonObject &json) :
		AreaEmitter(json) {}

	DiffuseAreaEmitter::DiffuseAreaEmitter(const JsonObject &json, std::shared_ptr<Shape> pshape)
	: AreaEmitter(json, pshape) {}

	Vec3f DiffuseAreaEmitter::sample_Li(const IntersectInfo &isect_info, Vec3f &wi, float &pdf, VisibilityTester &vt, const Point2f &u) const {
		IntersectInfo isect_emitter = _shape->sample(u);
		wi = (isect_emitter._p - isect_info._p).normalized();
		/// TODO: medium interface?
		pdf = Pdf();
		vt = VisibilityTester(isect_info, isect_emitter);
		//return L(isect_emitter, -wi) * std::abs(_shape->normal(isect_emitter).dot(-wi)) / (isect_emitter._p - isect_info._p).squareLength();
		return L(isect_emitter, -wi) / (isect_emitter._p - isect_info._p).squareLength();	/// TODO: need check
	}

	Vec3f DiffuseAreaEmitter::sample_Le(const Point2f &u1, const Point2f &u2, Ray &ray, Vec3f &normal, float &pdf_pos, float &pdf_dir) const {
		IntersectInfo emitter_sample(_shape->sample(u1));
		pdf_pos = _shape->pdf();
		normal = _shape->normal(emitter_sample);
		emitter_sample._normal = normal;

		Vec3f wo = cosineSampleHemisphereSolidAngle(u2);
		Vec3f t, b;
		ShadingCoordinateSystem(normal, t, b);
		Vec3f wo_w = wo.x()*t + wo.y()*b + wo.z()*normal;
		pdf_dir = (normal.dot(wo_w) > 0.f) ? (AbsCosTheta(wo)*INVPI) : 0.f ;

		ray = emitter_sample.spawnRay(wo_w);
		
		return L(emitter_sample, wo_w);
	}

	void DiffuseAreaEmitter::Pdf_Le(const Ray &r, const Vec3f &normal, float &pdf_pos, float &pdf_dir) const {
		IntersectInfo isect(r._o);
		pdf_pos = _shape->pdf();
		pdf_dir = (std::abs(r._d.dot(normal)*INVPI));
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

	bool VisibilityTester::unoccluded(const Scene &scene, std::shared_ptr<Emitter> evalemitter) const {
		return !(scene.intersectP(_I1.spawnRayTo(_I2), evalemitter));
	}

	Vec3f VisibilityTester::Tr(std::shared_ptr<Scene> scene, std::shared_ptr<Emitter> evalemitter, std::shared_ptr<PixelSampler> sampler) const {
		/// TODO
		return unoccluded(*scene, evalemitter);
	}

}