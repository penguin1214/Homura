#include "core/io/JsonObject.h"
#include "core/math/CoordinateSystem.h"

namespace Homura {
void JsonObject::fill(bool &dst) const {
	if (_value->IsBool())
		dst = _value->GetBool();
	else
		std::cerr << "JSON_PARSE_ERROR::BOOL_TYPE_NOT_MATCH" << std::endl;
}

template<typename T>
void getJsonNumber(const rapidjson::Value &v, T &dst) {
	if (v.IsInt())
		dst = v.GetInt();
	else if (v.IsUint())
		dst = v.GetUint();
	else if (v.IsDouble())
		dst = v.GetDouble();
	else
		std::cerr << "JSON_PARSE_ERROR::NUMBER_TYPE_NOT_EXIST" << std::endl;
}

Vec3f JsonObject::getVec3() const {
	if ((!_value->IsArray()) && (_value->Size() == 3)) {
		;	/// TODO: exception handler
	}
	else {
		return Vec3f(
			operator[](unsigned(0)).cast<float>(),
			operator[](unsigned(1)).cast<float>(),
			operator[](unsigned(2)).cast<float>());
	}
}

Mat4f JsonObject::getTransform() const {
	if (_value->IsArray()) {
		;	/// TODO
	}
	else {
		Vec3f T, S, R;
		getField("translate", T);
		getField("scale", S);
		//getField("rotate", R);
		/// TODO: rotation, how to represent

		return Mat4f::scale(S) * Mat4f::translate(T);
	}
}

void JsonObject::fill(int &dst) const {
	getJsonNumber(*_value, dst);
}

void JsonObject::fill(float &dst) const {
	getJsonNumber(*_value, dst);
}

void JsonObject::fill(double &dst) const {
	getJsonNumber(*_value, dst);
}

void JsonObject::fill(unsigned &dst) const {
	getJsonNumber(*_value, dst);
}

void JsonObject::fill(std::string &dst) const {
	if (_value->IsString())
		dst = _value->GetString();
	else
		std::cerr << "JSON_PARSE_ERROR::STRING_TYPE_ERROR" << std::endl;
}

void JsonObject::fill(Mat4f &dst) const {
	if (_value->IsArray()) {
		if (_value->Size() == 16) {
			for (unsigned i = 0; i < 16; i++)
				dst[i] = operator[](i).cast<float>();
		}
		else
			std::cerr << "JSON_PARSE_ERROR::MAT4F_SIZE_ERROR" << std::endl;
	}
	else {
		// from lookat to cam2world matrix
		Vec3f pos, lookat, up;
		getField("position", pos);
		getField("at", lookat);
		getField("up", up);
		/// TODO: comprehensive test
		CoordinateSystem frame(pos, lookat, up);
		dst = Mat4f(
			frame._right.x(), frame._right.y(), frame._right.z(), 0,
			frame._up.x(), frame._up.y(), frame._up.z(), 0,
			frame._forward.x(), frame._forward.y(), frame._forward.z(), 0,
			pos.x(), pos.y(), pos.z(), 1
		);
	}
}

}