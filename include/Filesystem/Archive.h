#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#ifdef USE_POLYMORPHIC_ARCHIVE
	
	#include "Archive/polymorphic_portable_archive.hpp"

#else
	
	#include "Archive/portable_oarchive.hpp"
	#include "Archive/portable_iarchive.hpp"

#endif // USE_POLYMORPHIC_ARCHIVE

#endif
