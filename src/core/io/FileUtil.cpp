#include "core/io/FileUtil.h"

namespace Homura {
	bool FileUtil::is_empty(std::string file_path) {
		std::ifstream f(file_path);
		if (!f.is_open()) {
			/// TODO: exception handle
			std::cerr << "File not opened!" << std::endl;
			return true;
		}
		if (f.peek() == std::ifstream::traits_type::eof()) {
			/// TODO: exception handle
			std::cerr << "File is empty!" << std::endl;
			return true;
		}
		return false;
	}

	std::string FileUtil::loadFileString(std::string file_path) {
		if (FileUtil::is_empty(file_path)) {
			std::cerr << "File load failed." << std::endl;
		}
		else {
			std::ifstream ifs(file_path);
			std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
			return content;
		}
	}
}