
#include <OneWire.h>
#include <OWGridEye.h>

using namespace OneWire;
using namespace RomCommands;

void print_rom_id(RomId & romId);
void gridEyeFx(OWGridEye & owGridEye);

DS2484 owm;

void setup() 
{
  Serial.begin(115200);
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
  OneWireMaster::CmdResult owResult;
  SearchState searchState;
  MultidropRomIterator selector(owm);
  OWGridEye owGridEye(selector);
  DS2413 owSwitch(selector);
  
  bool ds2413Found = false;
  bool ds28e17Found = false;
  
  owResult = owm.OWReset();
  if(owResult == OneWireMaster::Success)
  {
	  //Find DS2413
	  searchState.findFamily(OWGridEye::DS2413_FAMILY_CODE);
	  owResult = OWNext(owm, searchState);
	  if(owResult == OneWireMaster::Success)
	  {
		  Serial.print("\nROM ID = ");
		  print_rom_id(searchState.romId);
		  
		  if(searchState.romId.familyCode() == OWGridEye::DS2413_FAMILY_CODE)
		  {
			  //ensure DS28E17 and GridEye are connected to bus
		      owSwitch.setRomId(searchState.romId);
		      owSwitch.pioAccessWriteChAB(2);
		  
		      //let GridEye object know what the DS2413 ROM ID is
		      owGridEye.setOWSwitchRomId(searchState.romId);
		      ds2413Found = true;
		  }
		  else
          {
		      Serial.println("Failed To Find DS2413");
          }
	  }
	  
	  //Find DS28E17
	  searchState.findFamily(OWGridEye::DS28E17_FAMILY_CODE);
	  owResult = OWNext(owm, searchState);
	  if(owResult == OneWireMaster::Success)
	  {
		  Serial.print("\nROM ID = ");
		  print_rom_id(searchState.romId);
		  
		  if(searchState.romId.familyCode() == OWGridEye::DS28E17_FAMILY_CODE)
		  {
			  //let GridEye object know what the DS28E17 ROM ID is
		      owGridEye.setI2CBridgeRomId(searchState.romId);
		      ds28e17Found = true;
		  } 
		  else
	      {
		      Serial.println("Failed To Find DS28E17");
	      }
	  }
	  
	  if(ds2413Found && ds28e17Found)
	  {
		  Serial.println();
		  Serial.println("OWGridEye Found");
		  gridEyeFx(owGridEye);
	  }
	  else
	  {
		  Serial.println("Failed To Find OWGridEye Sensor");
	  }
	}
	else
	{
		Serial.println("No OneWire Devices On Bus");
	}
	
	delay(3000);
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


//*********************************************************************
void gridEyeFx(OWGridEye & owGridEye)
{
    if(owGridEye.connectGridEye() == OWGridEye::Success)
    {
        Serial.println("GridEye Connected");
        
        //wait long enough for sample
        delay(100);
        
        int16_t pixelTemperature[64];
        float pixelTemperatureF;
        uint8_t idx, idy;
        
        OWGridEye::CmdResult result = owGridEye.gridEyeGetFrameTemperature(pixelTemperature);
        if(result == OWGridEye::Success)
        {
            Serial.println();
            for(idx = 0; idx < 8; idx++)
            {
                for(idy = 0; idy < 8; idy++)
                {
                    pixelTemperatureF = fAMG_PUB_CMN_ConvStoF(pixelTemperature[idx*8 + idy]);
                    Serial.print(pixelTemperatureF, 2);
                    Serial.print(" ");
                }
                Serial.println();
            } 
			Serial.println();

			if(owGridEye.disconnectGridEye() == OWGridEye::Success)
			{
				Serial.println("GridEye Diconnected");
			}
			else
			{
				Serial.println("GridEye Diconnect Failed");
			}
        }
        else
        {
            Serial.println("GridEye Access Failed");
        }
    }
    else
    {
        Serial.println("GridEye Connect Failed");
    }
}