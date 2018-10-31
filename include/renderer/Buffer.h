#ifndef HOMURA_BUFFER_H_
#define HOMURA_BUFFER_H_

#include <vector>
#include "../core/math/Geometry.h"

namespace Homura {
	class Buffer {
	public:
        int _width;
        int _height;
		std::vector<Vec3f> _data;
//        Vec3f *_data;

		Buffer(int w, int h);
		~Buffer();

		void addSample(int index, Vec3f value);
	};
}

#endif // HOMURA_BUFFER_H_
