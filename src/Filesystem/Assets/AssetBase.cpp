#include "Filesystem/Assets/AssetBase.h"

AssetBase::AssetBase() :
	m_handle(0),
	m_filename("")
{
}

AssetBase::AssetBase(uint32 handle, const std::string& filename) :
	m_handle(handle),
	m_filename(filename)
{
}

AssetBase::~AssetBase()
{
}

const uint32& AssetBase::getHandle() const
{
	return m_handle;
}

const std::string& AssetBase::getFilename() const
{
	return m_filename;
}
