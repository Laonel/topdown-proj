#ifndef _ASSET_H_
#define _ASSET_H_

#include "Utils.h"

class Asset
{
public:

	Asset();

	~Asset();

private:

	uint32 m_id;
	std::string m_filename;

};

#endif
