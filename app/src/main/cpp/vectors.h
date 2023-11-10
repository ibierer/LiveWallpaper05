#ifndef VECTORS_H
#define VECTORS_H

#include <math.h>
#include <string>

namespace std {
	class string {
	public:
		// Constructors
		string();  // Default constructor
		string(const char* str);  // Constructor from C-style string
		string(const string& other);  // Copy constructor

		// Destructor
		~string();

		// Assignment operators
		string& operator=(const char* str);  // Assignment from C-style string
		string& operator=(const string& other);  // Copy assignment

		// Member functions
		const char* c_str() const;  // Returns a pointer to the C-style string
		size_t length() const;  // Returns the length of the string

		// Other member functions would be implemented here...

		template <typename T>
		string to_string(T input);

	private:
		char* data;  // Pointer to the dynamically allocated character array
		size_t size;  // Length of the string
	};
}

template <class T>
class _vec2 {
public:
	union {
		T v[2];
		struct {
			T x;
			T y;
		};
		struct {
			T r;
			T g;
		};
	};
	T& operator[](int index) {
		return v[index];
	}
	_vec2() {

	}
	_vec2(const T input) {
		for (int i = 0; i < 2; i++) this->v[i] = input;
	}
	_vec2(const T x, const T y) {
		this->x = x; this->y = y;
	}
	_vec2(const T array[2]) {
		for (int i = 0; i < 2; i++) this->v[i] = array[i];
	}
	_vec2(const _vec2<unsigned int>& input) {
		for (int i = 0; i < 2; i++) this->v[i] = (T)input.v[i];
	}
	_vec2(const _vec2<int>& input) {
		for (int i = 0; i < 2; i++) this->v[i] = (T)input.v[i];
	}
	_vec2(const _vec2<float>& input) {
		for (int i = 0; i < 2; i++) this->v[i] = (T)input.v[i];
	}
	_vec2(const _vec2<double>& input) {
		for (int i = 0; i < 2; i++) this->v[i] = (T)input.v[i];
	}
	_vec2 operator+(_vec2 input) {
		_vec2 _return;
		for (int i = 0; i < 2; i++) _return[i] = this->v[i] + input[i];
		return _return;
	}
	_vec2& operator+=(_vec2 input) {
		for (int i = 0; i < 2; i++) v[i] += input[i];
		return *this;
	}
	_vec2 operator-(_vec2 input) {
		_vec2 _return;
		for (int i = 0; i < 2; i++) _return[i] = v[i] - input[i];
		return _return;
	}
	_vec2& operator-=(_vec2 input) {
		for (int i = 0; i < 2; i++) v[i] -= input[i];
		return *this;
	}
	_vec2 operator-() {
		_vec2 _return;
		for (int i = 0; i < 2; i++) _return[i] = -v[i];
		return _return;
	}
	_vec2 operator*(_vec2 input) {
		_vec2 _return;
		for (int i = 0; i < 2; i++) _return[i] = v[i] * input[i];
		return _return;
	}
	_vec2& operator*=(_vec2 input) {
		for (int i = 0; i < 2; i++) v[i] *= input[i];
		return *this;
	}
	_vec2 operator*(T scalar) {
		_vec2 _return;
		for (int i = 0; i < 2; i++) _return[i] = v[i] * scalar;
		return _return;
	}
	_vec2& operator*=(T scalar) {
		for (int i = 0; i < 2; i++) v[i] *= scalar;
		return *this;
	}
	_vec2 operator/(_vec2 input) {
		_vec2 _return;
		for (int i = 0; i < 2; i++) _return[i] = v[i] / input[i];
		return _return;
	}
	_vec2& operator/=(_vec2 input) {
		for (int i = 0; i < 2; i++) v[i] /= input[i];
		return *this;
	}
	_vec2 operator/(T input) {
		_vec2 _return;
		for (int i = 0; i < 2; i++) _return[i] = v[i] / input;
		return _return;
	}
	_vec2& operator/=(T input) {
		for (int i = 0; i < 2; i++) v[i] /= input;
		return *this;
	}
	_vec2& operator=(_vec2<unsigned int> rhs) {
		for (int i = 0; i < 2; i++) v[i] = (T)rhs.v[i];
		return *this;
	}
	_vec2& operator=(_vec2<int> rhs) {
		for (int i = 0; i < 2; i++) v[i] = (T)rhs.v[i];
		return *this;
	}
	_vec2& operator=(_vec2<float> rhs) {
		for (int i = 0; i < 2; i++) v[i] = (T)rhs.v[i];
		return *this;
	}
	bool operator==(_vec2 rhs) {
		return rhs.x == x && rhs.y == y;
	}
	bool operator!=(_vec2 rhs) {
		return rhs.x != x || rhs.y != y;
	}
	std::string str(){
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
	}
};

template <typename T>
std::string std::to_string(_vec2<T> input) {
    return input.str();
}

template <typename T>
_vec2<T> operator*(T lhs, _vec2<T> rhs) {
	_vec2<T> _return;
	for (int i = 0; i < 2; i++) _return[i] = lhs * (T)rhs.v[i];
	return _return;
}

_vec2<float> operator*(float lhs, _vec2<int> rhs) {
	_vec2<float> _return;
	for (int i = 0; i < 2; i++) _return[i] = lhs * (float)rhs.v[i];
	return _return;
}
_vec2<float> operator*(_vec2<int> lhs, float rhs) {
	return rhs * lhs;
}
_vec2<float> operator*(int lhs, _vec2<float> rhs) {
	_vec2<float> _return;
	for (int i = 0; i < 2; i++) _return[i] = (float)lhs * rhs.v[i];
	return _return;
}
_vec2<float> operator*(_vec2<float> lhs, int rhs) {
	return rhs * lhs;
}

_vec2<double> operator*(double lhs, _vec2<float> rhs) {
	_vec2<double> _return;
	for (int i = 0; i < 2; i++) _return[i] = lhs * (double)rhs.v[i];
	return _return;
}
_vec2<double> operator*(_vec2<float> lhs, double rhs) {
	return rhs * lhs;
}
_vec2<double> operator*(float lhs, _vec2<double> rhs) {
	_vec2<double> _return;
	for (int i = 0; i < 2; i++) _return[i] = (double)lhs * rhs.v[i];
	return _return;
}
_vec2<double> operator*(_vec2<double> lhs, float rhs) {
	return rhs * lhs;
}

template <class T>
class _vec3 {
public:
	union {
		T v[3];
		_vec2<T> xy;
		struct {
			T x;
			T y;
			T z;
		};
		struct {
			T r;
			T g;
			T b;
		};
	};
	T& operator[](int index) {
		return v[index];
	}
	_vec3() {

	}
	_vec3(const T input) {
		for (int i = 0; i < 3; i++) this->v[i] = input;
	}
	_vec3(const T x, const T y, const T z) {
		this->x = x; this->y = y; this->z = z;
	}
	_vec3(const T array[3]) {
		for (int i = 0; i < 3; i++) this->v[i] = array[i];
	}
	_vec3(const _vec2<T>& input, const T& z) {//Should try reducing to just one loop of 3 iterations
		for (int i = 0; i < 2; i++) this->v[i] = input[i]; this->v[2] = z;
	}
	_vec3(const _vec3<unsigned int>& input) {
		for (int i = 0; i < 3; i++) this->v[i] = (T)input.v[i];
	}
	_vec3(const _vec3<int>& input) {
		for (int i = 0; i < 3; i++) this->v[i] = (T)input.v[i];
	}
	_vec3(const _vec3<float>& input) {
		for (int i = 0; i < 3; i++) this->v[i] = (T)input.v[i];
	}
	_vec3(const _vec3<double>& input) {
		for (int i = 0; i < 3; i++) this->v[i] = (T)input.v[i];
	}
	_vec3 operator+(_vec3 input) {
		_vec3 _return;
		for (int i = 0; i < 3; i++) _return[i] = v[i] + input[i];
		return _return;
	}
	_vec3& operator+=(_vec3 input) {
		for (int i = 0; i < 3; i++) v[i] += input[i];
		return *this;
	}
	_vec3 operator-(_vec3 input) {
		_vec3 _return;
		for (int i = 0; i < 3; i++) _return[i] = v[i] - input[i];
		return _return;
	}
	_vec3& operator-=(_vec3 input) {
		for (int i = 0; i < 3; i++) v[i] -= input[i];
		return *this;
	}
	_vec3 operator-() {
		_vec3 _return;
		for (int i = 0; i < 3; i++) _return[i] = -v[i];
		return _return;
	}
	_vec3 operator*(_vec3 input) {
		_vec3 _return;
		for (int i = 0; i < 3; i++) _return[i] = v[i] * input[i];
		return _return;
	}
	_vec3& operator*=(_vec3 input) {
		for (int i = 0; i < 3; i++) v[i] *= input[i];
		return *this;
	}
	_vec3 operator*(T scalar) {
		_vec3 _return;
		for (int i = 0; i < 3; i++) _return[i] = v[i] * scalar;
		return _return;
	}
	_vec3& operator*=(T scalar) {
		for (int i = 0; i < 3; i++) v[i] *= scalar;
		return *this;
	}
	_vec3 operator/(_vec3 input) {
		_vec3 _return;
		for (int i = 0; i < 3; i++) _return[i] = v[i] / input[i];
		return _return;
	}
	_vec3& operator/=(_vec3 input) {
		for (int i = 0; i < 3; i++) v[i] /= input[i];
		return *this;
	}
	_vec3 operator/(T input) {
		_vec3 _return;
		for (int i = 0; i < 3; i++) _return[i] = v[i] / input;
		return _return;
	}
	_vec3& operator/=(T input) {
		for (int i = 0; i < 3; i++) v[i] /= input;
		return *this;
	}
	_vec3& operator=(_vec3<unsigned int> rhs) {
		for (int i = 0; i < 3; i++) v[i] = (T)rhs.v[i];
		return *this;
	}
	_vec3& operator=(_vec3<int> rhs) {
		for (int i = 0; i < 3; i++) v[i] = (T)rhs.v[i];
		return *this;
	}
	_vec3& operator=(_vec3<float> rhs) {
		for (int i = 0; i < 3; i++) v[i] = (T)rhs.v[i];
		return *this;
	}
	bool operator==(_vec3 rhs) {
		return rhs.x == x && rhs.y == y && rhs.z == z;
	}
	bool operator!=(_vec3 rhs) {
		return rhs.x != x || rhs.y != y || rhs.z != z;
	}
	std::string str(){
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
	}
};

template <typename T>
std::string std::to_string(_vec3<T> input) {
    return input.str();
}

template <typename T>
_vec3<T> operator*(T lhs, _vec3<T> rhs) {
	_vec3<T> _return;
	for (int i = 0; i < 3; i++) _return[i] = lhs * (T)rhs.v[i];
	return _return;
}

_vec3<float> operator*(float lhs, _vec3<int> rhs) {
	_vec3<float> _return;
	for (int i = 0; i < 3; i++) _return[i] = lhs * (float)rhs.v[i];
	return _return;
}
_vec3<float> operator*(_vec3<int> lhs, float rhs) {
	return rhs * lhs;
}
_vec3<float> operator*(int lhs, _vec3<float> rhs) {
	_vec3<float> _return;
	for (int i = 0; i < 3; i++) _return[i] = (float)lhs * rhs.v[i];
	return _return;
}
_vec3<float> operator*(_vec3<float> lhs, int rhs) {
	return rhs * lhs;
}

_vec3<double> operator*(double lhs, _vec3<float> rhs) {
	_vec3<double> _return;
	for (int i = 0; i < 3; i++) _return[i] = lhs * (double)rhs.v[i];
	return _return;
}
_vec3<double> operator*(_vec3<float> lhs, double rhs) {
	return rhs * lhs;
}
_vec3<double> operator*(float lhs, _vec3<double> rhs) {
	_vec3<double> _return;
	for (int i = 0; i < 3; i++) _return[i] = (double)lhs * rhs.v[i];
	return _return;
}
_vec3<double> operator*(_vec3<double> lhs, float rhs) {
	return rhs * lhs;
}

template <class T>
class _vec4 {
public:
	union {
		T v[4];
		_vec2<T> xy;
		_vec3<T> xyz;
		struct {
			T x;
			T y;
			T z;
			T w;
		};
		struct {
			T r;
			T g;
			T b;
			T a;
		};
	};
	T& operator[](int index) {
		return v[index];
	}
	_vec4() {

	}
	_vec4(const T input) {
		for (int i = 0; i < 4; i++) this->v[i] = input;
	}
	_vec4(const T x, const T y, const T z, const T w) {
		this->x = x; this->y = y; this->z = z; this->w = w;
	}
	_vec4(const T array[4]) {
		for (int i = 0; i < 4; i++) this->v[i] = array[i];
	}
	_vec4(const _vec3<T>& input, const T& z) {//Should try reducing to just one loop of 4 iterations
		for (int i = 0; i < 3; i++) this->v[i] = (T)input.v[i]; this->v[3] = (T)z;
	}
	_vec4(const _vec4<unsigned int>& input) {
		for (int i = 0; i < 4; i++) this->v[i] = (T)input.v[i];
	}
	_vec4(const _vec4<int>& input) {
		for (int i = 0; i < 4; i++) this->v[i] = (T)input.v[i];
	}
	_vec4(const _vec4<float>& input) {
		for (int i = 0; i < 4; i++) this->v[i] = (T)input.v[i];
	}
	_vec4(const _vec4<double>& input) {
		for (int i = 0; i < 4; i++) this->v[i] = (T)input.v[i];
	}
	_vec4 operator+(_vec4 input) {
		_vec4 _return;
		for (int i = 0; i < 4; i++) _return[i] = v[i] + input[i];
		return _return;
	}
	_vec4& operator+=(_vec4 input) {
		for (int i = 0; i < 4; i++) v[i] += input[i];
		return *this;
	}
	_vec4 operator-(_vec4 input) {
		_vec4 _return;
		for (int i = 0; i < 4; i++) _return[i] = v[i] - input[i];
		return _return;
	}
	_vec4& operator-=(_vec4 input) {
		for (int i = 0; i < 4; i++) v[i] -= input[i];
		return *this;
	}
	_vec4 operator-() {
		_vec4 _return;
		for (int i = 0; i < 4; i++) _return[i] = -v[i];
		return _return;
	}
	_vec4 operator*(_vec4 input) {
		_vec4 _return;
		for (int i = 0; i < 4; i++) _return[i] = v[i] * input[i];
		return _return;
	}
	_vec4& operator*=(_vec4 input) {
		for (int i = 0; i < 4; i++) v[i] *= input[i];
		return *this;
	}
	_vec4 operator*(T scalar) {
		_vec4 _return;
		for (int i = 0; i < 4; i++) _return[i] = v[i] * scalar;
		return _return;
	}
	_vec4& operator*=(T scalar) {
		for (int i = 0; i < 4; i++) v[i] *= scalar;
		return *this;
	}
	_vec4 operator/(_vec4 input) {
		_vec4 _return;
		for (int i = 0; i < 4; i++) _return[i] = v[i] / input[i];
		return _return;
	}
	_vec4& operator/=(_vec4 input) {
		for (int i = 0; i < 4; i++) v[i] /= input[i];
		return *this;
	}
	_vec4 operator/(T input) {
		_vec4 _return;
		for (int i = 0; i < 4; i++) _return[i] = v[i] / input;
		return _return;
	}
	_vec4& operator/=(T input) {
		for (int i = 0; i < 4; i++) v[i] /= input;
		return *this;
	}
	_vec4& operator=(_vec4<unsigned int> rhs) {
		for (int i = 0; i < 4; i++) v[i] = (T)rhs.v[i];
		return *this;
	}
	_vec4& operator=(_vec4<int> rhs) {
		for (int i = 0; i < 4; i++) v[i] = (T)rhs.v[i];
		return *this;
	}
	_vec4& operator=(_vec4<float> rhs) {
		for (int i = 0; i < 4; i++) v[i] = (T)rhs.v[i];
		return *this;
	}
	bool operator==(_vec4 rhs) {
		return rhs.x == x && rhs.y == y && rhs.z == z && rhs.w == w;
	}
	bool operator!=(_vec4 rhs) {
		return rhs.x != x || rhs.y != y || rhs.z != z || rhs.w != w;
	}
	std::string str(){
		return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ", " + std::to_string(w) + ")";
	}
};

template <typename T>
std::string std::to_string(_vec4<T> input) {
    return input.str();
}

template <typename T>
_vec4<T> operator*(T lhs, _vec4<T> rhs) {
	_vec4<T> _return;
	for (int i = 0; i < 4; i++) _return[i] = lhs * (T)rhs.v[i];
	return _return;
}

_vec4<float> operator*(float lhs, _vec4<int> rhs) {
	_vec4<float> _return;
	for (int i = 0; i < 4; i++) _return[i] = lhs * (float)rhs.v[i];
	return _return;
}
_vec4<float> operator*(_vec4<int> lhs, float rhs) {
	return rhs * lhs;
}
_vec4<float> operator*(int lhs, _vec4<float> rhs) {
	_vec4<float> _return;
	for (int i = 0; i < 4; i++) _return[i] = (float)lhs * rhs.v[i];
	return _return;
}
_vec4<float> operator*(_vec4<float> lhs, int rhs) {
	return rhs * lhs;
}

_vec4<double> operator*(double lhs, _vec4<float> rhs) {
	_vec4<double> _return;
	for (int i = 0; i < 4; i++) _return[i] = lhs * (double)rhs.v[i];
	return _return;
}
_vec4<double> operator*(_vec4<float> lhs, double rhs) {
	return rhs * lhs;
}
_vec4<double> operator*(float lhs, _vec4<double> rhs) {
	_vec4<double> _return;
	for (int i = 0; i < 4; i++) _return[i] = (double)lhs * rhs.v[i];
	return _return;
}
_vec4<double> operator*(_vec4<double> lhs, float rhs) {
	return rhs * lhs;
}

typedef _vec2<float> vec2;
typedef _vec2<unsigned int> uvec2;
typedef _vec2<int> ivec2;
typedef _vec2<unsigned char> ucvec2;

typedef _vec3<float> vec3;
typedef _vec3<unsigned int> uvec3;
typedef _vec3<int> ivec3;
typedef _vec3<unsigned char> ucvec3;

typedef _vec4<float> vec4;
typedef _vec4<unsigned int> uvec4;
typedef _vec4<int> ivec4;
typedef _vec4<unsigned char> ucvec4;

vec3 cross(vec3 left, vec3 right) {
	return vec3(
		left.y * right.z - left.z * right.y,
		-left.x * right.z + left.z * right.x,
		left.x * right.y - left.y * right.x
	);
}
float dot(vec2 left, vec2 right) {
	vec2 product = left * right;
	return product.x + product.y;
}
float dot(vec3 left, vec3 right) {
	vec3 product = left * right;
	return product.x + product.y + product.z;
}
float dot(vec4 left, vec4 right) {
	vec4 product = left * right;
	return product.x + product.y + product.z + product.w;
}
float distance(vec2 coordinates) {
	return sqrt(dot(coordinates, coordinates));
}
float distance(vec3 coordinates) {
	return sqrt(dot(coordinates, coordinates));
}
float distance(vec2 coordinates1, vec2 coordinates2) {
	return distance(coordinates2 - coordinates1);
}
float distance(vec3 coordinates1, vec3 coordinates2) {
	return distance(coordinates2 - coordinates1);
}
float length(vec2 coordinates) {
	return distance(coordinates);
}
float length(vec3 coordinates) {
	return distance(coordinates);
}
template <class T>
void clamp(int vectorDimensionCount, T* x, T* minVal, T* maxVal) {
	for (int i = 0; i < vectorDimensionCount; i++) {
		if (x[i] < minVal[i])
			x[i] = minVal[i];
		if (x[i] > maxVal[i])
			x[i] = maxVal[i];
	}
}
float clamp(float x, float minVal, float maxVal) {
	clamp<float>(1, (float*)&x, (float*)&minVal, (float*)&maxVal);
	return x;
}
vec2 clamp(vec2 x, vec2 minVal, vec2 maxVal) {
	clamp<float>(2, (float*)&x, (float*)&minVal, (float*)&maxVal);
	return x;
}
vec3 clamp(vec3 x, vec3 minVal, vec3 maxVal) {
	clamp<float>(3, (float*)&x, (float*)&minVal, (float*)&maxVal);
	return x;
}
vec4 clamp(vec4 x, vec4 minVal, vec4 maxVal) {
	clamp<float>(4, (float*)&x, (float*)&minVal, (float*)&maxVal);
	return x;
}


float distance(const float x, const float y) {
	return sqrt(x * x + y * y);
}
float distance(const float x, const float y, const float z) {
	return sqrt(x * x + y * y + z * z);
}
float toDegrees(const float radians) {
	return radians * 180.0f / M_PI;
}
float toRadians(const float degrees) {
	return degrees * M_PI / 180.0f;
}
vec2 normalize(vec2 input) {
	return input / distance(input);
}
vec3 normalize(vec3 input) {
	return input / distance(input);
}

class mat3 {
public:
	vec3 m[3];
	mat3() {

	}
	mat3(vec3 a, vec3 b, vec3 c) {
		m[0] = a;
		m[1] = b;
		m[2] = c;
	}
	vec3 operator*(vec3 input) {
		vec3 _return;
		for (int i = 0; i < 3; i++) {
			_return[i] = dot(m[i], input);
		}
		return _return;
	}
};

mat3 quaternionTo3x3(vec4 input) {
	float thetaOver2 = acosf(input[3]);
	float sinOfHalfTheta = sinf(thetaOver2);
	float x = input[0] / sinOfHalfTheta;
	float y = input[1] / sinOfHalfTheta;
	float z = input[2] / sinOfHalfTheta;
	float theta = 2.0f * thetaOver2;
	return mat3(
		vec3(x * x * (1 - cosf(theta)) + cosf(theta), x * y * (1 - cosf(theta)) - z * sinf(theta), x * z * (1 - cosf(theta)) + y * sinf(theta)),
		vec3(x * y * (1 - cosf(theta)) + z * sinf(theta), y * y * (1 - cosf(theta)) + cosf(theta), y * z * (1 - cosf(theta)) - x * sinf(theta)),
		vec3(x * z * (1 - cosf(theta)) - y * sinf(theta), y * z * (1 - cosf(theta)) + x * sinf(theta), z * z * (1 - cosf(theta)) + cosf(theta))
	);
}

#endif