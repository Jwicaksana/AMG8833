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


#ifndef OWGRIDEYE_H
#define OWGRIDEYE_H


#include "Slaves/Bridges/Bridges.h"
#include "Slaves/Switches/Switches.h"

using namespace OneWire;

#include "API_Level_1/grideye_api_lv1.h"
#include "API_Level_2/grideye_api_lv2.h"
#include "API_Level_3/grideye_api_lv3.h"


/**
* @brief Object for interfacing to MAXREFDES131#
*
* @details MAXREFDES131# combines the DS28E17 1-wire to I2C bridge
* with the Panasonic AMG8833 GridEye sensor.  The reference design also
* includes a DS2413 2ch open drain switch for controlling the MAX4717 
* dual SPDT analog switch.  The DS28E17 and AMG8833 are connected to 
* the 1-wire bus via COM2 of the MAX4717 and COM1 is used for 
* daisy-chaining additional modules.  Disconnecting the DS28E17/AMG8833
* from the main 1-wire branch puts both devices to sleep and reduces current
* consumption from 10mA to a couple hundred uA.
*/
class OWGridEye
{
    private:
    DS2413 m_switch;
    DS28E17 m_i2c_bridge;
    
    static const uint8_t I2C_ADRS = 0x68;
    
    public:
    
    /**
    * @brief OWGridEye command results
    */
    enum CmdResult
    {
        Success,
        OpFailure
    };
    
    /**
    * @brief AMG8833 register map
    */
    enum GridEyeRegister
    {
        POWER_CONTROL,
        RESET,
        FRAME_RATE,
        INT_CONTROL,
        STATUS,
        STATUS_CLEAR,
        AVERAGE = 7,
        INT_LEVEL_1,
        INT_LEVEL_2,
        INT_LEVEL_3,
        INT_LEVEL_4,
        INT_LEVEL_5,
        INT_LEVEL_6,
        THERMISTOR_LOW,
        THERMISTOR_HI,
        INT_1,
        INT_2,
        INT_3,
        INT_4,
        INT_5,
        INT_6,
        INT_7,
        INT_8,
        PIXEL_BASE_ADRS = 0x80
    };
    
    static const uint8_t DS2413_FAMILY_CODE = 0x3A;
    
    static const uint8_t DS28E17_FAMILY_CODE = 0x19;
    
    /**
    * @brief OWGridEye Constructor
    *
    * @details setOWSwitchRomId() and setI2CBridgeRomId() must be 
    * called before any other member fxs.  
    *
    * @param[in] selector - MultidropRomIterator object that 
    * encapsulates ROM fxs of 1-wire protocol
    */
    OWGridEye(RandomAccessRomIterator & selector);
    
    
    /**
    * @brief setOWSwitchRomId
    *
    * @details sets the RomId of the DS2413
    *
    * On Entry:
    * @param[in] romId - RomId of the DS2413 for this module
    *
    * @return none
    */
    void setOWSwitchRomId(const RomId & romId)
    {
        m_switch.setRomId(romId);
    };
    
    
    /**
    * @brief getOWSwitchRomId
    *
    * @details Gets the RomId of the DS2413 for this sensor.
    * The romId must have been set first.
    *
    * @return RomId of the DS2413 for this sensor
    */
    RomId getOWSwitchRomId(void)
    {
        return m_switch.romId();
    };
    
    
    /**
    * @brief setI2CBridgeRomId
    *
    * @details sets the RomId of the DS28E17
    *
    * On Entry:
    * @param[in] romId - RomId of the DS28E17 for this module
    *
    * @return none
    */
    void setI2CBridgeRomId(const RomId & romId)
    {
        m_i2c_bridge.setRomId(romId);
    };
    
    
    /**
    * @brief getI2CBridgeRomId
    *
    * @details Gets the RomId of the DS28E17 for this sensor.
    * The romId must have been set first.
    *
    * @return RomId of the DS28E17 for this sensor
    */
    RomId getI2CBridgeRomId(void)
    {
        return m_i2c_bridge.romId();
    };

    
    /**
    * @brief disconnectGridEye
    *
    * @details Disconnects the DS28E17 and AMG8833 sensor putting both
    * to sleep
    *
    * @return CmdResult - result of operation
    */    
    CmdResult disconnectGridEye(void);
    
    
    /**
    * @brief connectGridEye
    *
    * @details Connects the DS28E17 and AMG883 to 1-wire bus
    *
    * @return CmdResult - result of operation
    */
    CmdResult connectGridEye(void);
    
    
    /**
    * @brief connectOWbus
    *
    * @details Connects down stream devices on 1-wire bus
    *
    * @return CmdResult - result of operation
    */
    CmdResult connectOWbus(void);
    
    
    /**
    * @brief disconnectOWbus
    * 
    * @details Disconnects down stream devices on 1-wire bus
    *
    * @return CmdResult - result of operation
    */
    CmdResult disconnectOWbus(void);
    
    
    /**
    * @brief gridEyeAccess
    *
    * @details Provides read/write access to the AMG8833
    *
    * On Entry:
    * @param[in] readWrite - Boolean flag indicating desired access
    * @param[in] regAdrs - AMG8833 register to start reading/writting 
    * from/to
    * @param[in] numBytes - Number of bytes to read/write
    * @param[in] dataBuf - Pointer to data buffer for storing data in 
    * on read, or data to be written on write
    *
    * On Exit:
    * @param[out] dataBuf - Read data on read operation
    *
    * @return CmdResult - result of operation
    */
    CmdResult gridEyeAccess(bool readWrite, GridEyeRegister regAdrs, uint8_t numBytes, uint8_t * dataBuf);
    
    
    /**
    * @brief gridEyeGetThermistor
    *
    * @details Gets internal thermistor temperature 
    *
    * On Entry:
    * @param[in] thermTemp - reference to int16_t var that will be overwritten
    * with thermistor data
    *
    * On Exit:
    * @param[out] thermTemp - thermistor data
    *
    * @return CmdResult - result of operation
    */
    CmdResult gridEyeGetThermistor(int16_t & thermTemp);
    
    
    /**
    * @brief gridEyeGetPixelTemperature
    *
    * @details Gets individual pixel temperature 
    *
    * On Entry:
    * @param[in] pixelAdrs - address of pixel to read
    * @param[in] pixelTemp - reference to int16_t var that will be overwritten
    * with thermistor data
    *
    * On Exit:
    * @param[out] pixelTemp - pixel data
    *
    * @return CmdResult - result of operation
    */
    CmdResult gridEyeGetPixelTemperature(uint8_t pixelAdrs, int16_t & pixelTemp);
    
    
    /**
    * @brief gridEyeGetFrameTemperature
    *
    * @details Gets pixel frame(64 pixels) temperature 
    *
    * On Entry:
    * @param[in] frameTemp - buffer to hold data
    *
    * On Exit:
    * @param[out] frameTemp - pixel data
    *
    * @return CmdResult - result of operation
    */
    CmdResult gridEyeGetFrameTemperature(int16_t * frameTemp);
};


#endif /*OWGridEye_H*/