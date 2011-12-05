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
#include <Ethernet.h>
#include <SPI.h>
#include <EthernetUdp.h>

//GECEWriter
#include <CppDefs.h>
#include <AnimationExecutor.h>
#include <ColorCycle.h>
#include <ColorCycleAnimator.h>
#include <Light.h>
#include <StaticColorAnimator.h>
#include <StepAnimator.h>
#include <SyncAnimator.h>
#include <Writer.h>

using namespace ESoft::GECEWriter;

const uint8_t TRACK_COUNT = 4;
const uint8_t TRACKS[] = { 5, 6, 14, 15 };
const uint8_t LIGHT_COUNT = 50;
const uint8_t MAX_BRIGHTNESS = 0xCC;
const uint8_t LIGHT_BYTES_COUNT = 5; // track + 4 bytes data
const uint8_t BUF_SIZE = Writer::PORT_COUNT * (1 + LIGHT_BYTES_COUNT * Writer::PORT_PIN_COUNT);
const unsigned long INIT_DELAY = 5 * 1000; //ms

const byte MAC[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
IPAddress ip(192, 168, 1, 111);
const unsigned int LOCAL_PORT = 9900;

uint8_t buf[BUF_SIZE];
byte remoteIp[4];
unsigned int remotePort;

Light lights[8];

Writer *writers[Writer::PORT_COUNT];

EthernetUDP Udp;

void initLights() {

  Writer::initialize(TRACKS[1], LIGHT_COUNT, MAX_BRIGHTNESS, true, true); // reverse track 6 addressing
  Writer::initialize(TRACKS[3], LIGHT_COUNT, MAX_BRIGHTNESS, true, false, 48); // excluded defective light 48 on track 15
  
  for (int i = 0; i < TRACK_COUNT; i++)
      Writer::initialize(TRACKS[i], LIGHT_COUNT, MAX_BRIGHTNESS, true);

  for (int i = 0; i < Writer::PORT_COUNT; i++)
    writers[i] = NULL;
    
  uint8_t lines[Writer::PORT_PIN_COUNT];
  
  for (int i = 0; i < Writer::PORT_COUNT; i++) {
    int lineCount = 0;
    for (int j = 0; j < TRACK_COUNT; j++)
      if (Writer::getPort(TRACKS[j]) - 1 == i)
        lines[lineCount++] = TRACKS[j];
        
      if (lineCount > 0)
        writers[i] = new Writer(lineCount, lines);
  }
  
}

void idleLights() {

    Color roofColor[] = { Color::BLUE, Color::WHITE };
  
    SyncAnimator roof(TRACKS[0], LIGHT_COUNT, new ColorCycleAnimator(ColorCycle(2, roofColor), MAX_BRIGHTNESS, 1));
    
    StepAnimator tree1(TRACKS[1], LIGHT_COUNT, new StaticColorAnimator(MAX_BRIGHTNESS, Color::GREEN));
    StepAnimator tree2(TRACKS[2], LIGHT_COUNT, new StaticColorAnimator(MAX_BRIGHTNESS, Color::GREEN));
    
    StepAnimator door(TRACKS[7], LIGHT_COUNT, new StaticColorAnimator(MAX_BRIGHTNESS, Color::RED));
    
    TrackAnimator *list[] = { &roof, &tree1, &tree2, &door };
    AnimationExecutor exec(TRACK_COUNT, list);
    exec.animate();
    
}

void setup() {

  delay(INIT_DELAY);
  
  initLights();
  
  idleLights();
  
  if (Ethernet.begin(const_cast<byte *>(MAC))) 
    Ethernet.begin(const_cast<byte *>(MAC), ip);
    
  Udp.begin(LOCAL_PORT);
    
}

void loop() {
  
  int len = Udp.parsePacket();
  
  if (len) {
    
    Udp.read(buf, BUF_SIZE);

    int pos = 0; 
    boolean done = false;
    
    while (!done && pos < len) {
      
      uint8_t lightCount = buf[pos++];

      if (lightCount == 0xFF) 
        idleLights();    
      if (lightCount > 0 && lightCount <= Writer::PORT_PIN_COUNT && lightCount * LIGHT_BYTES_COUNT + pos <= len) {
        for (int i = 0; i < lightCount; ++i) {
          int index = pos + (i * LIGHT_BYTES_COUNT);
          lights[i].setTrack(buf[index]).setData(buf + index + 1);
        }

        pos += lightCount * LIGHT_BYTES_COUNT;
        
        uint8_t port = Writer::getPort(lights[0].getTrack()) - 1;

        if (writers[port] != NULL)
          writers[port]->write(lightCount, lights);
      } else 
        done = true;
    }

  }
  
}

