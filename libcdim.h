#ifndef __CDIM_H
#define __CDIM_H

#include <string>
#include <list>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include <iostream>
#include <bitset>

using namespace std;

namespace cdim
{

    struct s_direntry
    {
      unsigned int filetype;		// filetype
      bool file_open;			// true - open file / false - file closed
      bool file_locked;			// true - file locked / false - file unlocked
      unsigned int track;		// first track of file
      unsigned int sector;		// first sector of file
      string filename;			// direntry filename 16 chars, padded with $a0
      unsigned int rel_sidetrack;	// first track of sideblocksector (REL files only)
      unsigned int rel_sidesector;	// first sector of sideblocksector (REL files only)
      unsigned int rel_recordlength;	// recordlength REL file (max. 254)
      unsigned int unused_geos_1;	// unused, expect GEOS disc (not yet supported)
      unsigned int unused_geos_2;	// unused, expect GEOS disc (not yet supported)
      unsigned int unused_geos_3;	// unused, expect GEOS disc (not yet supported)
      unsigned int unused_geos_4;	// unused, expect GEOS disc (not yet supported)
      unsigned int unused_geos_5;	// unused, expect GEOS disc (not yet supported)
      unsigned int unused_geos_6;	// unused, expect GEOS disc (not yet supported)
      unsigned int filesize;		// filesize in sectors
    };
    
    enum e_ext_filetyp
    {
      e_P00,
      e_PRG,
      e_PRG_strip_linker
    };

    class cdim
    {
      public:
	enum
	{
	  e_UNKNOWN,
	  e_D64
	};
		  
	/* constructor */
	cdim ();
	
	/* destructor */
	~cdim ();
		
	/* set filename */
	void setFilename (const string &);
	
	/* open image with filename as argument */
	bool openImage (const string &);
	
	/* open image */
	bool openImage (void);
	
	/* close image */
	void closeImage (void);
		
	/* get sectorcontent */
	bool getSector (const unsigned int &, unsigned int, vector <unsigned char> &);
	
	/* get directory content */
	bool getDirectory (list <s_direntry> &);
	
	/* extract file from image to filesystem */
	bool extractFileByIndex (const unsigned int, const string, int);
	
	/* extract file from image to filesystem by name */
	bool extractFileByName (const string &, const string, int);
	
	/* delete a File by the indexposition (mark direntry with 00 and free sectors in BAM) */
	bool scratchFileByIndex (const unsigned int);
	
	/* this function returns a file (or any other chained content) as a unsigned char vector */
	vector <unsigned char> extractFile (unsigned int, unsigned int);
	
	/* this function return the position of the direntry with the filename xxx */
	int findIndexByName (const string &);
	
	/* convert hexvalue to decimal */
	unsigned int hexchar2int (unsigned char);
	
    private:
	/* generate tracktable for image */
	void generateTrackTable (void);
	
	/* read directory */
	void readDirectory (void);

	int m_diskType;					// desired diskimagetype
	map <unsigned int, unsigned int> m_trackTable;	// mapping track startpos
	map <unsigned int, unsigned int> m_trackSector;	// table for sector per track
	fstream m_ImgFILE;				// fstream object for the image1
	vector <unsigned char> m_diskContent;		// content of the image
	string m_filename;				// imagefilename
	bool m_imageLoaded;				// flag for image is loaded or not
	list <s_direntry> m_directory;			// directory (index, entry)
    };

}

#endif

