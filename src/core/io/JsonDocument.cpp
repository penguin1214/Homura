#include "core/io/JsonDocument.h"
#include <iostream>
#include <fstream>

#define JsonParseFlags rapidjson::kParseCommentsFlag | rapidjson::kParseTrailingCommasFlag | rapidjson::kParseNanAndInfFlag

namespace Homura {
	void JsonDocument::loadDocument() {
		_document.Parse<JsonParseFlags>(_json_str.c_str());
		if (_document.HasParseError()) {
			std::cerr << "Json Parse Error." << std::endl;
		}
	}
}