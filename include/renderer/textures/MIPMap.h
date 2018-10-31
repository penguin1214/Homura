#ifndef HOMURA_MIPMAP_H_
#define HOMURA_MIPMAP_H_

#include "core/math/Geometry.h"
#include "core/io/FileUtil.h"
#include "core/io/JsonObject.h"
#include <vector>

namespace Homura {
	enum ImageWrapMode {
		REPEAT, BLACK/*, CLAMP*/
	};

	template <typename T, unsigned Size>
	class MIPMap {
		/*Do trilinear now*/
	public:
		MIPMap(std::string filepath, int wmode);

		int width() const { return _res[0]; }
		int height() const { return _res[1]; }
		int levels() const { return _pyramid.size(); }

		Vector<T, Size> texel(int level, int s, int t) const;
		Vector<T, Size> lookup(const Point2f &st) const;
		//T lookup(const Point2f &st, float width) const;

	private:
		const ImageWrapMode _wrap_mode;
		Point2i _res;
		std::vector<std::unique_ptr<std::vector<Vector<T, Size>>>> _pyramid;	// T is usually Vec3i?
	};

	template <typename T, unsigned Size>
	MIPMap<T, Size>::MIPMap(std::string filepath, int wmode) :
	_wrap_mode(static_cast<ImageWrapMode>(wmode)) {
		/// TODO: handle non-powerof2 resolution, resample haven't implemented yet.
		/// TODO: handle rectangular images, now only offers square images.
		int width, height;
		std::vector<T> v = loadPNG<T>(filepath, _res[0], _res[1]);
		std::vector<Vector<T, Size>> img = extractVector<T, Size>(v);

		int n_levels = 1 + int(std::log2(std::max(_res[0], _res[1])));
		_pyramid.resize(n_levels);

		_pyramid[0].reset(new std::vector<Vector<T, Size>>(img));

		for (int i = 1; i < n_levels; i++) {
			int l_res = _res[0] / (2 * i);
			_pyramid[i].reset(
				new std::vector<Vector<T, Size>>(l_res*l_res)
			);

			int idx = 0;
			for (int t = 0; t < l_res; t++) {
				for (int s = 0; s < l_res; s++) {
					(*_pyramid[i])[idx++] = (
						texel(i - 1, 2 * s, 2 * t) +
						texel(i - 1, 2 * s + 1, 2 * t) +
						texel(i - 1, 2 * s, 2 * t + 1) +
						texel(i - 1, 2 * s + 1, 2 * t + 1)
						) * .25f;
				}
			}
		}
	}

	template <typename T, unsigned Size>
	Vector<T, Size> MIPMap<T, Size>::texel(int level, int s, int t) const {
		const std::vector<Vector<T, Size>> &l = *_pyramid[level];
		const int sz = std::sqrt(l.size());
		switch (_wrap_mode) {
		case ImageWrapMode::REPEAT:
			s = s % sz;
			t = t % sz;
			break;
		//case ImageWrapMode::CLAMP:
		case ImageWrapMode::BLACK:
			if (s < 0 || s >= sz || t < 0 || t >= sz) {
				return 0.f;
			}
			break;
		}
		return l[t*sz + s];
	}

	template <typename T, unsigned Size>
	Vector<T, Size> MIPMap<T, Size>::lookup(const Point2f &st) const {
		return texel(levels() - 1, std::floor(st.x()-0.5f), std::floor(st.y()-0.5f));
	}

	/*template <typename T>
	T MIPMap<T>::lookup(const Point2f &st, float width) const {
		float level = levels() - 1 + std::log2(std::max(width, 1e-8));
		if (level < 0)
			return triangle(0, st);
		else if (level > levels() - 1)
			return texel(levels() - 1, 0, 0);
		else {
			int l = std::floor(level);
			float delta = level - l;
			return Lerp(delta, triangle(l, st), triangle(l + 1, st));
		}
	}*/
}

#endif // !HOMURA_MIPMAP_H_
