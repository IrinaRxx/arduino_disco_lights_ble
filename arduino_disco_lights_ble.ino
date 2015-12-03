/*********************************************************************
This is an example for music controlled Arduino NEOPIXEL strip using
"Disco Lights BLE" Android application.

It utilizes

 1.) Arduino UNO

 2.) nRF8001 Bluetooth Low Energy Breakout from the adafruit http://www.adafruit.com/products/1697
     and library written by Kevin Townsend/KTOWN https://github.com/adafruit/Adafruit_nRF8001.

     A guide for setup at https://learn.adafruit.com/getting-started-with-the-nrf8001-bluefruit-le-breakout. 

 3.) FastLED library for controlling NEOPIXEL strip https://github.com/FastLED/FastLED

 4.) Two effects written by Mark Kriegsman and ported to the VisualizationScheduler environment:

     a. Twinkling 'holiday' lights
     b. DiscoStrobe

 5.) VisualizationScheduler for managing different effects in time or space.

 6.) "Disco Lights BLE" Application Communication Protocol.

 7.) "Disco Lights BLE" Android application which provides beat and other information 
     for music controlled visualization.

Steps:

1.) Download nRF8001 BLE Breakout library and copy in to Arduino folder

2.) Download 'FastLed3.x' and copy in to Arduino folder

3.) Copy this folder in to Arduino folder

4.) Adjust HW setting according the your HW setup
    
    The sketch HW configuration is

    a. Arduino Uno

    b. nRF8001 BLE Breakout is connected same way as described in guide above 
       (https://learn.adafruit.com/getting-started-with-the-nrf8001-bluefruit-le-breakout/hooking-everything-up).

    c. NEOPIXEL controller is connected to pin 6.

5.) DONE

MIT license, check LICENSE for more information
All text above must be included in any redistribution
*********************************************************************/

///// BLE UART LIB ///////////////////////////////////////////////////////

#include <SPI.h>
#include "Adafruit_BLE_UART.h"

// Connect CLK/MISO/MOSI to hardware SPI
// e.g. On UNO & compatible: CLK = 13, MISO = 12, MOSI = 11
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2  // This should be an interrupt pin, on Uno thats #2 or #3
#define ADAFRUITBLE_RST 9

Adafruit_BLE_UART BTLEserial = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);
aci_evt_opcode_t  LastStatus = ACI_EVT_DISCONNECTED;

///// FASTLED LIB AND VISUALIZATION SCHEDULER WITH EFFECTS LIB

#include <FastLED.h>
#include "VisualizationScheduler.h"
#include "EffectTwinklingHolidayLights.h"
#include "EffectDiscoStrobeLights.h"
 
struct NeopixelHardwareConfiguration
{
  enum { NumofLeds      = 150   };
  enum { ControllerPin  = 6     };
  enum { MaxCurrentInmA = 10000 };
  enum { TickTimeInMs   = 0     }; // NO WAIT INSIDE SINCE THE BLE HW MUST BE POLLED //
};
 
struct EffectsConfiguration
{
  enum { NumofEffects        = 2  };
  enum { EffectTimeoutInSecs = 30 };
  enum { ExecutionOrder      = EffectOrderIncreasing };
  
  static void addEffects();
}; 

NEOPIXEL_STRIP_DEFINE_EFFECT_SCHEDULER(EffectsConfiguration);
NEOPIXEL_STRIP_DEFINE_VISUALIZATION_IFACE(NeopixelHardwareConfiguration);

// ADDING EFFECTS ON THE STRIP //

EffectTwinklingHolidayLights < 0/*StartIndex*/, NeopixelHardwareConfiguration::NumofLeds/*Length*/ > gHolidayLights;
EffectDiscoStrobeLights      < 0/*StartIndex*/, NeopixelHardwareConfiguration::NumofLeds/*Length*/ > gDiscoStrobeLights;

void EffectsConfiguration::addEffects()
{
  NEOPIXEL_STRIP_EFFECT_INSERT( &gHolidayLights,     0 );
  NEOPIXEL_STRIP_EFFECT_INSERT( &gDiscoStrobeLights, 1 );
}

///// "DISCO LIGHTS BLE" COMMUNICATION PROTOCOL

#include "DiscoLightsBleProtocol.h"

DiscoLightsBleProtocol ProtocolLayer;
bool                   BeatState;
uint8_t                BeatPerMin;
unsigned short         TotalIntensityEma;

inline
void updateTotalIntensityEma ( )
{
  enum { EmaAlpha   = 127          };
  enum { EmaAlphaM1 = 255-EmaAlpha };

  TotalIntensityEma = (TotalIntensityEma>>8) * (unsigned short)EmaAlphaM1 
                    + (unsigned short)ProtocolLayer.mTotalLogIntensity * (unsigned short)EmaAlpha;
}

inline
unsigned char GetTotalIntensityEma()
{
  uint8_t intesity = TotalIntensityEma>>8;

  if(intesity<32)
    intesity=32;
  
  return intesity;
}

inline
void ResetLocalVars ( )
{
  BeatPerMin          = 0;
  TotalIntensityEma   = 0;

  NEOPIXEL_STRIP_GLOBAL_BRIGHTNESS(120);
}

/// ARDUINO SETUP

void setup(void)
{ 
  // DEBUGGING
  
  Serial.begin(115200);
  while(!Serial); // Leonardo/Micro should wait for serial init

  // BLE UART
  
  BTLEserial.begin();

  // VISUALIZATION SCHEDULER & PROTOCOL LAYER
  
  NEOPIXEL_STRIP_ARDUINO_SETUP();
  
  // switching to manual mode: controlling effects using music recovered context ...
  
  NEOPIXEL_STRIP_MANUAL_MODE(true);
  
  BeatState = false;
  ResetLocalVars();
  NEOPIXEL_STRIP_EFFECT_SET_ENABLE(&gHolidayLights, true);
}

/// ARDUINO LOOP

inline 
void UpdateBleUartState()
{
  BTLEserial.pollACI();

  aci_evt_opcode_t status = BTLEserial.getState();
  if (status != LastStatus)
  { 
    if( status == ACI_EVT_DEVICE_STARTED || status == ACI_EVT_DISCONNECTED)
    {
	  // reset application protocol instance //
	  ProtocolLayer.Init();
	  ResetLocalVars ( );
    }
    LastStatus = status;
  }

  if (status == ACI_EVT_CONNECTED) 
  {
    if(BTLEserial.available())
    {
      while (BTLEserial.available()) 
	  {
        ProtocolLayer.FrameDecode(BTLEserial.read());
      }
    }
  }
}

inline
void ProtocolLayerSendsDevNotif( uint8_t effect_index )
{
  if(ProtocolLayer.FillBufferWithNotification(effect_index)==true )
  {
    uint8_t length = ProtocolLayer.Encode();
    BTLEserial.write(ProtocolLayer.Buffer(),length+1);
  }
}

inline
void ProcessingCommands()
{
  bool stop_read_cmd = false;

  while(stop_read_cmd==false)
  {
    unsigned short cmd   = ProtocolLayer.CommandQueueRead();
    unsigned char  param = cmd>>8;

    switch( (cmd&0xff) ) 
    {
    case ProtoID_Unknown:
      stop_read_cmd = true;
      break;
    case ProtoID_DeviceCapability:
      if(ProtocolLayer.FillBufferWithDeviceCapabilityFrame(eLedTypeStrip, 0/*high byte*/, NeopixelHardwareConfiguration::NumofLeds/*low byte*/, 2, eFeatureSupportEffectInfo)==true )
      {
        uint8_t length = ProtocolLayer.Encode();
        BTLEserial.write(ProtocolLayer.Buffer(),length+1);
      }
      break;
    case ProtoID_EffectInfo:
      if(param==1 ) 
      {
      	if(ProtocolLayer.FillBufferWithEffectInfo(param, 0, "TWINLINGHLDY" )==true)
      	{
      	  uint8_t length = ProtocolLayer.Encode();
      	  BTLEserial.write(ProtocolLayer.Buffer(),length+1);
      	}
      }
      else
      if(param==2)
      {
      	if(ProtocolLayer.FillBufferWithEffectInfo(param, 0, "DISCOSTROBE" )==true)
      	{
      	  uint8_t length = ProtocolLayer.Encode();
      	  BTLEserial.write(ProtocolLayer.Buffer(),length+1);
      	}
      }
      break;
    case ProtoID_GetDevStatus:
	    ProtocolLayerSendsDevNotif(BeatState==true?2:1);
      break;
    }
  }  
}

void loop()
{
  // BLE HW AND PROTOCOL LAYER UPDATE
  
  UpdateBleUartState();

  ProcessingCommands();

  // VISUALIZATION SCHEDULER IN MANUAL MODE
  
  if( BeatState==false 
   && ProtocolLayer.mQ8p8BeatPeriodInSamples!=0 
   && ProtocolLayer.IsInputSignalPresent()==true 
   && ProtocolLayer.IsBeatTracked()==true )
  {
    NEOPIXEL_STRIP_EFFECT_SET_ENABLE(&gHolidayLights,     false);
    NEOPIXEL_STRIP_EFFECT_SET_ENABLE(&gDiscoStrobeLights, true);

    BeatState = true;

    ProtocolLayerSendsDevNotif(2);
  }
  else
  if ( BeatState==true 
   && ( ProtocolLayer.mQ8p8BeatPeriodInSamples==0 || ProtocolLayer.IsInputSignalPresent()==false ) )
  {
    NEOPIXEL_STRIP_EFFECT_SET_ENABLE(&gHolidayLights,     true);
    NEOPIXEL_STRIP_EFFECT_SET_ENABLE(&gDiscoStrobeLights, false);    

    BeatState = false;

    if(ProtocolLayer.mQ8p8FsInHz!=0)
    {
      // connected to "Disco Lights BLE"
      
	    ProtocolLayerSendsDevNotif(1);
    }
  }

  if(ProtocolLayer.mQ8p8FsInHz!=0)
  {
    // connected to "Disco Lights BLE"
    
    updateTotalIntensityEma ( );
  
    if(BeatState==true)
    {
      // calculating BPM and updating appropriate effect
      
      uint8_t bpm = ProtocolLayer.GetBpm();
  
      if( BeatPerMin!=bpm)
      {
        BeatPerMin=(uint8_t)bpm;
        
        gDiscoStrobeLights.SetBeatBpm(BeatPerMin);
      }
    } 

    NEOPIXEL_STRIP_GLOBAL_BRIGHTNESS(GetTotalIntensityEma());
  }
  
  NEOPIXEL_STRIP_ARDUINO_LOOP();
}


