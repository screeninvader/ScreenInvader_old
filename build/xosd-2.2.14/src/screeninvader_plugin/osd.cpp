/* XOSD

Copyright (c) 2001 Andre Renaud (andre@ignavus.net)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "osd.hpp"

using std::string;
using std::cerr;
using std::endl;
using boost::lexical_cast;

xosd *osd = NULL;

void initialize(int v_offset, int h_offset, int h_align, int v_align, int timeout, string colour, string font)
{
  osd = xosd_create(2);
  int shadow_offset = 1;
  int outline_offset = 0;
  xosd_pos pos = XOSD_bottom;
  xosd_align align = XOSD_center;

  switch(h_align) {
    case 0:
      align = XOSD_left;
      break;
    case 1:
      align = XOSD_center;
      break;
    case 2:
      align = XOSD_right;
      break;
  }
  
  switch(v_align) {
    case 0:
      pos = XOSD_top;
      break;
    case 1:
      pos = XOSD_middle;
      break;
    case 2:
      pos = XOSD_bottom;
      break;
  }
  if (osd) {
    if (xosd_set_font(osd, font.c_str()) == -1)
      cerr << "invalid font " << font << endl;

    xosd_set_colour(osd, colour.c_str());
    if(timeout > 0)
      xosd_set_timeout(osd, timeout);
    xosd_set_shadow_offset(osd, shadow_offset);
    xosd_set_outline_offset(osd, outline_offset);
    xosd_set_pos(osd, pos);
    xosd_set_align(osd, align);
    xosd_set_vertical_offset(osd, v_offset);
    xosd_set_horizontal_offset(osd, h_offset);
  }
}

void printUsage() {
  cerr << "Usage: osd [options] text" << endl; 
  cerr << "Options:" << endl;
  cerr << "\t-v   vertical alignment: 0 = top, 1 = middle, 2 = bottom" << endl;
  cerr << "\t-h   horizontal alignment: 0 = left, 1 = center, 2 = right" << endl;
  cerr << "\t-l   vertical offset" << endl;
  cerr << "\t-b   horizontal offset" << endl;
  cerr << "\t-t   timeout: 0 = forever" << endl;
  cerr << "\t-f   font string" << endl;
  cerr << "\t-c   color name" << endl << endl;
}

int main(int argc, char *argv[])
{
  int c = 0;
  int timeout = 0;
  int v_offset = 0;
  int h_offset = 50;
  int h_align = 1;
  int v_align = 2;
  string colour = "white";
  string font = "-misc-topaz a500a1000a2000-medium-r-normal--0-260-0-0-c-0-iso8859-1";
  string text;
  
  while (optind < argc) {
    while ((c = getopt(argc, argv, "v:h:t:f:c:l:b:")) != -1) {
      switch (c) {
      case 'v':
   v_align = lexical_cast<int>(optarg);
	break;
      case 'h':
	h_align = lexical_cast<int>(optarg);
	break;
      case 't':
	timeout = lexical_cast<int>(optarg);
	break;
      case 'f':
  font = optarg;
  break;
      case 'c':
  colour = optarg; 
  break;
       case 'l':
  v_offset = lexical_cast<int>(optarg);
  break;
       case 'b':
  h_offset = lexical_cast<int>(optarg);
  break;
      case ':':
	printUsage();
	break;
      case '?':
	printUsage();
	break;
      }
    }

    if (optind < argc) {
      text = argv[optind++];
    }
    else {
      printUsage();
    }
  }
 
  initialize(h_offset, v_offset, h_align,v_align,timeout,colour,font);

  if (osd) {
    xosd_display(osd, 1, XOSD_string, text.c_str());
    xosd_wait_until_no_display (osd);
    xosd_destroy(osd);
    osd = NULL;
  } else {
    cerr << "Unable to initialize osd" << endl;
  }
    
  return 0;
}
