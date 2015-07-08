/*
 *   libcdim - a library for manipulation CBM imagefiles (mainly d64)
 * 
 *   Copyright (C) [2015]  [Thomas Martens]
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the
 *   Free Software Foundation; either version 3 of the License, or (at your
 *   option) any later version.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *   or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *   for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, see <http://www.gnu.org/licenses/>.
 */

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
	      
	      if (imgSize == 0)
	      {
		m_diskType = e_NEW;
	      }
	      
	      if (imgSize == 174848)
	      {
		// assuming D64 Image
		m_diskType = e_D64;
	
		this->generateTrackTable ();
	      }
	      
	      if (m_diskType != e_UNKNOWN && m_diskType != e_NEW)
	      {
		m_diskContent.clear ();
		m_diskContent.reserve (imgSize);
	
		m_ImgFILE >> noskipws;	// turn off whitespaceskipping
		m_diskContent.insert( m_diskContent.begin(), istream_iterator<unsigned char>(m_ImgFILE), istream_iterator<unsigned char>() );
		
		m_imageLoaded = true;
		
		this->readDirectory ();
		
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
	bool cdim::readSector (const unsigned int &track, unsigned int sector, vector <unsigned char> &sectordata)
	{
	  vector <unsigned char>::iterator it_sectorpos, it_sectorposbase;
	  it_sectorpos = this->calcSectorStartpos (track, sector);
	      
	  if (it_sectorpos != m_diskContent.end ())
	  {
	    sectordata.clear ();
	    sectordata.reserve (256);
	    
	    it_sectorposbase = it_sectorpos;

	    while (it_sectorpos <= it_sectorposbase + 255 && it_sectorpos != m_diskContent.end ())
	    {
	      sectordata.push_back (*it_sectorpos);
	      it_sectorpos++;
	    }
		
	    if (sectordata.size () == 256)
	    {
	      return true;
	    }
	  }
	      
	  return false;
	}

	/* write the content of a sector */
	bool cdim::writeSector (const unsigned int &track, unsigned int sector, vector <unsigned char> &sectordata)
	{
	  if (sectordata.size () != 256)
	  {
	    return false;
	  }

  	  vector <unsigned char>::iterator it_sectorpos, it_sectorposbase;
	  it_sectorpos = this->calcSectorStartpos (track, sector);
	      
	  if (it_sectorpos != m_diskContent.end () && it_sectorpos + 255 < m_diskContent.end ())
	  {
	    copy (sectordata.begin (), sectordata.end (), it_sectorpos);
	    return true;
	  }
	  
	  return false;
	}
	
	/* return the directory content */
	bool cdim::getDirectory (list <s_direntry> &dirlist)
	{
	  if (m_imageLoaded && !m_directory.empty ())
	  {
	    dirlist = m_directory;
	    return true;
	  }
	  else
	  {
	    return false;
	  }
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

	/* read directoy */
	void cdim::readDirectory (void)
	{
	  m_directory.clear ();

	  if (m_imageLoaded)
	  {
	    unsigned int track, sector;

	    /* TODO: read track/sector from BAM */
	    track = 18;
	    sector = 1;
	    
	    vector <unsigned char> dirsector;
	    vector <unsigned char>::iterator dirsector_it;
	    
	    while (track != 0 && sector != 255)
	    {
	      if (this->readSector (track, sector, dirsector))
	      {
		dirsector_it = dirsector.begin ();

		if (dirsector_it != dirsector.end () && dirsector_it + 1 != dirsector.end ())
		{
		  track = hexchar2int (*dirsector_it);
		  sector = hexchar2int (* (++dirsector_it));

		  /* reset sector iterator */
		  dirsector_it = dirsector.begin ();
		
		  int counter = 0;
		
		  while (dirsector_it != dirsector.end ())
		  {
		    vector <unsigned char> diskdirentry;
		    vector <unsigned char>::iterator diskdirentry_it, test_it;

		    diskdirentry.clear ();

		    for (counter = 0; counter < 32 && dirsector_it != dirsector.end (); counter++)
		    {
		      diskdirentry.push_back (*dirsector_it); 
		      dirsector_it++;
		    }

		    diskdirentry_it = diskdirentry.begin ();
		    
		    if (counter != 32 || diskdirentry_it == diskdirentry.end ())
		    {
		      /* direntry not complete TODO: errorhandling */
		    }
		    else
		    {
		      /* ignore the first two bytes of the direntry
		       * - after the first direntry these bytes are always #$00
		       * - in the first direntry these bytes contains the track/sector of the next sector
		       */
		      diskdirentry_it++;
		      diskdirentry_it++;
		      
		      s_direntry direntry;
		      bitset<8> filetypflags;
		    
		      /* filetype and flags (open, lock) */
		      unsigned int tmp_filetyp;
		      tmp_filetyp = *diskdirentry_it;
		      
		      filetypflags = tmp_filetyp;
		      direntry.filetype = tmp_filetyp & 7;
		      
		      if (filetypflags.any ())
		      {
			/* file open/close flag */
			if (filetypflags[7] == 0)
			{
			  /* open file */
			  direntry.file_open = true;
			}
			else
			{
			  /* file closed */
			  direntry.file_open = false;
			}
		      
			/* file locked flag */
			if (filetypflags[6] == 1)
			{
			  /* file locked */
			  direntry.file_locked = true;
			}
			else
			{
			  /* file unlocked */
			  direntry.file_locked = false;
			}
			
			diskdirentry_it++;
		    
			direntry.track = hexchar2int (*diskdirentry_it);
			diskdirentry_it++;
		    
			direntry.sector = hexchar2int (*diskdirentry_it);
			diskdirentry_it++;
			
			string entryfilename (diskdirentry_it, diskdirentry_it + 16);
			stripWhiteSpace(entryfilename);
			
			direntry.filename = entryfilename;
			diskdirentry_it = diskdirentry_it + 16;
		    
			/* REL files relevant */
			direntry.rel_sidetrack = *diskdirentry_it;
			diskdirentry_it++;
		    
			direntry.rel_sidesector = *diskdirentry_it;
			diskdirentry_it++;
		    
			direntry.rel_recordlength = *diskdirentry_it;
			diskdirentry_it++;
		    
			/* skip unused bytes (except GEOS discs) */
			diskdirentry_it = diskdirentry_it + 6;
		    
			/* filesize */
			unsigned char low, high;
			unsigned int low_i, high_i;
			
			low = *diskdirentry_it;
			diskdirentry_it++;
			
			high = *diskdirentry_it;
			diskdirentry_it++;
			
			low_i = this->hexchar2int (low);
			high_i = this->hexchar2int (high);

			direntry.filesize = (high_i * 256) + low_i;
		      
			/* add entry to dirlist */
			m_directory.push_back (direntry);
		      }
		    }
		  }
		}
		else
		{
		  /* TODO: errorhandling */
		}
	      }
	      else
	      {
		/* TODO: errorhandling */
	      }
	    }
	  }
	}
	
	/* extract a file from the image and save it to the local filesystem
	 * 	parameters: 	const int: index position in m_directory (start with 0)
	 * 			const string:	filename for localfilesystem
	 * 			int: fileformat ({psur}00, prg, ...) see e_ext_filetyp
	 * 
	 * TODO: errorhandling, check if file is a link, overwrite protection, etc.
	 */
	bool cdim::extractFileByIndex (const unsigned int index, const string destfilename, int destfiletyp)
	{
	  if (!destfilename.empty ())
	  {
	    ofstream outFILE;
	    outFILE.open (destfilename.c_str (), ios::out | ios::binary);
	  
	    if (outFILE)
	    {
	      list <s_direntry>::iterator dir_it;
	      dir_it = m_directory.begin ();
	      
	      advance (dir_it, index);
	      
	      if (dir_it != m_directory.end ())
	      {
		unsigned int track, sector;
		s_direntry entry = *dir_it;
		/*TODO checks*/
		track = entry.track;
		sector = entry.sector;
		
		vector <unsigned char> file = this->extractFile (track, sector);
		
		switch (destfiletyp)
		{
		  case e_PRG_strip_linker:
		    file.erase (file.begin (), file.begin () + 2);
		    break;
		    
		  case e_P00:
		    /* add P00 header */
		    break;
		}
	      
		if (!file.empty())
		{
		  outFILE.write (reinterpret_cast <char*> (&file[0]), file.size () * sizeof (file[0]));
		}
	      }
	    }
	    outFILE.close ();
	    
	    return true;
	  }
	  return false;
	}
	
	/* extract file by name
	 * 	parameters: 	const string&:  filename inside the image
	 * 			const string:	filename for localfilesystem
	 * 			int: fileformat ({psur}00, prg, ...) see e_ext_filetyp
	 * 
	 * TODO: errorhandling, check if file is a link, overwrite protection, etc.
	 */
	bool cdim::extractFileByName (const string &dirfilename, const string destfilename, int destfiletyp)
	{
	  if (!destfilename.empty ())
	  {
	    int index = this->findIndexByName (dirfilename);

	    if (index != -1)
	    {
	      if (this->extractFileByIndex (index, destfilename, destfiletyp))
	      {
		return true;
	      }
	      else
	      {
		/* TODO errorhandling */
		return false;
	      }
	    }
	    else
	    {
		/* TODO errorhandling */
		return false;
	    }
	  }
	  return false;
	}

	/* delete a file from the image
	 * 	parameters: 	const int: index position in m_directory (start with 0)
	 * 
	 * returns false on error (f.e. scratch protection), true on success
	 * TODO: errorhandling
	 */
	bool cdim::scratchFileByIndex (const unsigned int index)
	{
	      list <s_direntry>::iterator dir_it;
	      dir_it = m_directory.begin ();
	      
	      advance (dir_it, index);
	      
	      if (dir_it != m_directory.end ())
	      {
		s_direntry entry = *dir_it;
		
		entry.filetype = 0x00;
		entry.file_open = false;
		
		entry.file_locked = false;
		
		/* TODO: free blocks in BAM */
		return true;
	      }
	      else
	      {
		return false;
	      }
	}
	
	/* this function returns a file (or any other chained content) as a unsigned char vector
	 * the track and sector values must be decimal */
	vector <unsigned char> cdim::extractFile (unsigned int track, unsigned int sector)
	{ 
	  vector <unsigned char> block, filecontent;
	  vector <unsigned char>::iterator it_block, it_filecontent, it_tmpblockend;
	  
	  bool breakme = false;
	  
	  while (track != 0 && !breakme)
	  {
	    if (this->readSector (track, sector, block))
	    {
	      it_block = block.begin ();

	      if (it_block != block.end () && block.size () == 256)
	      {
		track = hexchar2int (*it_block);
		sector = hexchar2int (* (++it_block));

		it_block++;
		
		if (track == 0)
		{
		  it_tmpblockend = it_block + sector;
		  breakme = true;
		}
		else
		{
		  it_tmpblockend = block.end ();
		}

		/* append current block to filevector */
		filecontent.insert (filecontent.end (), it_block, it_tmpblockend);
	      }
	      else
	      {
		/* failure block not complete */
		breakme = true;
	      }
	    }
	    else
	    {
	      /* failure (invalid sector) */
	      breakme = true;
	    }
	  }
	  
	  return filecontent;
	}
	
	/* this function return the position of the direntry with the filename xxx */
	int cdim::findIndexByName (const string &filename)
	{
	  list <s_direntry>::iterator dir_it;
	  dir_it = m_directory.begin ();
	  
	  bool found = false;
	  int counter = 0;

	  while (dir_it != m_directory.end () && !found)
	  {
	    s_direntry entry = *dir_it;

	    if (entry.filename == filename)
	    {
	      found = true;
	      return counter;
	    } 
	    
	    counter++;
	    dir_it++;
	  }
	  
	  counter = -1;
	  return counter;
	}

	/* calculate the startposition of a sector */
	vector <unsigned char>::iterator cdim::calcSectorStartpos (const unsigned int &track, unsigned int sector)
	{
	  map <unsigned int, unsigned int>::iterator tracktable_it, sectortable_it;
	  vector <unsigned char>::iterator imgposbase_it;

	  unsigned int trackstart, maxsectors;
	  
	  tracktable_it = m_trackTable.find (track);
	  sectortable_it = m_trackSector.find (track);
	  
	  if (tracktable_it != m_trackTable.end () && sectortable_it != m_trackSector.end ())
	  {
	    trackstart = tracktable_it->second;
	    maxsectors = sectortable_it->second;

	    if (sector <= maxsectors)
	    {
	      imgposbase_it = m_diskContent.begin () + trackstart + sector * 256;
	      return imgposbase_it;
	    }
	  }
	  
	  return m_diskContent.end ();
	}

	/* read a single byte from an exact position inside the image
	 * 	parameters:
	 * 		track	: starttrack in decimal (starts with 1)
	 * 		sector	: startsector in decimal (starts with 0)
	 *		bytepos	: byteposition (decimal) inside the sector (0 - 255)
	 * 		byte	: contains byte
	 * 
	 * returns true if byteposition is valid, false on invalid position */
	bool cdim::readByte (const unsigned int &track, const unsigned int &sector, const unsigned int &bytepos, unsigned char &byte)
	{
	  vector <unsigned char>::iterator byteposition_it;
	  byteposition_it = this->calcSectorStartpos (track, sector) + bytepos;
	  
	  if (byteposition_it < m_diskContent.end ())
	  {
	      byte = *byteposition_it;
	      return true;
	  }
	  
	  return false;
	}

	/* write a single byte on an exact position inside the image
	 * 	parameters:
	 * 		track	: starttrack in decimal (starts with 1)
	 * 		sector	: startsector in decimal (starts with 0)
	 *		bytepos	: byteposition (decimal) inside the sector (0 - 255)
	 * 		byte	: the byte to write
	 * 
	 * returns true if byteposition is valid, false on invalid position */
	bool cdim::writeByte (const unsigned int &track, const unsigned int &sector, const unsigned int &bytepos, const unsigned char &byte)
	{
	  vector <unsigned char>::iterator byteposition_it;
	  byteposition_it = this->calcSectorStartpos (track, sector) + bytepos;
	  
	  if (byteposition_it < m_diskContent.end ())
	  {
	      *byteposition_it = byte;
	      return true;
	  }
	  
	  return false;
	}

	/* read discname
	 * returns the name of the disc */
	string cdim::getDiscname ()
	{
	  string discname = "";
	  
	  if (m_imageLoaded)
	  {
	    unsigned int i;
	    unsigned char byte;
	    
	    for (i = 0; i < 16; i++)
	    {
	      if (!this->readByte (c_TrackBAM, c_SectorBAM, c_DiscnameBAM + i, byte))
	      {
		/* something went wrong
		 * TODO: errorhandling */
		discname = "";
		break;
	      }
	      
	      discname += byte;
	    }

	    stripWhiteSpace (discname);
	  }
	  
	  return discname;
	}
	
	/* clear discname
	 * fills the discname with #$a0 inside the BAM
	 * 	parameters: none
	 * 
	 * returns true on success */
	bool cdim::clearDiscname (void)
	{
	  unsigned int i;
	  
	  for (i = 0; i < 16; i++)
	  {
	    if (!this->writeByte (c_TrackBAM, c_SectorBAM, c_DiscnameBAM + i, 0xA0))
	    {
	      return false;
	    }
	  }
	  
	  return true;
	}
	
	/* set discname
	 * 	parameters:
	 * 		discname: the discname (max 16 bytes)
	 * 
	 * returns false if the discname is too long, or is empty. */
	bool cdim::setDiscname (const string &discname)
	{
	  if (discname.size () > 16 || discname.size () == 0)
	  {
	    return false;
	  }
	  
	  this->clearDiscname ();
	  unsigned int i;
	  
	  for (i = 0; i < discname.size (); i++)
	  {
	    if (!this->writeByte (c_TrackBAM, c_SectorBAM, c_DiscnameBAM + i, discname[i]))
	    {
	      /* something went wrong ... */
	      return false;
	    }
	  }
	  
	  return true;
	}

	/* get discid
	 * returns the discid */
	string cdim::getDiscID ()
	{
	  string discid = "";
	  
	  if (m_imageLoaded)
	  {
	    unsigned char byte1, byte2;
	    
	    if (this->readByte (c_TrackBAM, c_SectorBAM, c_IdBAM, byte1) && this->readByte (c_TrackBAM, c_SectorBAM, c_IdBAM + 1, byte2))
	    {
	      discid += byte1;
	      discid += byte2;
	    }
	    else
	    {
	      /* something went wrong */
	    }
	  }
	  
	  return discid;
	}
	
	/* set discid
	 * 	parameters:
	 * 		discid: the two byte id
	 * 
	 * returns false if the discid != 2 */
	bool cdim::setDiscID (const string &discid)
	{
	  if (discid.size () != 2)
	  {
	    return false;
	  }
	  
	  if (!this->writeByte (c_TrackBAM, c_SectorBAM, c_IdBAM, discid[0]) || !this->writeByte (c_TrackBAM, c_SectorBAM, c_IdBAM, discid[1]))
	  {
	    /* something went wrong */
	    return false;
	  }
	  
	  return true;
	}
	
	/* get dostype
	 * returns the dostype */
	string cdim::getDosType ()
	{
	  string dostype = "";
	  
	  if (m_imageLoaded)
	  {
	    unsigned char byte1, byte2;
	    
	    if (this->readByte (c_TrackBAM, c_SectorBAM, c_DosTypeBAM, byte1) && this->readByte (c_TrackBAM, c_SectorBAM, c_DosTypeBAM + 1, byte2))
	    {
	      dostype += byte1;
	      dostype += byte2;
	    }
	    else
	    {
	      /* something went wrong */
	    }
	  }
	  
	  return dostype;
	}
	
	/* set dostype
	 * 	parameters:
	 * 		dostype: the two byte dostype
	 * 
	 * ATTENTION: invalid values make the image unreadable
	 * 
	 * returns false if the dostype != 2 */
	bool cdim::setDosType (const string &dostype)
	{
	  if (dostype.size () != 2)
	  {
	    return false;
	  }
	  
	  if (! (this->writeByte (c_TrackBAM, c_SectorBAM, c_DosTypeBAM, dostype[0]) && this->writeByte (c_TrackBAM, c_SectorBAM, c_DosTypeBAM, dostype[1])))
	  {
	    /* something went wrong */
	    return false;
	  }
	  
	  return true;
	}

	/* mark a block as unused (free) in the BAM
	 * 	parameters:
	 * 		track	: starttrack in decimal (starts with 1)
	 * 		sector	: startsector in decimal (starts with 0)
	 * 
	 * returns true on success, false on failure */
	bool cdim::markBlockAsFree (const unsigned int &track, const unsigned int &sector)
	{
	  return this->markBlock (track, sector, false);
	}

	/* mark a block as used in the BAM
	 * 	parameters:
	 * 		track	: starttrack in decimal (starts with 1)
	 * 		sector	: startsector in decimal (starts with 0)
	 * 
	 * returns true on success, false on failure */
	bool cdim::markBlockAsUsed (const unsigned int &track, const unsigned int &sector)
	{
	  return this->markBlock (track, sector, true);
	}

	/* mark a block as used/unusued in the BAM
	 * 	parameters:
	 * 		track	: starttrack in decimal (starts with 1)
	 * 		sector	: startsector in decimal (starts with 0)
	 * 		freeorused:	true mark as used
	 * 				false mark as free
	 * 
	 * returns true on success, false on failure */
	bool cdim::markBlock (const unsigned int &track, const unsigned int &sector, bool freeorused)
	{
	  if (m_imageLoaded)
	  {
	    if (track <= 35)
	    {
	      int maxsectors;
	      maxsectors = this->getMaxSectors (track);
	      
	      if ((unsigned int)maxsectors < sector)
	      {
		return false;
	      }
	      
	      unsigned int i, bamoffset;
	      unsigned char bamentry[4];
	      
	      bool success = true;
	      bamoffset = 4 * (track - 1);
	      
	      for (i = 0; i < 4; i++)
	      {
		if (!this->readByte (c_TrackBAM, c_SectorBAM, c_BitmapBAM + bamoffset + i, bamentry[i]))
		{
		  /* something went wrong */
		  success = false;
		}
	      }
	      
	      if (success)
	      {
		unsigned int pos;
		pos = sector >> 3;
		
		unsigned int bitpos [8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
		
		if (freeorused)
		{
		  /* value is true, so mark block as used */
		  bamentry[1 + pos] = bamentry[1 + pos] & ~bitpos[sector & 7];
		}
		else
		{
		  /* value is false, so mark block as free */
		  bamentry[1 + pos] = bamentry[1 + pos] | bitpos[sector & 7];
		}
		
		/* ensure that the unused bits in the bitmap mark as used */
		unsigned int wastebit[5] = { 128, 192, 224, 255, 248 }; /* sectors: 17, 18, 19, invalid, 21 */
		bamentry[3] = bamentry[3] & wastebit[maxsectors & 7];
		
		/* calc free blocks */
		bamentry[0] = bamentry[1] + bamentry[2] + bamentry[3];
	      
		for (i = 0; i < 4; i++)
		{
		  if (!this->writeByte (c_TrackBAM, c_SectorBAM, c_BitmapBAM + bamoffset + i, bamentry[i]))
		  {
		    /* something went wrong */
		  }
		}
		
		return true;
	      }
	    }
	  }
	  
	  return false;
	}

	/* this function converts a hexvalue (unsigned char) to a decimal integer value) */
	unsigned int cdim::hexchar2int (unsigned char hexvalue)
	{
	  stringstream tmpstream;
	  tmpstream << hex << (unsigned int) hexvalue;
	  
	  unsigned int retvalue = 0;
	  tmpstream >> retvalue;
	  
	  return retvalue;
	}
	
	/* this function returns the max sector for a track
	 * 	parameter:
	 * 		track: desired track
	 * 
	 * return the max sectors or -1 if track is invalid */
	int cdim::getMaxSectors (const unsigned int &track)
	{
	  int sectors;
	  map <unsigned int, unsigned int>::iterator sectortable_it;
	  
	  sectortable_it = m_trackSector.find (track);
	  
	  if (sectortable_it != m_trackSector.end ())
	  {
	    sectors = sectortable_it->second;
	  }
	  else
	  {
	    sectors = -1;
	  }
	  
	  return sectors;
	}
	
	/* this function remove the trailing #$a0 from a string (filename, discname,..)
	 * 	parameter:
	 * 		str: reference to a string
	 */
	void cdim::stripWhiteSpace (string &str)
	{
	  size_t found = str.find_last_not_of (0xA0);
	      
	  if (found != string::npos)
	  {
	    str.erase(found+1);
	  }
	}
	
}
