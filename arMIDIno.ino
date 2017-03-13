
//---------------------------------------------------------//
//---------------------------------------------------------//
//------------- Arduino Mega MIDI Controller --------------//
//---- This sketch allows to control a MIDI instrument ----//
//- Arduino Mega 2560 board with a TFT Touchscreen shield -//
//---------------------------------------------------------//
//----- code by Andrea Fioroni (andrifiore@gmail.com) -----//
//---------------------------------------------------------//
//---------------------------------------------------------//

/*
 *  MIDI enconded strings
 *  14-1F are Undefinied Control Change IDs
 *  
 *  A MIDI message is made of 3 bytes:
 *  [status byte][data1 byte][data2 byte]  
 *  
 *  - The [status byte] of a Control Change message is Bn (hex)
 *    where 'n' is the MIDI channel (1->F = 16 possible MIDI channels)
 *  
 *  - The [data1 byte] of a Control message can go from 00 to 7F and 
 *    identifies the control ID
 *    
 *  - The [data2 byte] of a Control message is the value of the message
 *  
 */

#define MESSAGE_HIGH 127
#define MESSAGE_LOW 0

int channel = 0xb0; // b = Control Change ID, 0 = MIDI channel 0
int previousMessage = 0x14;
int stopMessage = 0x15;
int playMessage = 0x16;
int pauseMessage = 0x17;
int nextMessage = 0x18;
int sliderMessage = 0x19;

// delay value for loop() code
#define DELAY 10
#define POST_BUTTON_DELAY 200

//------- INCLUDES -------//
#include <SPI.h>
#include <Adafruit_GFX.h>
//#include <Adafruit_TFTLCD.h> // not needed -> implemented by MCUFRIEND_kbv
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
//------------------------//

void setup() {
  Serial.begin(9600);
  displaySetup();
  drawScreen();
}

void loop() {
  touchDetection();
  delay(DELAY);
}
