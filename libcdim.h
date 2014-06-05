#ifndef __CDIM_H
#define __CDIM_H

#include <string>
#include <list>
#include <fstream>

using namespace std;

namespace cdim
{
	/* return list with supported imagetypes */
	list <string> supportedImages (void);

	/* check if imagetype is supported */
	bool isImageSupported (string &);
}

#endif

