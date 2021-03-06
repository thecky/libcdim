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

/* some usally imagepositions (in decimal) */
#define c_TrackBAM      18
#define c_SectorBAM     0
#define c_BitmapBAM     4
#define c_IdBAM         162
#define c_DosTypeBAM    165
#define c_DiscnameBAM   144

using namespace std;

typedef unsigned char byte;

namespace cdim
{

    struct s_direntry
    {
        unsigned int filetype;          // filetype
        bool file_open;                 // true - open file / false - file closed
        bool file_locked;               // true - file locked / false - file unlocked
        unsigned int track;             // first track of file
        unsigned int sector;            // first sector of file
        string filename;                // direntry filename 16 chars, padded with $20 - MSB cleared
        string rawfilename;             // direntry filename 16 chars, padded with $a0
        unsigned int rel_sidetrack;     // first track of sideblocksector (REL files only)
        unsigned int rel_sidesector;    // first sector of sideblocksector (REL files only)
        unsigned int rel_recordlength;  // recordlength REL file (max. 254)
        unsigned int unused_geos_1;     // unused, expect GEOS disc (not yet supported)
        unsigned int unused_geos_2;     // unused, expect GEOS disc (not yet supported)
        unsigned int unused_geos_3;     // unused, expect GEOS disc (not yet supported)
        unsigned int unused_geos_4;     // unused, expect GEOS disc (not yet supported)
        unsigned int unused_geos_5;     // unused, expect GEOS disc (not yet supported)
        unsigned int unused_geos_6;     // unused, expect GEOS disc (not yet supported)
        unsigned int filesize;          // filesize in sectors
    };

    enum e_ext_filetyp
    {
        e_P00,
        e_PRG,
        e_PRG_strip_linker
    };

    enum e_filetype
    {
        e_ft_DEL,
        e_ft_SEQ,
        e_ft_PRG,
        e_ft_USR,
        e_ft_REL
    };

    class cdim
    {
        public:
            enum
            {
                e_UNKNOWN,
                e_NEW,
                e_D64
            };

            /* constructor */
            cdim ();

            /* destructor */
            ~cdim ();

            /* set imagefilename */
            void setFilename (const string &);

            /* open image with filename as argument */
            bool openImage (const string &);

            /* open image */
            bool openImage (void);

            /* close image */
            void closeImage (void);

            /* get sectorcontent */
            bool readSector (const unsigned int &, unsigned int, vector <byte> &);

            /* write sectorcontent */
            bool writeSector (const unsigned int &, unsigned int, vector <byte> &);

            /* get directory content */
            bool getDirectory (list <s_direntry> &);

            /* extract file from image to filesystem */
            bool extractFileByIndex (const unsigned int, const string, int);

            /* extract file from image to filesystem by name */
            bool extractFileByName (const string &, const string, int);

            /* delete a File by the indexposition (mark direntry with 00 and free sectors in BAM) */
            bool scratchFileByIndex (const unsigned int);

            /* this function returns a file (or any other chained content) as a unsigned char vector */
            vector <byte> extractFile (unsigned int, unsigned int);

            /* this function return the position of the direntry with the filename xxx */
            int findIndexByName (const string &);

            /* calculate the address of a given track and sector */
            vector <byte>::iterator calcSectorStartpos (const unsigned int &, unsigned int);

            /* return discname */
            string getDiscname (void);

            /* clear the discname (fill with #$a0) */
            bool clearDiscname (void);

            /* set discname */
            bool setDiscname (const string &);

            /* get discid */
            string getDiscID (void);

            /* set discid */
            bool setDiscID (const string &);

            /* get dosversion */
            string getDosType (void);

            /* set dosversion */
            bool setDosType (const string &);
                    
            /* convert hexvalue to decimal */
            unsigned int hexchar2int (byte);

            /* convert intvalue to hexvalue */
            byte int2hexchar (unsigned int);

            /* mark a block as unused (free) in the BAM */
            bool markBlockAsFree (const unsigned int &, const unsigned int &);

            /* mark a block as unused (free) in the BAM */
            bool markBlockAsUsed (const unsigned int &, const unsigned int &);

        private:
            /* generate tracktable for image */
            void generateTrackTable (void);

            /* read directory */
            void readDirectory (void);

            /* write m_directory entries back */
            bool writeDirectory (void);

            /* read a single byte from the discimage */
            bool readByte (const unsigned int &, const unsigned int &, const unsigned int &, byte &);

            /* write a single byte to the discimage */
            bool writeByte (const unsigned int &, const unsigned int &, const unsigned int &, const byte &);

            /* returns the max sectors per track */
            int getMaxSectors (const unsigned int &);

            /* mark block as used in BAM */
            bool markBlock (const unsigned int &, const unsigned int &, bool);

            /* remove trailing #$a0 from string */
            void stripWhiteSpace (string &);

            unsigned int m_diskType;                            // desired diskimagetype (f.e. e_D64)
            map <unsigned int, unsigned int> m_trackTable;      // mapping track startpos
            map <unsigned int, unsigned int> m_trackSector;     // table for sector per track
            fstream m_ImgFILE;                                  // fstream object for the discimage
            vector <byte> m_diskContent;                        // content of the discimage
            string m_filename;                                  // imagefilename
            bool m_imageLoaded;                                 // flag for image is loaded or not
            list <s_direntry> m_directory;                      // directory entries
            list <byte> m_directory_sectorlist;                 // all tracks and sectors from the directory
    };

}

#endif  /* __CDIM_H */
