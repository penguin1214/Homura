#ifndef HOMURA_JSONOBJECT_H_
#define HOMURA_JSONOBJECT_H_
#include <rapidjson/document.h>
#include <iostream>
#include "core/math/Mat4f.h"

namespace Homura {

class JsonDocument;

class JsonObject {
	/*
	* JsonObject holds basic json objects, at any hierarchy level.
	*/
protected:
	const JsonDocument *_document;
	const rapidjson::Value *_value;

	JsonObject(const JsonDocument *doc, const rapidjson::Value *vl) : _document(doc), _value(vl) {}

	template<typename T>
	T cast() const {
		T t;
		fill(t);
		return t;
	}

public:
	JsonObject() : JsonObject(nullptr, nullptr) {}

	void fill(bool &dst) const;
	void fill(int &dst) const;
	void fill(float &dst) const;
	void fill(double &dst) const;
	void fill(unsigned &dst) const;
	void fill(std::string &dst) const;
	void fill(Mat4f &dst) const;

	template<typename ElementType, unsigned Size>
	void fill(Vector<ElementType, Size> &dst) const {
		if (!_value->IsArray()) {
			// single value initiate
			dst = Vector<ElementType, Size>(cast<ElementType>());
		}
		else {
			if (_value->Size() != Size)
				std::cerr << "JSON_PARSE_ERROR::ARRAY_SIZE_NOT_MATCH" << std::endl;

			for (unsigned i = 0; i < Size; i++)
				dst[i] = (*this)[i].cast<ElementType>();
		}
	}

	std::string getString() const {
		if (!_value->IsString())
			return std::string();
		else
			return _value->GetString();
	}

	int getInt() const { return _value->GetInt(); }
	bool getBool() const { return _value->GetBool(); }
	float getFloat() const { return _value->GetFloat(); }
	Vec3f getVec3() const;
	Mat4f getTransform() const;
	template<typename T>
	std::vector<T> getVector() const {
		std::vector<T> ret;
		if (!_value->IsArray())
			;
		else {
			for (unsigned i = 0; i < size(); i++)
				ret.push_back(operator[](i).cast<T>());
		}
		return ret;
	}

	JsonObject operator[](const char *field) const {
		if (!_value->IsObject())
			return JsonObject();

		auto object = _value->FindMember(field);
		if (object == _value->MemberEnd())
			return JsonObject();

		return JsonObject(_document, &object->value);
	}

	JsonObject operator[](unsigned i) const {
		if (!_value->IsArray())
			return JsonObject();
		return JsonObject(_document, &(*_value)[i]);
	}

	template<typename T>
	bool getField(const char* field, T &dst) const {
		/* getField() is used to assign an object, while operator[] is used to only return the found JsonObject. */
		if (auto object = this->operator [](field)) {
			object.fill(dst);
			return true;
		}
		return false;
	}

	unsigned size() const { return _value->Size(); }
	operator bool() const { return _value != nullptr; }
};

}

#endif // !HOMURA_JSONOBJECT_H_
