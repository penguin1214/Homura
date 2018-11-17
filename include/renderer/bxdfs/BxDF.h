#ifndef HOMURA_BxDF_H_
#define HOMURA_BxDF_H_

#include "core/Common.h"
#include "core/math/Geometry.h"
#include "renderer/Ray.h"
#include "renderer/IntersectInfo.h"
#include <vector>
#include <memory>
#include <iostream>

namespace Homura {
    /////////////////////////// Utility Functions ///////////////////////////
    inline float CosTheta(const Vec3f &w) { return w.z(); }
    inline float Cos2Theta(const Vec3f &w) { return w.z()*w.z(); }
    inline float AbsCosTheta(const Vec3f &w) { return std::abs(w.z()); }
    inline float Sin2Theta(const Vec3f &w) { return std::max(0.f, 1.f - Cos2Theta(w)); }
    inline float SinTheta(const Vec3f &w) { return std::sqrt(Sin2Theta(w)); }
    inline float TanTheta(const Vec3f &w) { return SinTheta(w) / CosTheta(w); }
    inline float Tan2Theta(const Vec3f &w) { return Sin2Theta(w) / Cos2Theta(w); }
    inline float CosPhi(const Vec3f &w) {
        float r = SinTheta(w);
        return (r == 0.f) ? 1.f : clamp<float>(w.x()/r, -1.f, 1.f);
    }
    inline float SinPhi(const Vec3f &w) {
        float r = Cos2Theta(w);
        return (r == 0.f) ? 0.f : clamp<float>(w.y()/r, -1.f, 1.f);
    }
    inline float Cos2Phi(const Vec3f &w) { return CosPhi(w)*CosPhi(w); }
    inline float Sin2Phi(const Vec3f &w) { return SinPhi(w)*SinPhi(w); }
    /// TODO: CosDPhi?

    /// Note Reflect() and Refract() do not assume directions in shading coord!
    /// TODO: Reflect()
    inline bool Refract(const Vec3f &wi, const Vec3f &n, float eta, Vec3f &wt) {
        float cosi = n.dot(wi);
        float sin2thetai = std::max(0.f, 1.f - cosi*cosi);
        if (sin2thetai >= 1) return false;
        float cost = std::sqrt(1.f - eta*eta*sin2thetai);
        wt = -wi*eta + n*(eta*cosi-cost);
        return true;
    }
    /////////////////////////////////////////////////////////////////////////

    /// why have reflection?
    enum BxDFType {
        BSDF_REFLECTION = 1 << 0,
        BSDF_TRANSMISSION = 1 << 1,
        BSDF_DIFFUSE = 1 << 2,
        BSDF_GLOSSY =  1 << 3,
        BSDF_SPECULAR = 1 << 4,
        BSDF_ALL = BSDF_REFLECTION || BSDF_TRANSMISSION || BSDF_DIFFUSE || BSDF_GLOSSY || BSDF_SPECULAR,
    };

    enum TransportMode {
        RADIANCE, IMPORTANCE,
    };

    class BxDF {
    public:
		BxDF(BxDFType type);
        bool matchType(BxDFType t) const { return _type&t == _type; }  // match exactly

        virtual Vec3f f(const Vec3f &wo, const Vec3f &wi) const = 0;    // wo, wi are in local coord
		virtual Vec3f sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample/*TODO*/, float &pdf, BxDFType *sampled_type = nullptr) const { return Vec3f(0.f); }; // used for MC Integrators
		virtual float Pdf(const Vec3f &wo, const Vec3f &wi) const { return 1.f;/*TODO*/ }

        /// TODO: hemispherical reflection distribution
//        virtual rho();
//        virtual rhh()

        BxDFType  _type;
    };

    class ScaledBxDF: public BxDF {
    public:
        ScaledBxDF(std::shared_ptr<BxDF> bxdf, const Vec3f scale)
        : BxDF(bxdf->_type), _BxDF(bxdf), _scale(scale) {}

        Vec3f f(const Vec3f &wo, const Vec3f &wi) const override { return _scale * _BxDF->f(wo, wi); };
        Vec3f sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample/*TODO*/, float &pdf, BxDFType *sampled_type = nullptr) const override;
		float Pdf(const Vec3f &wo, const Vec3f &wi) const override;

    private:
        std::shared_ptr<BxDF> _BxDF;
        Vec3f _scale;
    };

    class BSDF {
    public:
		BSDF(IntersectInfo &isect_info, const float eta = 1.f);
		BSDF(const BSDF &origin);

        void add(std::unique_ptr<BxDF> bxdf) {
            _bxdfs.push_back(std::move(bxdf));
        }

		Vec3f world2local(const Vec3f &v) const {
			return Vec3f(
				_ts.dot(v),
				_bs.dot(v),
				_ns.dot(v)
			);
		}

		Vec3f local2world(const Vec3f &v) const {
			return _ts * v.x() + _bs*v.y() + _ns * v.z();
		}

        Vec3f f(const Vec3f &wo_w, const Vec3f &wi_w, BxDFType types=BSDF_ALL) const {
            Vec3f wi = world2local(wi_w);
			//std::cout << "wi_w(light ray): " << wi_w << std::endl;
			//std::cout << "wi: " <<wi << std::endl;
            Vec3f wo = world2local(wo_w);
            bool reflect = (_ng.dot(wi_w) * _ng.dot(wo_w)) > 0.f;
            Vec3f f(0.f);
            for (int i = 0; i < _bxdfs.size(); i++) {
                if (_bxdfs[i]->matchType(types) && (
                        (reflect && (_bxdfs[i]->_type&BSDF_REFLECTION)) || (!reflect &&(_bxdfs[i]->_type&BSDF_TRANSMISSION))
                        )) {
                    f += _bxdfs[i]->f(wo, wi);
                }
            }
            return f;
        }

		Vec3f sample_f(const Vec3f &wo_w, Vec3f &wi_w, float &pdf, const Point2f &u, BxDFType &sampled_types/*TODO*/) const {
			const Vec3f wo(world2local(wo_w));
			Vec3f wi;
			/// TODO: sample which bxdf to use
			// choose the first bxdf as sample for naive implement now
			Vec3f f = _bxdfs[0]->sample_f(wo, wi, u, pdf/*, type*/);
			wi_w = local2world(wi);
			return f;
		}

		float Pdf(const Vec3f &wo_w, const Vec3f &wi_w) const {
			return _bxdfs[0]->Pdf(world2local(wo_w), world2local(wi_w));
		}

		inline int numComponents(BxDFType flags) const {
			int n = 0;
			for (int i = 0; i < _bxdfs.size(); i++) {
				if (_bxdfs[i]->matchType(flags))
					n++;
			}
			return n;
		}

        /// TODO: rho()

    private:
        float _eta; // TODO: should store two eta?
        Vec3f _ng;  // store geometry normal to avoid problems caused by shading normals.
        Vec3f _ns, _ts, _bs;   // normal, tangent, bitangent
        std::vector<std::shared_ptr<BxDF>> _bxdfs;
    };
}
#endif //HOMURA_BSDF_H_
