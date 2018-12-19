#include "renderer/integrators/BDPTIntegrator.h"

namespace Homura {
	inline Vertex Vertex::createSensor(std::shared_ptr<ProjectiveSensor> sensor, const Ray &ray, const Vec3f &beta) {
		return Vertex(VertexType::SENSOR, EndpointInfo(sensor, ray), beta);
	}

	inline Vertex Vertex::createEmitter(std::shared_ptr<Emitter> emitter, const Ray &ray, const Vec3f &n_light, const Vec3f &Le, float pdf) {
		Vertex v(VertexType::LIGHT, EndpointInfo(emitter, ray, n_light), Le);
		v._pdfFwd = pdf;
		return v;
	}

	inline Vertex Vertex::createSurface(const IntersectInfo &si, const Vec3f &beta, float pdf, const Vertex &prev) {
		Vertex v(si, beta);
		v._pdfFwd = prev.convertPdf(pdf, v);
		return v;
	}

	Vec3f Vertex::Le(const Vertex &v) const {
		if (!isEmitter())
			return Vec3f(0.f);

		Vec3f wo = v.p() - p();
		if (wo.squareLength() < 1e-6)
			return Vec3f(0.f);

		wo.normalize();
		if (isInfiniteEmitter()) {
			/// TODO
			return Vec3f(0.f);
		}
		else {
			return _si.Le(wo);	/// TODO: check direction
		}
	}

	float Vertex::convertPdf(float pdf, const Vertex &next) const {
		// Multiply Jacobian matrix(transforming between distributions)
		/// TODO: infinite light
		Vec3f w = next.p() - p();
		float invdist2 = 1.f / w.squareLength();
		if (next.isOnSurface()) {
			pdf *= std::abs((w*std::sqrt(invdist2)).dot(next.ng()));
		}
		return pdf * invdist2;
	}

	Vec3f Vertex::f(const Vertex &next) const {
		Vec3f wi = (p() - next.p()).normalized();
		return getInfo()._bsdf->f(getInfo()._wo, wi);	/// TODO: medium
	}

	float Vertex::Pdf(const Vertex *prev, const Vertex &next) const {
		if (_type == VertexType::LIGHT)
			return PdfEmitter(next);

		Vec3f w_next = (next.p() - p()).normalized();
		// `prev` may be nullptr for endpoint vertex
		/// TODO: what if prev is nullptr?
		Vec3f w_prev;
		if (prev)
			w_prev = (prev->p() - p()).normalized();

		float pdf, placeholder;
		if (_type == VertexType::SENSOR)
			_ei._sensor->Pdf_We(_ei.spawnRay(w_next), placeholder, pdf);
		else if (_type == VertexType::SURFACE)
			pdf = _si._bsdf->Pdf(w_prev, w_next);

		return convertPdf(pdf, next);	/// TODO: why use `next`, shouldn't it be `this`? (directions are sampled w.r.t. current object's bsdf, is shouldn't be related to the sampled object?
	}

	float Vertex::PdfEmitter(const Vertex &next) const {
		Vec3f w = next.p() - p();
		float invdist2 = 1.f / w.squareLength();
		w *= std::sqrt(invdist2);

		float pdf;
		if (isInfiniteEmitter()) {
			/// TODO: special handling of infinite light
		}
		else {
			const std::shared_ptr<Emitter> light = (_type == VertexType::LIGHT) ? (_ei._emitter) : (_si._primitive->getEmitter());
			float pdf_pos, pdf_dir;
			light->Pdf_Le(Ray(p(), w), ng(), pdf_pos, pdf_dir);
		}

		if (next.isOnSurface())
			pdf *= std::abs(next.ng().dot(w));

		return pdf;
	}

	void BDPTIntegrator::render() {
		// generate camera and light sample
		float Li_weight = 1.0f / _sampler->_spp;
		float invH = 1.f / _scene->_cam->_film->height();

		for (int y = 0; y < _scene->_cam->_film->height(); y++) {
			fprintf(stderr, "\rRendering %5.2f%%", invH*y);
			for (int x = 0; x < _scene->_cam->_film->width(); x++) {
				Vec2i current_pixel(x, y);
				_sampler->startPixel(current_pixel);

				do {
					Vec3f L(0.f);
					Point2f p_film = Point2f(current_pixel.x(), current_pixel.y()) + _sampler->get2D();
					std::vector<Vertex> sensor_vertices(_max_depth + 2);
					std::vector<Vertex> emitter_vertices(_max_depth + 1);

					// sample camera subpath
					int n_sensor_vertex = generateSensorSubpath(sensor_vertices, p_film);
					// sample light subpath
					int n_emitter_vertex = generateEmitterSubpath(emitter_vertices);
					// connect
					for (int t = 1; t <= n_sensor_vertex; t++) {
						for (int s = 0; s <= n_emitter_vertex; s++) {
							int depth = t + s - 2;	/// TODO: check definition of depth
							// what is s=0 case? if direct light, s should be 1?
							if (t == 1 && s == 1 || depth < 0 || depth > _max_depth) continue;

							// do connection and compute contribution
							Point2f p_film_new = p_film;
							Vec3f L_path = connectBDPT(t, s, sensor_vertices, emitter_vertices, &p_film_new);
							if (t != 1)
								L += L_path;
							else {
								// addsplat
							}
						}
					}
					//_scene->_cam->_film->addSample(pixel_sample._p_film, radiance*Li_weight, ray_weight);
				} while (_sampler->startNextSample());
			}
		}
	}

	int BDPTIntegrator::generateSensorSubpath(std::vector<Vertex> v_path, const Point2f &p_f) {
		if (_max_depth == 0)	return 0;
		// sample initial ray on sensor
		SensorSample sensor_sample;
		sensor_sample._p_film = p_f;
		sensor_sample.time = _sampler->get1D();
		Ray ray;
		Vec3f beta = _scene->_cam->generatePrimaryRay(sensor_sample, ray);
		/// TODO: ray differential

		v_path[0] = Vertex::createSensor(_scene->_cam, ray, beta);

		float pdf_pos, pdf_dir;
		_scene->_cam->Pdf_We(ray, pdf_pos, pdf_dir);
		return randomWalk(ray, beta, pdf_dir, TransportMode::RADIANCE, v_path);
	}

	int BDPTIntegrator::generateEmitterSubpath(std::vector<Vertex> l_path) {
		if (_max_depth == 0)	return 0;
		/// TODO: sample emitter to use
		const std::shared_ptr<Emitter> light = _scene->_emitters[0]->getEmitter();
		float light_pdf;

		float pdf_pos, pdf_dir;
		Ray ray;
		Vec3f normal_light;
		Vec3f Le = light->sample_Le(_sampler->get1D(), _sampler->get2D(), ray, normal_light, pdf_pos, pdf_dir);
		if (pdf_pos == 0 || pdf_dir || Le.max() == 0)	return 0;

		l_path[0] = Vertex::createEmitter(light, ray, normal_light, Le, pdf_pos*light_pdf);
		Vec3f beta = Le * std::abs(normal_light.dot(ray._d)) / (light_pdf, pdf_pos, pdf_dir);	/// TODO: ???

		int n = randomWalk(ray, beta, pdf_dir, TransportMode::IMPORTANCE, l_path);

		/// TODO: infinite light

		return n;
	}

	int BDPTIntegrator::randomWalk(Ray &ray, Vec3f &beta, float &pdf, const TransportMode &mode, std::vector<Vertex> path) {
		int current_depth = 1;
		float pdfFwd = pdf, pdfRev = 0.f;
		while (true) {
			if (beta.max() == 0) break;
			IntersectInfo isect;
			bool intersected = _scene->intersect(ray, isect);
			if (!intersected) {
				/// special handle escaped ray
				break;
			}
			Vertex &vertex = path[current_depth];
			Vertex &prev = path[current_depth - 1];
			/// compute scattering function
			isect.computeScatteringFunction();	/// TODO: transport mode?

			/// initialize vertex
			vertex = Vertex::createSurface(isect, beta, pdfFwd, prev);

			if (++current_depth >= _max_depth)
				break;

			/// compute BSDF and reversed pdf
			Vec3f wi, wo = isect._wo;
			BxDFType sampled_type;
			Vec3f f = isect._bsdf->sample_f(wo, wi, pdfFwd, _sampler->get1D(), _sampler->get2D(), BSDF_ALL, &sampled_type);
			if (f.max() < 1e-6 || pdfFwd < 1e-6)
				break;

			beta *= f * std::abs(wi.dot(isect._shading._n)) / pdfFwd;
			pdfRev = isect._bsdf->Pdf(wi, wo);

			if (sampled_type & BSDF_SPECULAR) {
				vertex._is_delta = true;
				pdfFwd = pdfRev = 0.f;
			}
			/// TODO: correct shading normal

			/// TODO: pdfRev
			prev._pdfRev = pdfRev;

			// update ray
			ray = isect.spawnRay(wi);
		}
		return current_depth;
	}

	Vec3f BDPTIntegrator::connectBDPT(int t, int s, std::vector<Vertex> &camera_vertex, std::vector<Vertex> &light_vertex, Point2f *p_raster) {
		Vec3f L(0.f);

		// connect
		if (s == 0) {
			// no vertex on light subpath
			// interprete camera subpath as the complete path
			// i.e. when p_{t-1} is an emitter
			const Vertex &pt = camera_vertex[t - 1];
			if (pt.isEmitter())
				L = pt.Le(camera_vertex[t-2]) * pt._beta;	/// TODO: why pt.Le()?
		}
		else if (t == 1) {
			// only one vertex on camera subpath, i.e. the vertex on camera
			// sample a vertex on camera and connect to the light subpath
			const Vertex &qs = light_vertex[s - 1];
			if (qs.isConnectible()) {
				// TODO: sample_Wi
				VisibilityTester vt;
				Vec3f wi;
				float pdf;
				Vec3f Wi = _scene->_cam->sample_Wi(qs.getInfo(), _sampler->get2D(), wi, pdf, p_raster, &vt);
				if (Wi.max() < 1e-6 || pdf < 1e-6) {
					/// TODO: generate a vertex?
				}
			}
		}
		else if (s == 1) {
			// only one vertex on light subpath, i.e. the vertex on light surface
			// sample a vertex on light and connect to the camera subpath
			/// TODO: direct lighting
		}
		else {
			// plain cases
			const Vertex &qs = light_vertex[s - 1];
			const Vertex &pt = camera_vertex[t - 1];
			if (qs.isConnectible() && pt.isConnectible()) {
				L = qs._beta * qs.f(pt) * pt.f(qs) * pt._beta;
				if (!(L.max() < 1e-6))
					L *= G(qs, pt);
			}
		}

		/// TODO: compute MIS weight
		return L;
	}

	Vec3f BDPTIntegrator::G(const Vertex &v0, const Vertex &v1) const {
		Vec3f d = v0.p() - v1.p();
		float g = 1.f / d.squareLength();
		/// TODO d *=
		if (v0.isOnSurface())
			g *= std::abs(v0.ng().dot(d));	/// TODO: ns or ng?
		if (v1.isOnSurface())
			g *= std::abs(v1.ng().dot(d));
		VisibilityTester vt(v0.getInfo(), v1.getInfo());
		return g * vt.unoccluded(*_scene);	/// TODO: Tr() for medium
	}

}