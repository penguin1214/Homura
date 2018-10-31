#include "core/sampler/Sampler.h"

namespace Homura {
	//Sampler::Sampler(int64_t spp) : _spp(spp) {}
	Sampler::Sampler(const JsonObject &json) : _spp(json["spp"].getInt()) {}

	Sampler::Sampler(const Sampler& origin) :
	_spp(origin._spp), _current_pixel(origin._current_pixel), _current_pixel_sample_idx(origin._current_pixel_sample_idx)/*TODO: need reset sample index? seems doesn't matter though*/ {
		/// TODO: sample array handle
	}

	/* Provide starting pixel coordinate for the Sampler instance.
	 * Reset _current_pixel_sample_idx.
	 */
	void Sampler::startPixel(const Vec2i &p) {
		_current_pixel = p;
		_current_pixel_sample_idx = 0;
	}

	/* Tells the Sampler instance subsequent requests for sample components should start from first dimension.
	 * Returns True if to sample next **pixel**.
	 */
	bool Sampler::startNextSample() {
		_array1DOffset = 0; _array2DOffset = 0;
		return ++_current_pixel_sample_idx < _spp;
	}

	bool Sampler::setSampleIdx(int64_t idx) {
		_array1DOffset = 0; _array2DOffset = 0;
		_current_pixel_sample_idx = idx;
		return _current_pixel_sample_idx < _spp;
	}

	int64_t Sampler::currentSampleIdx() const { return _current_pixel_sample_idx; }

	void Sampler::request1DArray(int n) {
		_samples1DArraySizes.push_back(n);
		_sampleArray1D.push_back(std::vector<float>(n*_spp));
	}

	void Sampler::request2DArray(int n) {
		_samples2DArraySizes.push_back(n);
		_sampleArray2D.push_back(std::vector<Vec2f>(n*_spp));
	}

	/* */
	const float* Sampler::get1DArray(int n) {
		if (_array1DOffset == _samples1DArraySizes.size()) return nullptr;
		return (&(_sampleArray1D[_array1DOffset++][_current_pixel_sample_idx*n]));
	}

	const Vec2f* Sampler::get2DArray(int n) {
		if (_array2DOffset == _samples2DArraySizes.size()) return nullptr;
		return (&(_sampleArray2D[_array2DOffset++][_current_pixel_sample_idx*n]));
	}

	PixelSample PixelSampler::getSensorSample(Vec2i p) {
		PixelSample cs;
		cs._p_film = Vec2f(p.x(), p.y()) + get2D();
		cs.time = get1D();
		cs._p_lens = get2D();
		return cs;
	}

	PixelSampler::PixelSampler(const JsonObject &json) : Sampler(json) {
		int ndim = json["ndim"].getInt();
		for (int i = 0; i < ndim; i++) {
			_samples1D.push_back(std::vector<float>(_spp));
			_samples2D.push_back(std::vector<Vec2f>(_spp));
		}
	}


	bool PixelSampler::startNextSample() {
		_current_1D_dim_idx = 0; _current_2D_dim_idx = 0;
		return Sampler::startNextSample();
	}

	bool PixelSampler::setSampleIdx(int64_t idx) {
		_current_1D_dim_idx = 0; _current_2D_dim_idx = 0;
		return Sampler::setSampleIdx(idx);
	}

	float PixelSampler::get1D() {
		if (_current_1D_dim_idx < _samples1D.size())
			return _samples1D[_current_1D_dim_idx++][_current_pixel_sample_idx];
		else
			return rng.UniformFloat();
	}

	Vec2f PixelSampler::get2D() {
		if (_current_2D_dim_idx < _samples2D.size())
			return _samples2D[_current_2D_dim_idx++][_current_pixel_sample_idx];
		else
			return Vec2f(rng.UniformFloat(), rng.UniformFloat());
	}

	PixelSampler::PixelSampler(const PixelSampler &origin) :
	Sampler(origin) {
		int ndim = origin._samples1D.size();
		for (int i = 0; i < ndim; i++) {
			_samples1D.push_back(std::vector<float>(_spp));
			_samples2D.push_back(std::vector<Vec2f>(_spp));
		}
	}


	void PixelSampler::resetSeed(const int &seed) {
		rng.SetSequence(seed);
	}

}