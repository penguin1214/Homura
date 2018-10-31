#include "core/io/ObjHandler.h"

#include <string>
#include <cctype>
#include <sstream>

namespace Homura {
	ObjLoader::ObjLoader(std::ifstream &file) {
		std::string line;
		while (!file.eof()) {
			std::getline(file, line);
			parseLine(line.c_str());
		}
	}

	void ObjLoader::parseLine(const char *line) {
		/// TODO: .mtl prefix parse

		skipWhitespaces(line);
		if (hasPrefix(line, "v"))
			_verts.push_back(loadVec<float, 3>(line + 2));
		else if (hasPrefix(line, "f"))
			_indecies.push_back((loadVec<int, 3>(line + 2)-1));	// since .obj file index starts from 1, substitute 1 to make sure no access violation.
		else if (hasPrefix(line, "vn"))
			_normals.push_back(loadVec<float, 3>(line + 3));
		else
			return;
	}

	void ObjLoader::skipWhitespaces(const char *&cursor) {
		while (std::isspace(*cursor))
			cursor++;
	}

	bool ObjLoader::hasPrefix(const char *str, const char *pre) {
		do {
			if (*str != *pre)
				return false;
		} while (*(++str) && *(++pre));
		return (*pre == '\0') && (isspace(*str) || *str == '\0');
	}

	template<typename T, unsigned Size>
	Vector<T, Size> ObjLoader::loadVec(const char *str) {
		std::istringstream ss(str);
		Vector<T, Size> ret;
		for (int i = 0; i < Size && !ss.eof() && !ss.fail(); i++)
			ss >> ret[i];
		return ret;
	}

	bool ObjHandler::readObj(const char *fn, std::vector<Point3f> &vertices, std::vector<Vec3i> &indecies, std::vector<Vec3f> &normals) {
		std::ifstream file(fn, std::ios::binary);
		if (!file.good()) {
			std::cerr << "Open file " << fn << " failed!" << std::endl;
			return false;
		}
		
		ObjLoader loader(file);

		vertices = std::move(loader._verts);
		normals = std::move(loader._normals);
		indecies = std::move(loader._indecies);

		return true;
	}

	bool ObjHandler::writeObj(char *fn, std::vector<Point3f> vertices, std::vector<Vec3f> normals, std::vector<int> faceIndices) {
		std::ofstream file(fn, std::ios::out);
		if (!file.good()) {
			std::cout << "Cannot open " << fn << std::endl;
			return false;
		}

		int vert_num = vertices.size();
		int norm_num = normals.size();
		int face_num = faceIndices.size();
		if (vert_num > 0 && vert_num != norm_num) {
			std::cout << "number of vertices do not match number of normals" << std::endl;
			file.close();
			return false;
		}

		for (int i = 0; i < vert_num; i++) {
			char buf[2048];
			snprintf(buf, 2048, "v %.6f %.6f %.6f\n", vertices[i].x(), vertices[i].y(), vertices[i].z());
			file.write(buf, strlen(buf));
		}
		for (int i = 0; i < face_num; i++) {
			//sprintf_s(buf, "f %d %d %d\n", face_num.x)
		}

		for (int i = 0; i < norm_num; i++) {
			char buf[2048];
			snprintf(buf, 2048, "vn %.6f %.6f %.6f\n", normals[i].x(), normals[i].y(), normals[i].z());
		}
		file.close();
		std::cout << "Write file to " << fn << " done." << std::endl;
		return true;
	}
}