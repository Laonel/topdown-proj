#ifndef _ASSET_H_
#define _ASSET_H_

#include "../../../include/Utils.h"

#include <boost/intrusive/list.hpp>

class AssetBase : public boost::intrusive::list_base_hook<>
{
public:

	AssetBase();

	AssetBase(const std::string& filename = "");

	~AssetBase();

	const uint32& getId() const;

	const std::string& getFilename() const;

private:

	uint32 m_handle;
	std::string m_name;
	
	std::string m_filename;

};

typedef boost::intrusive::list<AssetBase> AssetList;

#endif
