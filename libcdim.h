#ifndef __CDIM_H
#define __CDIM_H

#include <string>
#include <list>
#include <fstream>

using namespace std;

namespace cdim
{
    class cdim
    {
      public:
	enum
	{
	  e_D64
	};
	
	/* constructor */
	cdim (int);
	
	/* destructor */
	~cdim ();
	
	/* return list with supported imagetypes */
	list <string> getsupportedImages (void);

	/* check if imagetype is supported */
	bool isImageSupported (string &);

    private:
	int m_diskType;
    };
}

#endif

