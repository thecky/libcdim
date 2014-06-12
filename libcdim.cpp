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
	
	/* generate tracktable for image */
	void cdim::generateTrackTable (void)
	{
	  m_trackTable.clear ();
	  m_trackSector.clear ();
	  
	  unsigned int track, trackend, position, sectors;
	  position = 0;

	  if (m_diskType == e_D64)
	  {
	    trackend = 36;
	    sectors = 20;
	  }
	  
	  for (track = 1; track < trackend; track++)
	  {
	    m_trackTable.insert (pair <unsigned int, unsigned int> (track, position) );
	    m_trackSector.insert (pair <unsigned int, unsigned int> (track, sectors) );
	    
	    if (track < 18)
	    {
	      position = position + 5376;	// offset is $1500
	      sectors = 20;
	    }
	    
	    if (track > 17 && track < 25)
	    {
	      position = position + 4864;	// offset is $1300
	      sectors = 18;
	    }
	    
	    if (track > 25 && track < 31)
	    {
	      position = position + 4608;	// offset is $1200
	      sectors = 17;
	    }
	    
	    if (track > 31)
	    {
	      position = position + 4352;	// offset is $1100
	      sectors = 16;
	    }
	  }
	}
}

