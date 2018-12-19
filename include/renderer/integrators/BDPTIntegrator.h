#ifndef HOMURA_BDPTINTEGRATOR_H_
#define HOMURA_BDPTINTEGRATOR_H_

#include "Integrator.h"

namespace Homura {
	enum VertexType {
		SENSOR, LIGHT, SURFACE
	};

	class Vertex {
	public:
		Vertex() : _ei() {}
		Vertex(VertexType type, const EndpointInfo &ei, const Vec3f &beta)
			: _type(type), _ei(ei), _beta(beta) {}
		Vertex(const IntersectInfo &si, const Vec3f &beta)
			: _type(VertexType::SURFACE), _si(si), _beta(beta) {}

		Vertex(const Vertex &v) { memcpy(this, &v, sizeof(Vertex)); }

		~Vertex() {}

		Vertex &operator=(const Vertex &v) {
			memcpy(this, &v, sizeof(Vertex));
			return *this;
		}

		static inline Vertex createSensor(std::shared_ptr<ProjectiveSensor> sensor, const Ray &ray, const Vec3f &beta);
		static inline Vertex createEmitter(std::shared_ptr<Emitter> emitter, const Ray &ray, const Vec3f &n_light, const Vec3f &Le, float pdf);
		static inline Vertex createSurface(const IntersectInfo &si, const Vec3f &beta, float pdf, const Vertex &prev);

		const IntersectInfo &getInfo() const {
			switch (_type)
			{
			case Homura::SURFACE:	return _si;
			default:	return _ei;
			}
		}

		inline const Point3f &p() const { return getInfo()._p; }
		inline const Vec3f &ng() const { return getInfo()._normal; }
		/// TODO: other getters

		Vec3f Le(const Vertex &v) const;

		bool isOnSurface() const;	/// TODO
		bool isEmitter() const {
			return (_type == VertexType::LIGHT) || (_type==VertexType::SURFACE && _si._primitive->isEmitter());
		}
		bool isDeltaEmitter() const {
			return (_type == VertexType::LIGHT) && (_ei._emitter) && (_ei._emitter->isDelta());
		}
		bool isInfiniteEmitter() const;
		bool isConnectible() const;

		Vec3f f(const Vertex &next) const;
		float Pdf(const Vertex *prev, const Vertex &next) const;
		float PdfEmitter(const Vertex &next) const;

		/* Convert solid angle pdf -> unit area pdf */
		float convertPdf(float pdf/*w.r.t. solid angle*/, const Vertex &next) const;

		VertexType _type;
		Vec3f _beta;	/// TODO
		bool _is_delta;	/// TODO

		float _pdfFwd = 0.f, _pdfRev = 0.f;

		union {
			IntersectInfo _si;
			EndpointInfo _ei;
		};
	};


	class BDPTIntegrator : public Integrator {
	public:
		BDPTIntegrator(std::shared_ptr<Scene> sc, const JsonObject &json);

		virtual void render() override;

		int generateSensorSubpath(std::vector<Vertex> v_path, const Point2f &p_f);
		int generateEmitterSubpath(std::vector<Vertex> l_path);

		int randomWalk(Ray &ray, Vec3f &beta, float &pdf, const TransportMode &mode, std::vector<Vertex> path);
		Vec3f connectBDPT(int t, int s, std::vector<Vertex> &camera_vertex, std::vector<Vertex> &light_vertex, Point2f *p_raster);
		Vec3f G(const Vertex &v0, const Vertex &v1) const;

	private:
		float correctShadingNormal() const;

		std::shared_ptr<Scene> _scene;
		std::unique_ptr<PixelSampler> _sampler;
		const int _max_depth;
	};
}

#endif // !HOMURA_BDPTINTEGRATOR_H_
