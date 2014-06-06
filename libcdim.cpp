#include "libcdim.h"

using namespace std;

namespace cdim
{
	/* constructor */
	cdim::cdim (int diskType)
	{
		m_diskType = diskType;
		//this->checkDiskType ();
		
		//this->generateTrackTable ();
	}
	
	/* destructor */
	cdim::~cdim ()
	{
	}
	
	/* return a list of supported diskimages */
	list <string> cdim::getsupportedImages (void)
	{
		list <string> imagelist;
		imagelist.push_back ("d64");
		
		return imagelist;
	}

	/* check if imagetype is supported */
	bool cdim::isImageSupported (string &imagetype)
	{
		list <string> imagelist;
		imagelist = cdim::getsupportedImages ();
	}
}

