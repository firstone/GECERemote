/*********************************************************************
 *
 *  File Name: E131.cpp
 *
 *  Description: E1.31 Packet converter
 *
 *  Date Created: Oct 5, 2011
 * 
 *  Revision History:
 *  
 *  NNN - MMM YY - Name - Change
 *  
 *  Copyright (C) 2011, Erissoft
 *  
 *  This file is part of Erissoft GE CE Lights Remote Controller
 *
 *  GE CE Lights Remote Controller is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *  
 *********************************************************************/
#include "E131.h"

namespace ESoft {
  
  namespace GECERemote {
    
    const uint8_t E131::RootLayer::ACN_PACKET_ID_[] = { 
      0x41, 0x53, 0x43, 0x2D, 0x45, 0x31, 
      0x2E, 0x31, 0x37, 0x00, 0x00, 0x00 
    };  

  }
  
}

