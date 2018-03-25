
#include <OneWire.h>
#include <OWGridEye.h>

using namespace OneWire;
using namespace RomCommands;

void gridEyeFx(OWGridEye & owGridEye);

DS2484 owm;

OneWireMaster::CmdResult owResult;
SearchState searchState;
MultidropRomIterator selector(owm);
OWGridEye owGridEye(selector);
DS2413 owSwitch(selector);

void setup() 
{
  Serial.begin(115200);
  while(!Serial);
  Serial.println("This program take 8x8 pixel temperature data with 1Hz rate");
  Serial.println();

  OneWireMaster::CmdResult result = owm.begin();
  
  if(result != OneWireMaster::Success)
  {
    Serial.println("Failed to initialize OneWire Master");
    Serial.println("Ending Program");
    while(1);
  }

  owResult = owm.OWReset();
  if(owResult == OneWireMaster::Success)
  {
    //Find DS2413
    searchState.findFamily(OWGridEye::DS2413_FAMILY_CODE);
    owResult = OWNext(owm, searchState);
    if(owResult == OneWireMaster::Success)
    {
      if(searchState.romId.familyCode() == OWGridEye::DS2413_FAMILY_CODE)
      {
        //ensure DS28E17 and GridEye are connected to bus
          owSwitch.setRomId(searchState.romId);
          owSwitch.pioAccessWriteChAB(2);
      
          //let GridEye object know what the DS2413 ROM ID is
          owGridEye.setOWSwitchRomId(searchState.romId);
      }
    }
    
    //Find DS28E17
    searchState.findFamily(OWGridEye::DS28E17_FAMILY_CODE);
    owResult = OWNext(owm, searchState);
    if(owResult == OneWireMaster::Success)
    {
      
      if(searchState.romId.familyCode() == OWGridEye::DS28E17_FAMILY_CODE)
      {
        //let GridEye object know what the DS28E17 ROM ID is
          owGridEye.setI2CBridgeRomId(searchState.romId);
      } 
    }
    if(owGridEye.connectGridEye() == OWGridEye::Success)
    {
        Serial.println("GridEye Connected");
    }
  }
}

void loop() 
{
  
  Serial.println();
  gridEyeFx(owGridEye);
	delay(400);
}

//*********************************************************************
void gridEyeFx(OWGridEye & owGridEye)
{     
        //wait long enough for sample
        delay(100);
        
        int16_t pixelTemperature[64];
        float pixelTemperatureF;
        uint8_t idx, idy;
        
        OWGridEye::CmdResult result = owGridEye.gridEyeGetFrameTemperature(pixelTemperature);
        if(result == OWGridEye::Success)
        {
            Serial.println("8x8 Grid Temperature Data:");
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
			  }    
}
