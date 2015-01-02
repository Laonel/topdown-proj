#include "../include/Utils.h"

#ifdef _WIN32
	#include <direct.h>
	#include <ShlObj.h>
#else
	#include <dirent.h>
	#include <sys/types.h>
	#include <pwd.h>
	#include <unistd.h>
#endif

#include <fstream>
#include <sys/stat.h>

uint32 roundUpPow2(uint32 x)
{
	x -= 1;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;

	return x + 1;
}

bool isPowerOfTwo(uint32 x)
{
	return ((x & (x - 1)) == 0);
}

bool isOddNumber(uint32 x)
{
	return (x & 0x00000001);
}

bool isFloatInRange(float value, float lower, float upper)
{
	return (value >= lower && value <= upper);
}

bool isFloatEqual(float value, float base, float delta)
{
	return (value >= (base - delta) && value <= (base + delta));
}

bool isEqual(float a, float b, float tolerance)
{
	if (std::fabs(b - a) <= tolerance)
		return true;

	return false;
}

float getFloatFraction(float value)
{
	return (value - getFloatInteger(value));
}

float getFloatInteger(float value)
{
	return static_cast<float>(static_cast<int>(value));
}

float floorToFloatMultiple(const float value, const float multiple)
{
	return multiple * floor(value / multiple);
}

Exception::Exception(const std::string& message, const std::string& file, const int line) throw() :
	m_message(message),
	m_file(file),
	m_line(line)
{
}

Exception::~Exception() throw()
{
}

std::string Exception::toString() const throw()
{
	return std::string("EXCEPTION:" + m_file + ":" + numberToString(m_line) + ": " + m_message);
}

std::string Exception::getMessage() const throw()
{
	return m_message;
}

std::string Exception::getFile() const throw()
{
	return m_file;
}

int Exception::getLine() const throw()
{
	return m_line;
}

float randomFloat()
{
	return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}

float randomFloat(float a, float b)
{
	if (a > b)
	{
		float c = a;
		a = b;
		b = c;
	}

	float r = static_cast<float>(rand() % 10001);
	return a + (b - a) * r / 10000.f;
}

int32 randomBoundedInteger(int32 lowerBound, int32 upperBound)
{
	int32 range;
	float result;

	range = upperBound - lowerBound + 1;
	if (range < 0)
	{
		// TODO: handling switched lower/upper bound arguments
		range = range * -1;
	}

	result = range * randomFloat();
	result = result + lowerBound;

	return static_cast<int32>(result);
}

int32 gaussianRandomValue(int32 mean, float stddev, bool positiveValue)
{
	float x, r;
	float grvUnit;
	float result;

	if (stddev < 0)
	{
		// negative value for standard deviation argument
		stddev = -1.f * stddev;
	}

	do
	{
		float y;

		x = 2.f * randomFloat() - 1.f;
		y = 2.f * randomFloat() - 1.f;
		r = x*x + y*y;
	} while (r > 1.f || r == 0.f);

	grvUnit = x * sqrt(-2.f * log(r) / r);

	result = (grvUnit * stddev) + mean;

	if (result < 0.f && positiveValue)
		return 0;
	else
		return static_cast<int32>(result);
}

bool propability(uint32 chance)
{
	uint32 value = static_cast<uint32>(randomBoundedInteger(1, 100));
	if (value <= chance)
		return true;
	else
		return false;
}

LocaleBool::LocaleBool(bool data) :
	data(data)
{ }

std::ostream& operator <<(std::ostream& out, LocaleBool b)
{
	out << std::boolalpha << b.data;
	return out;
}

std::istream& operator >>(std::istream& in, LocaleBool& b)
{
	in >> std::boolalpha >> b.data;
	return in;
}

bool isStringNumeric(const std::string& str)
{
	if (str.empty())
		return false;

	bool decimalAllowed = true;

	size_t len = str.length();

	for (size_t c = 0; c < len; ++c)
	{
		bool numericChar = (isdigit(static_cast<int32>(str[c]))) || (c == 0 && (str[c] == '-' || str[c] == '+'));

		if (!numericChar)
		{
			if (decimalAllowed && str[c] == '.')
			{
				decimalAllowed = false;
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

bool fileExist(const std::string& filename)
{
	#ifdef _WIN32
        return GetFileAttributes((LPCSTR)(filename.c_str())) != INVALID_FILE_ATTRIBUTES;
	#else
		struct stat buf;
		if (stat(filename.c_str(), &buf) == 0)
			return true;
		else
			return false;
	#endif
}

bool moveFile(const std::string& source, const std::string& destination)
{
	if (fileExist(destination))
		remove(destination.c_str());
	return (rename(source.c_str(), destination.c_str()) == 0);
}

void copyFile(const std::string& source, const std::string& destination)
{
	if (fileExist(destination))
		remove(destination.c_str());

	std::ifstream src(source.c_str());
	std::ofstream dst(destination.c_str());
	dst << src.rdbuf();
}

bool cleanDirectory(const std::string& dirName)
{
	struct stat buf;
	int32 i = stat(dirName.c_str(), &buf);
	if (i != 0)
		return true;

	#ifdef _WIN32
		char appPath[1024];
		GetCurrentDirectoryA(1024, appPath);

		int32 appPathLen = static_cast<int32>(strlen(appPath));
		if (appPathLen <= 0)
			return false;

		if (appPath[appPathLen - 1] == '\\')
			appPath[appPathLen - 1] = '\0';

		std::string fullPath = appPath;

		if (dirName[0] == '/' || dirName[0] == '\\')
		{
			fullPath += dirName;
		}
		else
		{
			fullPath += "\\";
			fullPath += dirName;
		}

		char fileFound[1024];
		WIN32_FIND_DATAA info;
		HANDLE hp;

		sprintf_s(fileFound, "%s\\*.*", fullPath.c_str());
		hp = FindFirstFileA(fileFound, &info);

		if (hp != INVALID_HANDLE_VALUE)
		{
			do
			{
				sprintf_s(fileFound, "%s\\%s", fullPath.c_str(), info.cFileName);
				DeleteFileA(fileFound);
			} while (FindNextFileA(hp, &info));
		}

		FindClose(hp);
	#else
		DIR* parentDir;
		struct dirent *dirFile;

		parentDir = opendir(dirName.c_str());
		if (!parentDir)
			return false;

		std::string baseDir = dirName;
		if (baseDir[baseDir.length() - 1] != '/')
			baseDir += "/";

		while ((dirFile = readdir(parentDir)))
		{
			std::string fileName = baseDir + dirFile->d_name;
			remove(fileName.c_str());
		}

		closedir(parentDir);
	#endif

		return true;
}

bool makeDirectory(const std::string& dirName)
{
	struct stat buf;
	int32 i = stat(dirName.c_str(), & buf);
	if (i == 0)
		return true;

	#ifdef _WIN32
		int32 success = _mkdir(dirName.c_str());
	#else
		int32 success = mkdir(dirName.c_str(), S_IRWXO | S_IRWXU);
	#endif

		if (success == -1)
			return false;

		return true;
}

bool removeDirectory(const std::string& dirName)
{
	struct stat buf;
	int32 i = stat(dirName.c_str(), &buf);
	if (i != 0)
		return true;

	cleanDirectory(dirName);

	int32 success = rmdir(dirName.c_str());

	if (success == -1)
		return false;

	return true;
}

std::vector<std::string> listDirectory(const std::string& dirName, const std::string& filter)
{
	std::vector<std::string> directoryList;

	//Don't try to list if the directory does not exist
	struct stat buf;
	int32 i = stat(dirName.c_str(), &buf);
	if (i != 0)
		return directoryList;

	//directory exists so lets list
	#if defined _WIN32
	//Windows platform

		// Get the current directory that the application resides in
		char appPath[1024];
		GetCurrentDirectoryA(1024, appPath);

		int32 appPathLen = static_cast<int32>(strlen(appPath));
		if (appPathLen <= 0)
			return directoryList;
		if(appPath[appPathLen - 1] == '\\')    // Remove the ending slash if one is there
			appPath[appPathLen - 1] = '\0';

		std::string fullPath = appPath;

		if (dirName[0] == '/' || dirName[0] == '\\') {
			fullPath += dirName;
		}
		else {
			fullPath += "\\";
			fullPath += dirName;
		}

		char fileFound[1024];
		WIN32_FIND_DATAA info;
		HANDLE hp;
		sprintf_s(fileFound, "%s\\*.*", fullPath.c_str());
		hp = FindFirstFileA(fileFound, &info);

		if (hp != INVALID_HANDLE_VALUE) {
			// List each file from the full_path directory
			do {
			   std::string fileName(fileFound);
				if(filter == "")
					directoryList.push_back(fileFound);
				else if(fileName.find(filter) != std::string::npos)
					directoryList.push_back(fileFound);
			} while(FindNextFileA(hp, &info));
		}
		FindClose(hp);
	#else
	//Not Windows
	DIR *dir;
	struct dirent *dirFile;
	dir = opendir(dirName.c_str()); //open the directory for listing
	if(!dir) {
		std::cerr << "UTILS ERROR: Failed to list directory: " << dirName << std::endl;
		return directoryList;
	}

	//List each file found in the directory as long as it end with .lua
	while ((dirFile = readdir(dir))) {
		std::string fileName = dirFile->d_name;
		//contains a .lua ending so put it in the directory
		if(filter == "")
			directoryList.push_back(dirFile->d_name);
		else if(fileName.find(filter) != std::string::npos)
			directoryList.push_back(dirFile->d_name);
	}

	closedir(dir);

	#endif

	return directoryList;
}

bool deleteFile(const std::string& filename)
{
	if (fileExist(filename))
	{
		remove(filename.c_str());
		if (!fileExist(filename))
			return true;
	}

	return false;
}

std::vector<std::string> tokenizeString(const std::string& str, const char* delimiter, bool keepEmpty)
{
	std::vector<std::string> tokens;
	size_t prev = 0, next = 0;

	while ((next = str.find_first_of(delimiter, prev)) != std::string::npos)
	{
		if (keepEmpty || (next - prev != 0))
		{
			tokens.push_back(str.substr(prev, next - prev));
		}
		prev = next + 1;
	}

	if (prev < str.size())
	{
		tokens.push_back(str.substr(prev));
	}

	return tokens;
}

float lerp(float start, float end, float percent)
{
	return (start + percent * (end - start));
}

sf::Vector2f lerp(sf::Vector2f start, sf::Vector2f end, float percent)
{
	return (start + percent * (end - start));
}

sf::Vector2f nlerp(sf::Vector2f start, sf::Vector2f end, float percent)
{
	sf::Vector2f erp = lerp(start, end, percent);

	float n = erp.x * erp.x + erp.y * erp.y;

	if (n == 1.f)
		return erp;

	n = sqrt(n);

	if (n < 0.000001f)
		return erp;

	n = 1.f / n;
	erp.x *= n;
	erp.y *= n;
	return erp;
}

sf::Vector2f slerp(sf::Vector2f start, sf::Vector2f end, float percent)
{
	if (percent == 0.f)
		return start;

	if (start == end || percent == 1.f)
		return end;

	double theta = std::acos(start.x * end.x + start.y * end.y);
	if (theta == 0)
		return end;

	double sinTheta = std::sin(theta);
	return (float)(std::sin((1 - percent) * theta) / sinTheta) * start + (float)(std::sin(percent * theta) / sinTheta) * end;
}

sf::Vector3f lerp(sf::Vector3f start, sf::Vector3f end, float percent)
{
	return (start + percent * (end - start));
}

void normalize(float& v)
{
	float n = v * v;

	if (n == 1.f)
		return;

	n = sqrt(n);

	if (n < 0.000001f)
		return;

	n = 1.f / n;
	v *= n;
}

float clamp(float x, float min, float max)
{
	if (x < min)
		return min;
	else if (x > max)
		return max;

	return x;
}

float degToRad(float deg)
{
	return ((deg * PI) / 180);
}

float radToDeg(float rad)
{
	return ((rad * 180) / PI);
}

float curveAngle(float start, float end, float percent)
{
	while (start < 0.f)
		start += TWO_PI;
	while (start >= TWO_PI)
		start -= TWO_PI;

	while (end < 0)
		end +=  TWO_PI;
	while (end >= TWO_PI)
		end -= TWO_PI;

	if (std::abs(start - end) < PI)
		return lerp(start, end, percent);

	if (start < end)
		start += TWO_PI;
	else
		end += TWO_PI;

	float retVal = lerp(start, end, percent);
	if (retVal >= TWO_PI)
		retVal -= TWO_PI;

	return retVal;
}
