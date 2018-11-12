#include "renderer/integrators/Integrator.h"
#include "renderer/emitters/Emitter.h"

namespace Homura {
    Integrator::~Integrator() {}

    SamplerIntegrator::SamplerIntegrator(std::shared_ptr<Scene> sc, const JsonObject &json) :
	_scene(std::move(sc)) {
		auto sampler_json = json["sampler"];
		std::string sampler_type = sampler_json["type"].getString();

		if (sampler_type == "uniform")
			;//_sampler = std::unique_ptr<UniformSampler>(new UniformSampler(json));
		else if (sampler_type == "stratified")
			_sampler = std::unique_ptr<StratifiedSampler>(new StratifiedSampler(sampler_json));
		else
			std::cerr << "NOT_IMPLEMENTED_ERROR: Sampler [" << sampler_type << "] not implemented." << std::endl;
	}

	/*
	All derived classes of SamplerIntegrator uses the same implementation of render(),
	except for their different implementations of interfaces used in it.
	*/
	void SamplerIntegrator::render() {
		float Li_weight = 1.0f / _sampler->_spp;
		float invH = 1.f / _scene->_cam->_film->height();

//#pragma omp parallel for
		for (int y = 0; y < _scene->_cam->_film->height(); y++) {
			fprintf(stderr, "\rRendering %5.2f%%", invH*y);
			for (int x = 0; x < _scene->_cam->_film->width(); x++) {
				Vec2i current_pixel(x, y);
				_sampler->startPixel(current_pixel);

				do {
					PixelSample pixel_sample = _sampler->getSensorSample(current_pixel);
					
					Ray ray;
					float ray_weight = _scene->_cam->generatePrimaryRay(pixel_sample, ray);

					Vec3f radiance = Li(ray, _sampler->clone(_sampler->currentSampleIdx()));	/// if multithread, should use clone.
					_scene->_cam->_film->addSample(pixel_sample._p_film, radiance*Li_weight, ray_weight);
				} while (_sampler->startNextSample());
			}
		}

		char fn[1024];
		sprintf(fn, "test.ppm");
		_scene->_cam->_film->writeColorBuffer(fn);
	}

	Vec3f SamplerIntegrator::evaluateDirect(const IntersectInfo &isect_info) const {
		Vec3f L(0.0f);
		for (auto light : _scene->_emitters) {
			L += light->evalDirect(_scene, isect_info, _sampler->get2D());
		}
		return L;
	}
}