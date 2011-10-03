/*********************************************************************
 *
 *  File Name: GECERemote.pde
 *
 *  Description: GE Color Effects lights remote controller main file
 *
 *  Date Created: Sep 28, 2011
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
#include <CppDefs.h>
#include <Ethernet.h>
#include <Light.h>
#include <SPI.h>
#include <Writer.h>
#include <Udp.h>

using namespace ESoft::GECEWriter;

const uint8_t TRACK_COUNT = 1;
const uint8_t TRACK = 5;
const uint8_t LIGHT_COUNT = 50;
const uint8_t MAX_BRIGHTNESS = 0xCC;
const uint8_t BUF_SIZE = 40;

const byte MAC[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
const byte IP[] = { 192,168,0,100 };
const unsigned int LOCAL_PORT = 9900;

uint8_t buf[BUF_SIZE];
byte remoteIp[4];
unsigned int remotePort;

Light lights[8];

Writer *writers[Writer::PORT_COUNT];

void setup() { 
  Writer::initialize(TRACK, LIGHT_COUNT, MAX_BRIGHTNESS, true);

  for (int i = 0; i < Writer::PORT_COUNT; i++)
    writers[i] = NULL;
    
  uint8_t lines[] = { TRACK };
  
  writers[Writer::getPort(TRACK) - 1] = new Writer(1, lines);
  
  Ethernet.begin(const_cast<byte *>(MAC), const_cast<byte *>(IP));
  Udp.begin(LOCAL_PORT);

}
  
void loop() {
  
  if (Udp.available()) {
    
    int len = Udp.readPacket(buf, BUF_SIZE);

    uint8_t lightCount = buf[0];
    
    if (lightCount > 0 && lightCount <= 8 && (lightCount << 2) <= len - 1) {
      for (int i = 0; i < lightCount; ++i)
        lights[i].setData(buf + (i << 2) + 1);

      uint8_t port = Writer::getPort(lights[0].getTrack()) - 1;
      
      if (writers[port] != NULL)
        writers[port]->write(lightCount, lights);
    }

  }
  
}

