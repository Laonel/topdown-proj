#ifndef _UTILS_H_
#define _UTILS_H_

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <memory>
#include <numeric>
#include <complex>
#include <iterator>

#include <boost/filesystem.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <Box2D/Box2D.h>

//! MACROS
#define ERROR_AND_ABORT(msg) Exception(msg, __FILE__, __LINE__);

#define PRINT_DEBUG std::cout << "DEBUG:" << __FILE__ << ":" << __LINE__ << ": "
#define PRINT_WARNING std::cerr << "WARNING:" << __FILE__ << ":" << __LINE__ << ": "
#define PRINT_ERROR std::cerr << "ERROR:" << __FILE__ << ":" << __LINE__ << ": "

#define IF_PRINT_DEBUG(var) if (var) std::cout << "DEBUG:" << __FILE__ << ":" << __LINE__ << ": "
#define IF_PRINT_WARNING(var) if (var) std::cerr << "WARNING:" << __FILE__ << ":" << __LINE__ << ": "

#define DELETE_OBJECT(o){ if (o) { delete o; o = 0; } }
#define DELETE_ARRAY(a)	{ if (a) { delete[] a; a = 0; } }

#define PTR_TYPEID(ptr) typeid(*ptr)

//! Typedefs for integer types
typedef int32_t int32;
typedef uint32_t uint32;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int8_t int8;
typedef uint8_t uint8;

const float QUARTER_PI =	0.785398163f;
const float HALF_PI =		1.570796326f;
const float PI =			3.141592653f;
const float TWO_PI =		6.283185307f;

uint32 roundUpPow2(uint32 x);

bool isPowerOfTwo(uint32 x);

bool isOddNumber(uint32 x);

bool isFloatInRange(float value, float lover, float upper);

bool isFloatEqual(float value, float base, float delta = 0.001f);

bool isEqual(float a, float b, float tolerance);

float getFloatFraction(float value);

float getFloatInteger(float value);

float floorToFloatMultiple(const float value, const float multiple);

template <class T>
void dataToString(std::string& s, const T& data)
{
	std::ostringstream stream;
	stream << data;
	s = stream.str();
}

class Exception
{
public:

	Exception(const std::string& message, const std::string& file = "", const int line = -1) throw();

	virtual ~Exception() throw();

	virtual std::string toString() const throw();

	virtual std::string getMessage() const throw();

	virtual std::string getFile() const throw();

	virtual int getLine() const throw();

private:

	const std::string m_message;
	const std::string m_file;
	const int m_line;

};

template <typename T>
std::string numberToString(const T t)
{
	std::ostringstream text("");
	text << static_cast<T>(t);
	return text.str();
}

template <typename T, size_t N>
size_t numberElementsArray(T(&)[N])
{
	return N;
}

float randomFloat();

float randomFloat(float a, float b);

int32 randomBoundedInteger(int32 lowerBound, int32 upperBound);

int32 gaussianRandomValue(int32 mean, float stddev = 10.f, bool positiveValue = true);

bool propability(uint32 chance);

struct LocaleBool
{
	bool data;

	LocaleBool() { }

	LocaleBool(bool data);

	operator bool() const { return data; }

	friend std::ostream& operator <<(std::ostream& out, LocaleBool b);

	friend std::istream& operator >>(std::istream& in, LocaleBool& b);
};

/////////////////////////////////////////////////////////////
/// \brief Performs an insertion sort on a vector of elements
/// \param swap_vec The vector of elements to be sorted.
///
/// Insertion sort should *only* be used for vectors that are already nearly sorted, or
/// for vectors of size 10 or less. Otherwise this algorithm becomes computationally expensive
/// and you should probably choose another sorting algorithm at that point. A good example of
/// code that uses this algorithm well can be found in map.cpp, which sorts map objects every frame.
/// Because map objects change position slowly, there is usually no change or little relative change
/// in sorting order from the previous pass.
///
/// \note The type of element that is passed should have its > and = operators functionally
/// correct (if T is a class, you must overload these operators). In general, its good
/// practice if you overload all comparison operators for these types.
///
/// \note Do not invoke this function with a vector of pointers to class-type objects, as it
/// will cause a compilation error.
//////////////////////////////////////////////////////////////
template <typename T>
void InsertionSort(std::vector<T>& swap_vec) {
	int32 i, j;
	T value;
	for (i = 1; i < swap_vec.size(); i++) {
		value = swap_vec[i];
		for (j = i - 1; j >= 0 && swap_vec[j] > value; j--) {
			swap_vec[j+1] = swap_vec[j];
		}
		swap_vec[j+1] = value;
	}
}

bool isStringNumeric(const std::string& str);

bool fileExist(const std::string& filename);

bool moveFile(const std::string& source, const std::string& destination);

void copyFile(const std::string& source, const std::string& destination);

bool cleanDirectory(const std::string& dirName);

bool makeDirectory(const std::string& dirName);

bool removeDirectory(const std::string& dirName);

std::vector<std::string> listDirectory(const std::string& dirName, const std::string& filter);

bool deleteFile(const std::string& filename);

std::vector<std::string> tokenizeString(const std::string& str, const char* delimiter = " ", bool keepEmpty = false);

float clamp(float x, float min, float max);

float lerp(float start, float end, float percent);

sf::Vector2f lerp(sf::Vector2f start, sf::Vector2f end, float percent);

sf::Vector2f nlerp(sf::Vector2f start, sf::Vector2f end, float percent);

sf::Vector2f slerp(sf::Vector2f start, sf::Vector2f end, float percent);

sf::Vector3f lerp(sf::Vector3f start, sf::Vector3f end, float percent);

sf::Vector3f slerp(sf::Vector3f start, sf::Vector3f end, float percent);

sf::Vector3f nlerp(sf::Vector3f start, sf::Vector3f end, float percent);

template <typename T>
std::complex<T> lerp(std::complex<T> start, std::complex<T> end, float percent)
{
	return (start + percent * (end - start));
}

template <typename T>
std::complex<T> nlerp(std::complex<T> start, std::complex<T> end, float percent)
{
	return std::norm(lerp<T>(start, end, percent));
}

template <typename T>
std::complex<T> slerp(std::complex<T> start, std::complex<T> end, float percent)
{
	float dot = start.real() * end.real() + start.imag() * end.imag();
	dot = clamp(dot, -1.f, 1.f);
	float theta = std::acos(dot) * percent;
	std::complex<float> relative = end - start * dot;
	std::norm(relative);

	return ((start * std::cos(theta)) + (relative * std::sin(theta)));
}

float degToRad(float deg);

float radToDeg(float rad);

float curveAngle(float start, float end, float percent);

#endif
