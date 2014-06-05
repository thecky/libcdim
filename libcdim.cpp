#include "libcdim.h"

using namespace std;

namespace cdim
{
	/* return a list of supported diskimages */
	list <string> getsupportedImages (void)
	{
		list <string> imagelist;
		imagelist.push_back ("d64");
		
		return imagelist;
	}

	/* check if imagetype is supported */
	bool isImageSupported (string &imagetype)
	{
		list <string> imagelist;
		imagelist = cdim::getsupportedImages ();
	}
}

