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


#include "OWGridEye.h"


//*********************************************************************
OWGridEye::OWGridEye(RandomAccessRomIterator & selector) 
: m_switch(selector), m_i2c_bridge(selector)
{
}


//*********************************************************************
OWGridEye::CmdResult OWGridEye::disconnectGridEye(void)
{
    OWGridEye::CmdResult result = OWGridEye::OpFailure;
    uint8_t pio_state;
    
    DS2413::CmdResult sw_result = m_switch.pioAccessReadChB(pio_state);
    if(sw_result == DS2413::Success)
    {
        //if high take low, falling edge puts bridge and sensor to sleep
        if(pio_state)
        {
            sw_result = m_switch.pioAccessWriteChB(0);
            if(sw_result == DS2413::Success)
            {
                result = OWGridEye::Success;
            }
        }
        else
        {
            //if low, take high, then low to get falling edge
            sw_result = m_switch.pioAccessWriteChB(1);
            if(sw_result == DS2413::Success)
            {
                sw_result = m_switch.pioAccessWriteChB(0);
                if(sw_result == DS2413::Success)
                {
                    result = OWGridEye::Success;
                }
            }
        }
    }
    
    return result;
}


//*********************************************************************
OWGridEye::CmdResult OWGridEye::connectGridEye(void)
{
    OWGridEye::CmdResult result = OWGridEye::OpFailure;
    
    DS2413::CmdResult sw_result = m_switch.pioAccessWriteChB(1);
    if(sw_result == DS2413::Success)
    {
        result = OWGridEye::Success;
    }
    
    return result;
}


//*********************************************************************
OWGridEye::CmdResult OWGridEye::connectOWbus(void)
{
    OWGridEye::CmdResult result = OWGridEye::OpFailure;
    
    DS2413::CmdResult sw_result = m_switch.pioAccessWriteChA(0);
    if(sw_result == DS2413::Success)
    {
        result = OWGridEye::Success;
    }
    
    return result;
}


//*********************************************************************
OWGridEye::CmdResult OWGridEye::disconnectOWbus(void)
{
    OWGridEye::CmdResult result = OWGridEye::OpFailure;
    
    DS2413::CmdResult sw_result = m_switch.pioAccessWriteChA(1);
    if(sw_result == DS2413::Success)
    {
        result = OWGridEye::Success;
    }
    
    return result;
}


//*********************************************************************
OWGridEye::CmdResult OWGridEye::gridEyeAccess(bool readWrite, GridEyeRegister regAdrs, uint8_t numBytes, uint8_t * dataBuf)
{
    OWGridEye::CmdResult gridEye_result = OWGridEye::OpFailure;
    DS28E17::CmdResult i2c_cmd_result = DS28E17::OperationFailure;
    
    uint8_t read_pointer = regAdrs;
    uint8_t status, wr_status;
    
    if(readWrite)
    {
        i2c_cmd_result = m_i2c_bridge.I2C_WriteReadDataWithStop((OWGridEye::I2C_ADRS << 1), 
                                                              1, &read_pointer, numBytes, 
                                                              status, wr_status, dataBuf);
    }
    else
    {
        i2c_cmd_result = m_i2c_bridge.I2C_WriteDataWithStop((OWGridEye::I2C_ADRS << 1), 
                                                           numBytes, dataBuf, status, 
                                                           wr_status);
    }
    
    if(i2c_cmd_result == DS28E17::Success)
    {
        gridEye_result = OWGridEye::Success;
    }
    
    return gridEye_result;
}


//*********************************************************************
OWGridEye::CmdResult OWGridEye::gridEyeGetThermistor(int16_t & thermTemp)
{
    OWGridEye::CmdResult result;
    uint8_t data[2];
    
    result = this->gridEyeAccess(1, OWGridEye::THERMISTOR_LOW, 2, data);
    if(result == OWGridEye::Success)
    {
        thermTemp = shAMG_PUB_TMP_ConvThermistor(data);
    }
    
    return result;
}
 
 
//*********************************************************************    
OWGridEye::CmdResult OWGridEye::gridEyeGetPixelTemperature(uint8_t pixelAdrs, int16_t & pixelTemp)
{
    OWGridEye::CmdResult result;
    uint8_t data[2];
    
    result = this->gridEyeAccess(1, (GridEyeRegister) pixelAdrs, 2, data);
    if(result == OWGridEye::Success)
    {
        pixelTemp = shAMG_PUB_TMP_ConvTemperature(data);
    }
    
    return result;
}
 
 
//*********************************************************************
OWGridEye::CmdResult OWGridEye::gridEyeGetFrameTemperature(int16_t * frameTemp)
{
    OWGridEye::CmdResult result;
    uint8_t data[128];
    
    result = this->gridEyeAccess(1,(GridEyeRegister) OWGridEye::PIXEL_BASE_ADRS, 128, data);
    if(result == OWGridEye::Success)
    {
        vAMG_PUB_TMP_ConvTemperature64(data, frameTemp);
    }
    
    return result;
}
