/*********************************************************************
 *
 *  File Name: E131.h
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
#ifndef __GE_CE_REMOTE_E131_H__
#define __GE_CE_REMOTE_E131_H__

#include <WProgram.h>

namespace ESoft {
  
  namespace GECERemote {
    
    class E131 {
    public:
      static const short DATA_OFFSET = 126;
      
      bool validate(uint8_t buf[]) {
        return root_.validate(buf)
          && frame_.validate(buf)
          && dmp_.validate(buf);
      }
      
      uint8_t getSeqNum() const { return frame_.getSeqNum(); }
      uint16_t getUniverse() const { return frame_.getUniverse(); }
      uint16_t getLength() const { return dmp_.getLength(); }
  
      static uint32_t ntoi(uint8_t *buf) {
        return static_cast<uint32_t>(buf[0]) << 24 
          | static_cast<uint32_t>(buf[1]) << 16
          | static_cast<uint32_t>(buf[2]) << 8 
          | buf[3];
      }
      
      static uint16_t ntos(uint8_t *buf) {
        return static_cast<uint16_t>(buf[0]) << 8 | buf[1];
      } 
          
    private:
      class RootLayer {
      public:
        bool validate(uint8_t buf[]) {
          if (ntos(buf) != PREAMBLE_SIZE_ 
            || ntos(buf + 2) != POSTAMBLE_SIZE_
            || ntoi(buf + 18) != VECTOR_)
            return false;       
        
          int i = 0;
          while (i < 12)
            if (buf[i + 4] != ACN_PACKET_ID_[i++])
              return false;

          return true;   
        }
        
      private:
        static const uint16_t PREAMBLE_SIZE_ = 0x0010;
        static const uint16_t POSTAMBLE_SIZE_ = 0x0000;
        static const uint8_t ACN_PACKET_ID_[];
        static const uint32_t VECTOR_ = 0x00000004;     
      };
  
      class FramingLayer {
      public:
        FramingLayer() : seqNum_(0), universe_(0) {}
        
        bool validate(uint8_t buf[]) {
          if (ntoi(buf + 40) != VECTOR_)
            return false;
          
          seqNum_ = buf[111];
          universe_ = ntos(buf + 113);
      
          return true;
        }
  
        uint8_t getSeqNum() const { return seqNum_; }
        uint16_t getUniverse() const { return universe_; }
        
      private:
        uint8_t seqNum_;
        uint16_t universe_;
        
        static const uint32_t VECTOR_ = 0x00000002;     
      };
  
      class DMPLayer {
      public:
        DMPLayer() : length_(0) {}
        
        bool validate(uint8_t buf[]) {
          if (buf[117] != VECTOR_
            || buf[118] != ADDR_TYPE_DATA_TYPE_
            || ntos(buf + 119) != FIRST_PROPERTY_ADDR_
            || ntos(buf + 121) != ADDR_INCREMENT_)
            return false;
            
          length_ = ntos(buf + 123) - 1;
         
          return true;
        }
        
        uint16_t getLength() const { return length_; }
        
      private:
        uint16_t length_;
        
        static const uint8_t VECTOR_ = 0x02;         
        static const uint8_t ADDR_TYPE_DATA_TYPE_ = 0xA1;         
        static const uint16_t FIRST_PROPERTY_ADDR_ = 0x0000;         
        static const uint16_t ADDR_INCREMENT_ = 0x0001;         

      };

      RootLayer root_;
      FramingLayer frame_;
      DMPLayer dmp_;
  
    };
    
  }
}

#endif //__GE_CE_REMOTE_E131_H__

