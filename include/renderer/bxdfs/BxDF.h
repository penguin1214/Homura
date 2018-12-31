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
		float sin2thetat = eta * eta*sin2thetai;
        if (sin2thetat >= 1) return false;
        float cost = std::sqrt(1.f - sin2thetat);
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
        BSDF_ALL = BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR,
    };

    enum TransportMode :short {
        RADIANCE, IMPORTANCE,
    };

    class BxDF {
    public:
		BxDF(BxDFType type, std::string name);
		bool matchType(BxDFType t) const { return (_type & t) == _type; }

		virtual void prepareForRender(const IntersectInfo &isect_info) {}

		virtual Vec3f f(const Vec3f &wo, const Vec3f &wi) const { return Vec3f(0.f); }    // wo, wi are in local coord
		virtual Vec3f sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample, float &pdf, BxDFType *sampled_type = nullptr, const TransportMode *mode = nullptr) const { return Vec3f(0.f); }
		virtual float Pdf(const Vec3f &wo, const Vec3f &wi) const { return 0.f; }

        /// TODO: hemispherical reflection distribution
//        virtual rho();
//        virtual rhh()

        BxDFType  _type;
		std::string _name;
    };

    class ScaledBxDF: public BxDF {
    public:
        ScaledBxDF(std::shared_ptr<BxDF> bxdf, const Vec3f scale, std::string name)
        : BxDF(bxdf->_type, name), _BxDF(bxdf), _scale(scale) {}

        Vec3f f(const Vec3f &wo, const Vec3f &wi) const override { return _scale * _BxDF->f(wo, wi); };
        Vec3f sample_f(const Vec3f &wo, Vec3f &wi, const Point2f &sample/*TODO*/, float &pdf, BxDFType *sampled_type = nullptr, const TransportMode *mode = nullptr) const override;
		float Pdf(const Vec3f &wo, const Vec3f &wi) const override;

    private:
        std::shared_ptr<BxDF> _BxDF;
        Vec3f _scale;
    };

    class BSDF {
    public:
		//BSDF(IntersectInfo &isect_info, const float eta = 1.f);
		BSDF() = default;
		//BSDF(const float eta = 1.f) {}
		BSDF(const BSDF &origin);

        void add(std::shared_ptr<BxDF> bxdf) {
            _bxdfs.push_back(bxdf);
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

		void prepareForRender(const IntersectInfo &isect_info) {
			_ng = isect_info._normal;
			_ns = isect_info._shading._n;
			_ts = isect_info._shading._tangent;
			_bs = isect_info._shading._bitangent;
			_transport_mode = isect_info._transport_mode;

			//for (auto &bxdf : _bxdfs)
			//	bxdf->prepareForRender(isect_info);
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

		Vec3f sample_f(const Vec3f &wo_w, Vec3f &wi_w, float &pdf, const float u0, const Point2f &u, const BxDFType &expected_types, BxDFType *sampled_types) const {
			// choose bxdf
			int n_matching_bxdf = numComponents(expected_types);
			if (n_matching_bxdf == 0) {
				pdf = 0;	// TODO: will cause divide error?
				*sampled_types = BxDFType(0);
				return Vec3f(0.f);
			}

			int sampled_idx = std::floor(u0 * n_matching_bxdf);
			std::shared_ptr<BxDF> sampled_bxdf = nullptr;

			int cnt = sampled_idx;
			for (int i = 0; i < _bxdfs.size(); i++) {
				if (_bxdfs[i]->matchType(expected_types) && (cnt-- == 0))
					sampled_bxdf = _bxdfs[i];
			}

			*sampled_types = sampled_bxdf->_type;

			const Vec3f wo(world2local(wo_w));
			Vec3f wi;
			Vec3f f = sampled_bxdf->sample_f(wo, wi, u, pdf, sampled_types, &_transport_mode);
			wi_w = local2world(wi);

			//std::cout << "frame: " << std::endl;
			//std::cout << _ns << std::endl;
			//std::cout << _ts << std::endl;
			//std::cout << _bs << std::endl;
			//std::cout << "sampled direction(local): " << wi << std::endl;
			//std::cout << "sampled direction: " << wi_w << std::endl;

			// use average pdf
			if (!(sampled_bxdf->_type & BSDF_SPECULAR) && n_matching_bxdf > 1) {
				for (int i = 0; i < _bxdfs.size(); i++) {
					if (sampled_bxdf != _bxdfs[i] && _bxdfs[i]->matchType(expected_types))
						pdf += _bxdfs[i]->Pdf(wo, wi);
				}
			}
			if (n_matching_bxdf > 1) pdf /= n_matching_bxdf;

			// also use average f
			if (!(sampled_bxdf->_type & BSDF_SPECULAR) && n_matching_bxdf > 1) {
				f = 0.f;
				bool reflect = (wo.z() * wi.z()) > 0;
				for (int i = 0; i < _bxdfs.size(); i++) {
					if (_bxdfs[i]->matchType(expected_types) &&
						(reflect && _bxdfs[i]->_type&BSDF_REFLECTION) || ((!reflect) && _bxdfs[i]->_type&BSDF_TRANSMISSION))
						f += _bxdfs[i]->f(wo, wi);
				}
			}

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

		bool isValid() { return _bxdfs.size() > 0; }

        /// TODO: rho()

    private:
		//float _eta;
        Vec3f _ng;  // store geometry normal to avoid problems caused by shading normals.
        Vec3f _ns, _ts, _bs;   // normal, tangent, bitangent
		TransportMode _transport_mode;
        std::vector<std::shared_ptr<BxDF>> _bxdfs;
    };
}
#endif //HOMURA_BSDF_H_
