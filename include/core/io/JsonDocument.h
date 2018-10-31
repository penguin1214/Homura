#ifndef HOMURA_JSONDOCUMENT_H_
#define HOMURA_JSONDOCUMENT_H_

#include <string>
#include <rapidjson/document.h>
#include "JsonObject.h"
#include "core/io/FileUtil.h"

namespace Homura {
	class JsonDocument : public JsonObject {
	/*
	* JsonDocument inherits [JsonDocument *_document] from JsonObject.
	* JsonObject::_document points to JsonDocument::_document.
	*/
	private:
		std::string _json_str;
		rapidjson::Document _document;

		void loadDocument();
	public:
		JsonDocument(const std::string file_path)
		:	JsonObject(this, &_document), _json_str(FileUtil::loadFileString(file_path)) {
			loadDocument();
		}
	};
}

#endif // !HOMURA_JSONDOCUMENT_H_
