#include "Filesystem/ConfigFile.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>

ConfigFile::ConfigFile() :
	m_file("")
{
}

ConfigFile::~ConfigFile()
{
	m_pTree.clear();
}

bool ConfigFile::readConfig(const std::string& filename)
{
	if (filename == "")
		return false;

	if (boost::filesystem::exists(filename))
	{
		m_pTree.clear();
		boost::property_tree::read_ini(filename, m_pTree);
	}

	m_file = filename;

	if (m_pTree.empty())
		return false;

	return true;
}

bool ConfigFile::saveConfig(const std::string& filename)
{
	if (m_pTree.empty())
		return false;

	if (filename != "")
		boost::property_tree::write_ini(filename, m_pTree);
	else
	{
		if (m_file == "")
			return false;
		else
			boost::property_tree::write_ini(m_file, m_pTree);
	}

	return true;
}

bool ConfigFile::empty()
{
	return m_pTree.empty();
}

bool ConfigFile::getBoolean(const std::string& property)
{
	return getValue<bool>(property);
}

bool ConfigFile::getBoolean(const std::string& property, bool def)
{
	return getValue<bool>(property, def);
}

int32 ConfigFile::getInt(const std::string& property)
{
	return getValue<int32>(property);
}

int32 ConfigFile::getInt(const std::string& property, int32 def)
{
	return getValue<int32>(property, def);
}

uint32 ConfigFile::getUInt(const std::string& property)
{
	return getValue<uint32>(property);
}

uint32 ConfigFile::getUInt(const std::string& property, uint32 def)
{
	return getValue<uint32>(property, def);
}

float ConfigFile::getFloat(const std::string& property)
{
	return getValue<float>(property);
}

float ConfigFile::getFloat(const std::string& property, float def)
{
	return getValue<float>(property, def);
}

std::string ConfigFile::getString(const std::string& property)
{
	return getValue<std::string>(property);
}

std::string ConfigFile::getString(const std::string& property, std::string def)
{
	return getValue<std::string>(property, def);
}
