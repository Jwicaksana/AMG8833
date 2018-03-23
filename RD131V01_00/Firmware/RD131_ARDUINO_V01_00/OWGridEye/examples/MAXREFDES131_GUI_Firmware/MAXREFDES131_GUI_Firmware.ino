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

/*
 * To increase the I2C (wire ?) bus frequency see the following link.
 * http://forum.arduino.cc/index.php?topic=16793.0
 * 
 * This will improve the response time of the demo.
 */

#include <OneWire.h>
#include <OWGridEye.h>

using namespace OneWire;
using namespace RomCommands;

enum DemoState
{
    STATE_ESTABLISH_COMMS,
    STATE_ENUMERATE_SENSORS,
    STATE_SELECT_SENSOR,
    STATE_SEND_DATA,
    STATE_STOP_DATA,
    STATE_IDLE,
    STATE_INVALID
};

String rx_str = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
boolean receivingCmd = false;

DS2484 owm;

const int LED_PIN = 13;
const int D3 = 3;

void setup() 
{
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(D3, OUTPUT);
  digitalWrite(D3, HIGH);
  
  Serial.begin(115200);
  while(!Serial);

  owm.begin();

  // reserve 32 bytes for the inputString:
  rx_str.reserve(32);
}

void loop() 
{
  OneWireMaster::CmdResult ow_result;
  MultidropRomIterator selector(owm);
  
  //look up table for sensors, element 0 is location 1
  OWGridEye * ow_grid_eye_array[5]; 
  uint8_t num_sensors = 0; 
  OWGridEye * p_sensor;
  OWGridEye::CmdResult sensor_result;
  
  uint8_t idx;
  uint8_t selectedSensor = 0;
  int16_t recvBuff[64];
  int16_t imageBuff[64];
  float temperatureBuff[64];
  float thermistor;
  
  bool sensorsEnumerated = false;
  bool sensorSelected = false;

  String rx_cmd;
  rx_cmd.reserve(32);

  DemoState demoState = STATE_IDLE;

  while(1)
  {
    serialEvent();
    // parse the string when a newline arrives:
    if (stringComplete) 
    {
      Serial.print(rx_str);
      
      //Just get first 5 chars that represent command
      rx_cmd = rx_str.substring(0,5);
      
      if(rx_cmd.compareTo("CMD_1") == 0)
      {
          demoState = STATE_ESTABLISH_COMMS;
      }
      else if(rx_cmd.compareTo("CMD_2") == 0)
      {
          demoState = STATE_ENUMERATE_SENSORS;
      }
      else if(rx_cmd.compareTo("CMD_3") == 0)
      {
          selectedSensor = (rx_str[6] - 48);
          demoState = STATE_SELECT_SENSOR;
      }
      else if(rx_cmd.compareTo("CMD_4") == 0)
      {
          demoState = STATE_SEND_DATA;
      }
      else if(rx_cmd.compareTo("CMD_5") == 0)
      {
          demoState = STATE_STOP_DATA;
      }
      else if(rx_cmd.compareTo("CMD_6") == 0)
      {
          demoState = STATE_SEND_DATA;
      }
      else
      {
          demoState = STATE_INVALID;
      }
      
      // clear the string:
      rx_str = "";
      stringComplete = false;
    }


    switch(demoState)
    {
        case STATE_ESTABLISH_COMMS:
        
            ow_result = owm.OWInitMaster();
            if(ow_result == OneWireMaster::Success)
            {
                Serial.println(F("MBED: 1-wire Master Initialized"));
                demoState = STATE_IDLE;
            }
            else
            {
                Serial.println(F("MBED: Failed to initialize the 1-wire Master"));
                delay(500);
            }
              
        break;
        
        case STATE_ENUMERATE_SENSORS:
        
            if(sensorsEnumerated)
            {
                for(uint8_t reset_idx = 0; reset_idx < num_sensors; reset_idx++)
                {
                    delete ow_grid_eye_array[reset_idx];
                    ow_grid_eye_array[reset_idx] = 0;
                }
                sensorsEnumerated = false;
            }
            
            if(!sensorsEnumerated)
            {
                //ensure default state of DS2413 PIO
                num_sensors = init_sensor_state();
                Serial.print(F("MBED: "));
                Serial.print(num_sensors, DEC);
                Serial.println(F(" sensors initialized"));
            
                num_sensors = enumerate_sensors(selector, ow_grid_eye_array);
                Serial.print(F("MBED: "));
                Serial.print(num_sensors, DEC);
                Serial.println(F(" sensors enumerated"));
                
                Serial.print(F("MBED: NUM_Sensors "));
                Serial.println(num_sensors, DEC);
            }
            
            demoState = STATE_IDLE;
            
        break;
        
        case STATE_SELECT_SENSOR:
            
            if(selectedSensor > 0)
            {
                if(sensorSelected)
                {
                    sensor_result = p_sensor->disconnectGridEye();
                    if(sensor_result != OWGridEye::Success)
                    {
                        Serial.println(F("MBED: Failed to disconnect sensor"));
                    }
                    else
                    {
                        sensorSelected = false;
                    }
                }
                
                if(!sensorSelected)
                {
                    p_sensor = ow_grid_eye_array[selectedSensor - 1];
                    sensor_result = p_sensor->connectGridEye();
                    if(sensor_result == OWGridEye::Success)
                    {
                        //wait for sensor to init
                        delay(50);
                        sensorSelected = true;
                    }
                    else
                    {
                        Serial.println(F("MBED: Failed to connect sensor"));
                        sensorSelected = false;
                    }
                }
            }
            else
            {
                Serial.println(F("MBED: Invalid Sensor Index"));
            }
            
            demoState = STATE_IDLE;
            
        break;
        
        case STATE_SEND_DATA:
            
            if(sensorSelected)
            {
                //get raw data from GridEye
                sensor_result = p_sensor->gridEyeGetFrameTemperature(recvBuff); 
                if(sensor_result == OWGridEye::Success)
                {
                    //rotate it 180 degress
                    vAMG_PUB_IMG_ConvertRotate180(8, 8, recvBuff, imageBuff);
                    
                    //move rotated image to recvBuff so we can flip it and put it back in imageBuff
                    memcpy(recvBuff, imageBuff, 128);
                    
                    //flip on X axis
                    vAMG_PUB_IMG_ConvertFlipX(8, 8, recvBuff, imageBuff);
                    
                    //convert to float
                    for(idx = 0; idx < 64; idx++)
                    {
                        temperatureBuff[idx] = fAMG_PUB_CMN_ConvStoF(imageBuff[idx]);
                    }
                    
                    //get thermistor data
                    p_sensor->gridEyeGetThermistor(recvBuff[0]);
                    
                    //convert to float
                    thermistor = fAMG_PUB_CMN_ConvStoF(recvBuff[0]);
                    
                    //send to GUI
                    send_data(temperatureBuff, thermistor);
                    
                    demoState = STATE_IDLE;
                }
                else
                {
                    Serial.println(F("MBED: Failed to Get Frame"));
                }
            }
            else
            {
                Serial.println(F("MBED: Please Select Sensor"));
            }
             
        break;
        
        case STATE_STOP_DATA:
            demoState = STATE_IDLE;
        break;
        
        case STATE_IDLE:
            //do nothing
        break;
        
        default:
            arduino_die();  
        break;
    }

  } 
}

//*********************************************************************
void serialEvent(void) 
{
  while (Serial.available()) 
  {
    // get the new byte:
    char inChar = (char)Serial.read();

    if(!receivingCmd && inChar == 'C')
    {
      receivingCmd = true;
    }

    if(receivingCmd)
    {
      // add it to the inputString:
      rx_str += inChar;

      // if the incoming character is a newline, set a flag
      // so the main loop can do something about it:
      if (inChar == '\n') 
      {
        receivingCmd = false;
        stringComplete = true;
      }
    }
  }
}

//*********************************************************************
uint8_t init_sensor_state()
{
    OneWireMaster::CmdResult ow_result = owm.OWReset();
    SearchState search_state;
    MultidropRomIterator selector(owm);
    
    DS2413 * p_ow_switch;
    
    uint8_t num_sensors = 0;
    uint8_t num_sensors_old = 0;
    
    if(ow_result == OneWireMaster::Success)
    {
        /*this loop will run at least twice. Each time a branch is closed
          OWFirst is called in order to make sure all sensors are found.
          When num_sensors == num_sensors_old, all sensors should have been found
        */
        do
        {
            num_sensors_old = num_sensors;
            num_sensors = 0;
            
            ow_result = OWFirst(owm, search_state);
        
            while(ow_result == OneWireMaster::Success)
            {
                if(search_state.romId.familyCode() == OWGridEye::DS2413_FAMILY_CODE)
                {
                    num_sensors++;
                    
                    p_ow_switch = new DS2413(selector);
                    
                    p_ow_switch->setRomId(search_state.romId);
                    
                    p_ow_switch->pioAccessWriteChAB(0);
                    
                    delete p_ow_switch;
                }
                
                ow_result = OWNext(owm, search_state); 
            }
        }
        while(num_sensors > num_sensors_old);
        
        //Disconnect all branches and GridEYE sensors for proper enumeration
        if(num_sensors > 0)
        {
            //set all sensors to default state
            uint8_t write_val = 0xFD;
            uint8_t send_block[] = {0x5A, write_val, ~write_val};
            
            //ow_result = OWOverdriveSkipRom(owm);
            ow_result = OWSkipRom(owm);
            if(ow_result == OneWireMaster::Success)
            {
                ow_result = owm.OWWriteBlock(send_block, 3);
            }
            
            if(ow_result != OneWireMaster::Success)
            {
                Serial.println(F("MBED: Failed to init sensors"));
            }
        }
    }
    
    return num_sensors;  
}
 
 
//*********************************************************************
uint8_t enumerate_sensors(RandomAccessRomIterator & selector, OWGridEye * array[])
{
    OWGridEye * p_sensor;
    OWGridEye::CmdResult sensor_result;
    
    RomId temp_rom;
    
    uint8_t num_sensors = 0;
    uint8_t num_sensors_old = 0;
    
    uint8_t array_idx = 0;
    
    uint8_t idx;
    
    bool sensor_exists = false;
    
    OneWireMaster::CmdResult ow_result = owm.OWReset();
    SearchState search_state;
 
    if(ow_result == OneWireMaster::Success)
    {
        do
        {
            num_sensors_old = num_sensors;
            num_sensors = 0;
            
            ow_result = OWFirst(owm, search_state);
            
            while(ow_result == OneWireMaster::Success)
            {
                num_sensors++;
                
                if(search_state.romId.familyCode() == OWGridEye::DS2413_FAMILY_CODE)
                {
                    //check if sensor already exists 
                    sensor_exists = false;
                    for(idx = 0; idx < array_idx; idx++)
                    {
                        p_sensor = array[idx];
                        
                        if(p_sensor->getOWSwitchRomId() == search_state.romId)
                        {
                            sensor_exists = true;
                        }
                    }
                    
                    //if sensor doesn't already exist add to array
                    if(!sensor_exists)
                    {
                        p_sensor = new OWGridEye(selector);
                        
                        p_sensor->setOWSwitchRomId(search_state.romId);
                        
                        sensor_result = p_sensor->connectGridEye();
                        if(sensor_result == OWGridEye::Success)
                        {
                            search_state.findFamily(OWGridEye::DS28E17_FAMILY_CODE);
                            ow_result = OWNext(owm, search_state);
                            if(ow_result == OneWireMaster::Success)
                            {
                                p_sensor->setI2CBridgeRomId(search_state.romId);
                                
                                array[array_idx++] = p_sensor;
                                
                                Serial.print(F("MBED: DS2413 romId = "));
                                temp_rom =  p_sensor->getOWSwitchRomId();
                                print_rom_id(temp_rom); 
                                Serial.print(F("MBED: DS28E17 romId = "));
                                temp_rom = p_sensor->getI2CBridgeRomId();
                                print_rom_id(temp_rom); 
                                
                                sensor_result = p_sensor->disconnectGridEye();    
                                if(sensor_result == OWGridEye::OpFailure)
                                {
                                    Serial.println(F("MBED: Failed to disconnect sensor"));
                                    arduino_die();
                                }   
                                
                                sensor_result = p_sensor->connectOWbus();    
                                if(sensor_result == OWGridEye::OpFailure)
                                {
                                    Serial.print(F("MBED: Failed to connect ow branch"));
                                    Serial.println(array_idx, DEC);
                                    arduino_die();
                                }
                            }
                            else
                            {
                                Serial.print(F("MBED: Failed to find DS28E17 "));
                                Serial.println(array_idx, DEC);
                            }
                        }
                        else
                        {
                            Serial.print(F("MBED: Failed to connect GridEye "));
                            Serial.println(array_idx, DEC);
                        }
                    }
                }
                
                ow_result = OWNext(owm, search_state);
            }
        }
        while(num_sensors > num_sensors_old);
    }
    
    
    return array_idx;
}
 
 
//*********************************************************************
void send_data(float * data_buffer, float therm_temp)
{
    Serial.print(F("DATA: "));
    delay(1);
    for(uint8_t idx = 0; idx < 64; idx++)
    {
        Serial.print(data_buffer[idx], 3);
        Serial.print(F(" "));
        delay(1);
        
        if(((idx+1) % 8) == 0)
        {
            Serial.println();
            Serial.print(F("DATA: "));
            delay(1);
        }
    }                                
    Serial.print(therm_temp, 3);
    Serial.print(F(" "));
    Serial.println();
    delay(1);
}

//*********************************************************************
void print_rom_id(RomId & romId)
{
    //print the rom number
    for(uint8_t idx = 0; idx < RomId::byteLen; idx++)
    {
      if(romId[idx] < 16)
      {
        Serial.print(F("0x0"));
        Serial.print(romId[idx], HEX);
        Serial.print(F(" "));
      }
      else
      {
        Serial.print(F("0x"));
        Serial.print(romId[idx], HEX);
        Serial.print(F(" "));
      }
    }
    Serial.println();
}

//*********************************************************************
void arduino_die(void)
{
  while(1)
  {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
}



