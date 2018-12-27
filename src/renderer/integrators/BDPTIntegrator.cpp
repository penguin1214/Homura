#include "renderer/integrators/BDPTIntegrator.h"
#include "renderer/Helper.h"

namespace Homura {
	inline Vertex Vertex::createSensor(std::shared_ptr<ProjectiveSensor> sensor, const Ray &ray, const Vec3f &beta) {
		return Vertex(VertexType::SENSOR, EndpointInfo(sensor, ray), beta);
	}

	inline Vertex Vertex::createSensor(std::shared_ptr<ProjectiveSensor> sensor, const IntersectInfo &info, const Vec3f &beta) {
		return Vertex(VertexType::SENSOR, EndpointInfo(info, sensor), beta);
	}

	inline Vertex Vertex::createEmitter(std::shared_ptr<Emitter> emitter, const Ray &ray, const Vec3f &n_light, const Vec3f &Le, float pdf) {
		Vertex v(VertexType::LIGHT, EndpointInfo(emitter, ray, n_light), Le);
		v._pdfFwd = pdf;
		return v;
	}

	inline Vertex Vertex::createEmitter(const EndpointInfo &ei, const Vec3f &beta, const float &pdf) {
		Vertex v(VertexType::LIGHT, ei, beta);
		v._pdfFwd = pdf;
		return v;
	}

	inline Vertex Vertex::createEmitter(const Ray &ray, const Vec3f &beta, const float &pdf) {
		Vertex v(VertexType::LIGHT, EndpointInfo(ray), beta);
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
		Vec3f wi = (next.p() - p()).normalized();
		const IntersectInfo info = getInfo();
		return info._bsdf->f(info._wo, wi);	/// TODO: medium
		//return getInfo()._bsdf->f(getInfo()._wo, wi);	/// TODO: medium
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

	/* Handle both incident and emitting pdfs.
	*/
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

	float Vertex::PdfEmitterOrigin(const std::shared_ptr<Scene> sc, const Vertex &v) const {
		Vec3f w = (v.p() - p()).normalized();
		if (isInfiniteEmitter()) {
			/// TODO: infinite light
		}
		else {
			float pdf_pos, pdf_dir, pdf_choice = 0.f;
			/// TODO: check isEmitter()
			const std::shared_ptr<Emitter> emitter = (_type == VertexType::LIGHT) ? (_ei._emitter) : (_si._primitive->getEmitter());

			/// TODO: check if emitter==nullptr

			pdf_choice = 1.f / sc->_emitters.size();
			emitter->Pdf_Le(Ray(p(), w), ng(), pdf_pos, pdf_dir);
			return pdf_pos * pdf_choice;
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	BDPTIntegrator::BDPTIntegrator(std::shared_ptr<Scene> sc, const JsonObject &json)
	: _scene(sc), _len_subpath(json["subpath"].getInt()), _max_depth(2*_len_subpath-2) {
		auto sampler_json = json["sampler"];
		std::string sampler_type = sampler_json["type"].getString();

		if (sampler_type == "uniform")
			;//_sampler = std::unique_ptr<UniformSampler>(new UniformSampler(json));
		else if (sampler_type == "stratified")
			_sampler = std::unique_ptr<StratifiedSampler>(new StratifiedSampler(sampler_json));
		else
			std::cerr << "NOT_IMPLEMENTED_ERROR: Sampler [" << sampler_type << "] not implemented." << std::endl;
	}

	void BDPTIntegrator::render() {
		// generate camera and light sample
		float Li_weight = 1.0f / _sampler->_spp;
		float invH = 1.f / _scene->_cam->_film->height();

#if 0
		int xx = 68;
		int yy = 254;
		int x_region = 1;
		int y_region = 1;
		float inv_region = 1.f / float(y_region);
		for (int y = yy; y < yy + y_region; y++) {
			fprintf(stderr, "\r Rendering %5.2f%%\n", (y - yy + 1)*inv_region);
			for (int x = xx; x < xx + x_region; x++) {
				//std::cout << "(" << x << ", " << y << "):" << std::endl;
#else
		for (int y = 0; y < _scene->_cam->_film->height(); y++) {
			fprintf(stderr, "\rRendering %5.2f%%", invH*y);
			for (int x = 0; x < _scene->_cam->_film->width(); x++) {
#endif
				Vec2i current_pixel(x, y);
				_sampler->startPixel(current_pixel);

				do {
					// for every sample, generate different position on the film.
					Vec3f L(0.f);
					Point2f p_film = Point2f(current_pixel.x(), current_pixel.y()) + _sampler->get2D();
					std::vector<Vertex> sensor_vertices(_len_subpath + 2);	/// TODO: extra vertex on camera? "Camera subpaths get yet again one more vertex, which allows camera paths to randomly intersect light sources—this"
					std::vector<Vertex> emitter_vertices(_len_subpath + 1);

					// sample camera subpath
					int n_sensor_vertex = generateSensorSubpath(sensor_vertices, p_film);
					//for (int i = 1; i < n_sensor_vertex; i++) {
					//	draw_line(_scene, sensor_vertices[i-1].p(), sensor_vertices[i].p(), 0.05f, Vec3f(0,1,0));
					//}
					//std::cout << "Generated " << n_sensor_vertex << " camera vertices" << std::endl;
					// sample light subpath
					int n_emitter_vertex = generateEmitterSubpath(emitter_vertices);
					//for (int i = 1; i < n_emitter_vertex; i++) {
					//	draw_line(_scene, emitter_vertices[i - 1].p(), emitter_vertices[i].p(), 0.05f, Vec3f(1,1,1));
					//}
					//std::cout << "Generated " << n_emitter_vertex << " light vertices" << std::endl;
					// connect
					//std::cout << "Connecting vertices..." << std::endl;

					for (int t = 1; t <= n_sensor_vertex; t++) {
						for (int s = 0; s <= n_emitter_vertex; s++) {
							int depth = t + s - 2;	// depth is number of bounces (exclude endpoints)
							if (t == 1 && s == 1 || depth < 0 || depth > _max_depth) continue;

							// do connection and compute contribution
							Point2f p_film_new = p_film;
							Vec3f L_path = connectBDPT(t, s, sensor_vertices, emitter_vertices, &p_film_new, nullptr);
							if (t != 1)
								L += L_path;
							else {
								// addsplat
								_scene->_cam->_film->addSplat(p_film_new, L_path*Li_weight);
							}
						}
					}
					_scene->_cam->_film->addSample(p_film, L*Li_weight, 1.f);
				} while (_sampler->startNextSample());
			}
		}
		char fn[1024];
		sprintf(fn, "test.ppm");
		_scene->_cam->_film->writeColorBuffer(fn);
	}

	int BDPTIntegrator::generateSensorSubpath(std::vector<Vertex> &v_path, const Point2f &p_f) {
		if (_len_subpath == 0)	return 0;
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

	int BDPTIntegrator::generateEmitterSubpath(std::vector<Vertex> &l_path) {
		if (_len_subpath == 0)	return 0;
		int n_l = _scene->_emitters.size();
		int sampled_light;
		do {
			sampled_light = std::floor(n_l*_sampler->get1D());
		} while (!(sampled_light < n_l));

		const std::shared_ptr<Emitter> light = _scene->_emitters[sampled_light]->getEmitter();
		float light_pdf = 1.f / (float)n_l;

		float pdf_pos, pdf_dir;
		Ray ray;
		Vec3f normal_light;
		Vec3f Le = light->sample_Le(_sampler->get1D(), _sampler->get2D(), ray, normal_light, pdf_pos, pdf_dir);
		if (pdf_pos < 1e-6 || pdf_dir < 1e-6 || Le.max() < 1e-6)	return 0;

		l_path[0] = Vertex::createEmitter(light, ray, normal_light, Le, pdf_pos*light_pdf);
		Vec3f beta = Le * std::abs(normal_light.dot(ray._d)) / (light_pdf*pdf_pos*pdf_dir);	/// TODO: why divide two pdf?

		int n = randomWalk(ray, beta, pdf_dir, TransportMode::IMPORTANCE, l_path);

		/// TODO: infinite light

		return n;
	}

	int BDPTIntegrator::randomWalk(Ray &ray, Vec3f &beta, float &pdf, const TransportMode &mode, std::vector<Vertex> &path) {
		if (_len_subpath == 1) return 1;
		int current_depth = 1;
		float pdfFwd = pdf, pdfRev = 0.f;
		while (true) {
			if (beta.max() == 0) break;

			Vertex &vertex = path[current_depth];
			Vertex &prev = path[current_depth - 1];

			IntersectInfo isect;
			bool intersected = _scene->intersect(ray, isect);
			if (!intersected) {
				if (mode == TransportMode::RADIANCE) {
					// only for paths emitted by camera
					vertex = Vertex::createEmitter(ray, beta, pdfFwd);
					current_depth++;
				}
				break;
			}

			/// compute scattering function
			isect._transport_mode = mode;
			isect.computeScatteringFunction();

			/// initialize vertex
			vertex = Vertex::createSurface(isect, beta, pdfFwd, prev);

			if (++current_depth > _len_subpath)
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

			prev._pdfRev = pdfRev;

			// update ray
			ray = isect.spawnRay(wi);
		}
		return current_depth;
	}

	Vec3f BDPTIntegrator::connectBDPT(int t, int s, std::vector<Vertex> &camera_vertex, std::vector<Vertex> &light_vertex, Point2f *p_raster, float *mis) {
		Vec3f L(0.f);

		// handling excaped case
		if (t > 1 && s != 0 && camera_vertex[t - 1]._type == VertexType::LIGHT)
			return L;

		// connect
		Vertex sampled;
		if (s == 0) {
			//return L;
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
			//return L;
			const Vertex &qs = light_vertex[s - 1];
			if (qs.isConnectible()) {
				VisibilityTester vt;
				Vec3f wi;
				float pdf;
				Vec3f Wi = _scene->_cam->sample_Wi(qs.getInfo(), _sampler->get2D(), wi, pdf, p_raster, &vt);
				if (!(pdf < 1e-6) && !(Wi.max() < 1e-6) && vt.unoccluded(*_scene)/*TODO:necessary?*/) {
					sampled = Vertex::createSensor(_scene->_cam, vt.isect2(), Wi / pdf);
					L = qs._beta * qs.f(sampled) * sampled._beta;
					if (qs.isOnSurface()) {
						L *= std::abs(wi.dot(qs.ns()));
					}
				}
			}
			//draw_line(_scene, qs.p(), _scene->_cam->p(), 0.01f, Vec3f(1, 0, 0));
		}
		else if (s == 1) {
			// only one vertex on light subpath, i.e. the vertex on light surface
			// sample a vertex on light and connect to the camera subpath
			//return L;
			const Vertex pt = camera_vertex[t - 1];

			if (pt.isConnectible()) {
				// sample a light
				int n_l = _scene->_emitters.size();
				int sampled_light;
				do {
					sampled_light = std::floor(n_l*_sampler->get1D());
				} while (!(sampled_light < n_l));

				const std::shared_ptr<Emitter> light = _scene->_emitters[sampled_light]->getEmitter();
				float light_pdf = 1.f / (float)n_l;
				// evaluate direct
				Vec3f wi;
				float pdf;
				VisibilityTester vt;
				Vec3f Li = light->sample_Li(pt.getInfo(), wi, pdf, vt, _sampler->get2D());
				if (!(pdf < 1e-6) && !(Li.max() < 1e-6)) {
					EndpointInfo ei(vt.isect2(), light);
					sampled = Vertex::createEmitter(ei, Li / (pdf*light_pdf), 0);
					sampled._pdfFwd = sampled.PdfEmitterOrigin(_scene, pt);
					L = pt._beta * pt.f(sampled) * sampled._beta;
					//std::cout << pt._beta << std::endl;
					//std::cout << pt.f(sampled) << std::endl;
					//std::cout << sampled._beta << std::endl;
					if (pt.isOnSurface())
						L *= std::abs(pt.ns().dot(wi)) * vt.unoccluded(*_scene, light);
				}
				//draw_line(_scene, pt.p(), vt.isect2()._p, 0.01f, Vec3f(1, 0, 0));
			}
		}
		else {
			// plain cases
			//return L;
			const Vertex &qs = light_vertex[s - 1];
			const Vertex &pt = camera_vertex[t - 1];
			if (qs.isConnectible() && pt.isConnectible()) {
				L = qs._beta * qs.f(pt) * pt.f(qs) * pt._beta;
				//std::cout << "(" << t << ", " << s << ") : " << G(qs,pt) << std::endl;
				if (!(L.max() < 1e-6)) {
					L *= G(qs, pt);
				}
			}
			//draw_line(_scene, qs.p(), pt.p(), 0.01f, Vec3f(1, 0, 0));
		}

		//float mis_weight = MISWeight(camera_vertex, light_vertex, t, s);
		//L *= mis_weight;

		//if (mis)
		//	*mis = mis_weight;

		return L;
	}

	Vec3f BDPTIntegrator::G(const Vertex &v0, const Vertex &v1) const {
		Vec3f d = v0.p() - v1.p();
		float g = 1.f / d.squareLength();
		d *= std::sqrt(g);
		if (v0.isOnSurface())
			g *= std::abs(v0.ng().dot(d));	/// TODO: ns or ng?
		if (v1.isOnSurface())
			g *= std::abs(v1.ng().dot(d));
		VisibilityTester vt(v0.getInfo(), v1.getInfo());
		return g/* * vt.unoccluded(*_scene)*/;	/// TODO: Tr() for medium
	}

	float BDPTIntegrator::MISWeight(std::vector<Vertex> &camera_vertex, std::vector<Vertex> &light_vertex, int t, int s) {
		if (t + s == 2)
			return 1.f;

		float sum_ri = 0.f;
		auto map0 = [](float f)->float {return f != 0 ? f : 1; };

		float ri = 1.f;
		for (int i = t - 1; i > 0; --i) {
			ri *= map0(camera_vertex[i]._pdfRev) / map0(camera_vertex[i]._pdfFwd);
			if (!camera_vertex[i]._is_delta && !camera_vertex[i - 1]._is_delta)
				sum_ri += ri;
		}

		ri = 1.f;
		for (int i = s - 1; i >= 0; i--) {
			ri *= map0(light_vertex[i]._pdfRev) / map0(light_vertex[i]._pdfFwd);
			/// TODO: check delta

			if (!light_vertex[i]._is_delta /* TODO: && !deltaLightVertex*/)
				sum_ri += ri;
		}

		return 1.f / (sum_ri + 1.f);
	}
}