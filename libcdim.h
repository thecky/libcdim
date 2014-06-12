#ifndef __CDIM_H
#define __CDIM_H

#include <string>
#include <list>
#include <map>
#include <vector>
#include <fstream>
#include <iterator>

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
	/* generate tracktable for image */
	void generateTrackTable (void);
	
	int m_diskType;					// desired diskimagetype
	map <unsigned int, unsigned int> m_trackTable;	// mapping track startpos
	map <unsigned int, unsigned int> m_trackSector;	// table for sector per track
	fstream m_ImgFILE;				// fstream object for the image
	vector <unsigned char> m_diskContent;		// content of the image
    };
}

#endif

