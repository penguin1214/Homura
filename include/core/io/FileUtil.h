#ifndef HOMURA_FILEUTIL_H_
#define HOMURA_FILEUTIL_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "core/io/lodepng.h"

namespace Homura {
	class FileUtil {
	public:
		static bool is_empty(std::string file_path);
		static std::string loadFileString(std::string file_path);
	};

	/// TODO: as class function?
	template <typename T>
	std::vector<T> loadPNG(std::string path, int &width, int &height) {
		std::vector<unsigned char> img;
		unsigned w, h;
		unsigned error = lodepng::decode(img, w, h, path.c_str());
		if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		width = w, height = h;
		std::vector<T> rec;
		rec.resize(3 * w*h);
		int idx = 0;
		for (int i = 0; i < img.size(); i += 4) {
			rec[idx++] = (T)(img[i]);
			rec[idx++] = (T)(img[i + 1]);
			rec[idx++] = (T)(img[i + 2]);
		}
		return rec;
	}
}

#endif // !HOMURA_FILEUTIL_H_
