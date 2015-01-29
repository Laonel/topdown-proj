#ifndef _ASSET_BASE_H_
#define _ASSET_BASE_H_

#include "../../../include/Utils.h"

#include <boost/intrusive/list.hpp>

class AssetBase : public boost::intrusive::list_base_hook<>
{
public:

	AssetBase();

	AssetBase(uint32 handle, const std::string& filename = "");

	~AssetBase();

	const uint32& getHandle() const;

	const std::string& getFilename() const;

private:

	uint32 m_handle;
	std::string m_name;
	
	std::string m_filename;

};

typedef boost::intrusive::list<AssetBase> AssetList;

#endif
