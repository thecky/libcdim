#include "libcdim.h"

using namespace std;

namespace cdim
{
	/* constructor */
	cdim::cdim ()
	{
	  m_diskType = e_UNKNOWN;
	  m_filename = "";
	  m_imageLoaded = false;
	}
	
	/* destructor */
	cdim::~cdim ()
	{
	  this->closeImage ();
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
	
	/* set filename */
	void cdim::setFilename (const string &filename)
	{
	  m_filename = filename;
	}
	
	/* open discimage (takes filename as argument) */
	bool cdim::openImage (const string &filename)
	{
	  this->setFilename (filename);
	  
	  if (this->openImage ())
	  {
	    return true;
	  }
	  
	  return false;
	}
	
	/* open discimage and trying to guess the imagetype */
	bool cdim::openImage (void)
	{
	  if (!m_filename.empty ())
	  {
	    m_diskType = e_UNKNOWN;
	    m_ImgFILE.open (m_filename.c_str (), ios::in | ios::out | ios::binary);
	  
	    if (m_ImgFILE)
	    {
	      int imgSize;
	      m_ImgFILE.seekg (0, ios::end);
	   
	      imgSize = m_ImgFILE.tellg ();
	      m_ImgFILE.seekg (0, ios::beg);
	      
	      if (imgSize == 174848)
	      {
		// assuming D64 Image
		m_diskType = e_D64;
	
		this->generateTrackTable ();
	      }
	      
	      if (m_diskType != e_UNKNOWN)
	      {
		m_diskContent.clear ();
		m_diskContent.reserve (imgSize);
	
		m_ImgFILE >> noskipws;	// turn off whitespaceskipping
		m_diskContent.insert( m_diskContent.begin(), istream_iterator<unsigned char>(m_ImgFILE), istream_iterator<unsigned char>() );
		
		m_imageLoaded = true;
		return true;
	      }
	    }
	  }
	  return false;
	}
	
	/* close discimage */
	void cdim::closeImage (void)
	{
	  if (m_ImgFILE)
	  {
	    m_ImgFILE.close ();
	  }
	}
	
	/* get the content of a sector */
	bool cdim::getSector (const unsigned int &track, unsigned int sector, vector <unsigned char> &sectordata)
	{
	  map <unsigned int, unsigned int>::iterator tracktable_it, sectortable_it;
	  vector <unsigned char>::iterator imgposbase_it, imgposition_it;

	  unsigned int trackstart, maxsectors, startpos;
	  
	  tracktable_it = m_trackTable.find (track);
	  sectortable_it = m_trackSector.find (track);
	  
	  if (tracktable_it != m_trackTable.end () && sectortable_it != m_trackSector.end ())
	  {
	    trackstart = tracktable_it->second;
	    maxsectors = sectortable_it->second;

	    if (sector <= maxsectors)
	    {
	      sectordata.clear ();
	      sectordata.reserve (256);
	    
	      imgposbase_it = m_diskContent.begin () + trackstart + sector * 256;
	      imgposition_it = imgposbase_it;
	      
	      while (imgposition_it <= imgposbase_it + 255 && imgposition_it != m_diskContent.end ())
	      {
		sectordata.push_back (*imgposition_it);
		imgposition_it++;
	      }
	      
	      return true;
	    }
	  }
	  
	  return false;
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

