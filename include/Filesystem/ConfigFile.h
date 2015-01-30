#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_

#include "Utils.h"

#include <boost/property_tree/ptree.hpp>

class ConfigFile
{
public:

	ConfigFile();

	~ConfigFile();

	bool readConfig(const std::string& filename);

	bool saveConfig(const std::string& filename = "");

	bool empty();

	bool getBoolean(const std::string& property);
	bool getBoolean(const std::string& property, bool def);

	int32 getInt(const std::string& property);
	int32 getInt(const std::string& property, int32 def);

	uint32 getUInt(const std::string& property);
	uint32 getUInt(const std::string& property, uint32 def);

	float getFloat(const std::string& property);
	float getFloat(const std::string& property, float def);

	std::string getString(const std::string& property);
	std::string getString(const std::string& property, std::string def);

private:

	template <typename T>
	T getValue(const std::string& property)
	{
		return m_pTree.get<T>(property);
	}

	template <typename T>
	T getValue(const std::string& property, T def)
	{
		return m_pTree.get<T>(property, def);
	}

private:

	boost::property_tree::ptree m_pTree;

	std::string m_file;

};

#endif
