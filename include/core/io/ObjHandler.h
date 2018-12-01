#ifndef HOMURA_OBJ_HANDLER_H_
#define HOMURA_OBJ_HANDLER_H_

#include <vector>
#include <iostream>
#include <fstream>
#include "../../core/math/Geometry.h"

namespace Homura {
	class ObjLoader {
	public:
		ObjLoader(std::ifstream &file);

		std::vector<Point3f> _verts;
		std::vector<Vec3i> _indecies;
		std::vector<Vec3f> _normals;
		std::vector<Vec3i> _normal_indecies;

	private:
		void parseLine(const char *line);
		void skipWhitespaces(const char *&cursor);
		bool hasPrefix(const char *str, const char *pre);

		template<typename T, unsigned Size>
		Vector<T, Size> loadVec(const char *str);
	};

	class ObjHandler {
	public:
		ObjHandler() = default;

		static bool readObj(const char *fn, std::vector<Point3f> &vertices, std::vector<Vec3i> &indecies, std::vector<Vec3f> &normals, std::vector<Vec3i> &normal_indecies);
		static bool writeObj(char *fn, std::vector<Point3f> vertices, std::vector<Vec3f> normals, std::vector<int> faceIndices);
	};
}

#endif
