/**********************************************************************
* Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
**********************************************************************/
 
 
#include "OneWire.h"
 
using namespace OneWire;
using namespace RomCommands;

void print_rom_id(RomId & romId);

DS2484 owm;

void setup() 
{
  Serial.begin(9600);
  while(!Serial);
  
  Serial.println("Starting Demo");
  Serial.println();

  OneWireMaster::CmdResult result = owm.begin();
  
  if(result != OneWireMaster::Success)
  {
    Serial.println("Failed to initialize OneWire Master");
    Serial.println("Ending Program");
    while(1);
  }
}

void loop()
{
  Serial.println();
  Serial.println();
  
  SearchState searchState;
  OneWireMaster::CmdResult result = owm.OWReset();
  if(result == OneWireMaster::Success)
  {
    Serial.println("1-wire devices detected");

    result = OWFirst(owm, searchState);
    if(result == OneWireMaster::Success)
    {
      do
      {
        print_rom_id(searchState.romId);
        if(searchState.romId.familyCode() == 0x10)
        {
            MultidropRomIterator selector(owm);
            DS1920 tempIbutton(selector);
            tempIbutton.setRomId(searchState.romId);

            DS1920::CmdResult tempResult = DS1920::OpFailure;
            float ds1920Temp;

            tempResult = tempIbutton.convertTemperature(ds1920Temp);
            if(tempResult == DS1920::Success)
            {
                Serial.println("Convert Temperature Success");
                Serial.print("Temperature = ");
                Serial.println(ds1920Temp, 2);
            }
            else
            {
                Serial.print("Convert Temperature Failed with code: ");
                Serial.println(tempResult, DEC);
            }
        }
        result = OWNext(owm, searchState);
      }
      while(result == OneWireMaster::Success);
    }
    else
    {
      Serial.print("OWFirst failed with error code: ");
      Serial.println(result, DEC);
    }
  }
  else
  {
    Serial.println("Failed to detect any 1-wire devices");
  }

  delay(500);
}
 
 
//*********************************************************************
void print_rom_id(RomId & romId)
{
    //print the rom number
    Serial.println();
    for(uint8_t idx = 0; idx < RomId::byteLen; idx++)
    {
      if(romId[idx] < 16)
      {
        Serial.print("0x0");
        Serial.print(romId[idx], HEX);
        Serial.print(" ");
      }
      else
      {
        Serial.print("0x");
        Serial.print(romId[idx], HEX);
        Serial.print(" ");
      }
    }
    Serial.println();
}
