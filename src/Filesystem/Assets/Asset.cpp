#include "Filesystem/Assets/Asset.h"

Asset::Asset() :
	m_id(0),
	m_filename("")
{
}

Asset::Asset(uint32 id, const std::string& filename) :
	m_id(id),
	m_filename(filename)
{
}

Asset::~Asset()
{
}

const uint32& Asset::getId() const
{
	return m_id;
}

const std::string& Asset::getFilename() const
{
	return m_filename;
}
