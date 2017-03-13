
//---------------------------------------------------------//
//---------------- DISPLAY VARIABLES ----------------------//
//---------------------------------------------------------//

// LCD pins
#define LCD_CS A3 // SHARED PIN!
#define LCD_CD A2 // SHARED PIN!
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// Touchscreen pins
#define YP A3 // SHARED PIN!
#define XM A2 // SHARED PIN!
#define YM 9
#define XP 8

// Color names
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define A_CYAN  0X0268
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// LCD dimension
#define WIDTH 320
#define HEIGTH 240

// Touchscreen effective values                  // TODO: Setup TS values
#define X_MIN 120
#define X_MAX 905
#define Y_MIN 75
#define Y_MAX 965

// Toucscreen pressure margins
#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define SLIDER_MIN 0
#define SLIDER_MAX 127

// Touchscreen realtime variables
MCUFRIEND_kbv tft;
uint16_t ID; // display ID
TSPoint tsLastP; // Last pressed point
TSPoint tsCurrentP; // Current pressed point
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Button variables
int statusBarY = 60;
int statusButtonW = 45;
int previousButtonX = 15;
int stopButtonX = previousButtonX + 60;
int playButtonX = stopButtonX + 60;
int pauseButtonX = playButtonX + 60;
int nextButtonX = pauseButtonX + 60;

int sliderState;
int sliderLastState;
int sliderY = 160;
int sliderX = 20;
int sliderW = 280;
int sliderH = 20;

int previousTouched = 0;
int stopTouched = 0;
int playTouched = 0;
int pauseTouched = 0;
int nextTouched = 0;

void displaySetup() {
  if (!Serial) delay(5000); // wait some time if Serial is not up
  ID = tft.readID(); // read display ID
  /*Serial.print("Display available with ID = 0x");
    Serial.println(ID, HEX);*/

  tft.begin(ID); // Start TFT display
  tft.setRotation(1); // Horizontal display

  /*Serial.print("Screen resolution: ");
    Serial.print(WIDTH);
    Serial.print("x");
    Serial.println(HEIGTH);*/

  tft.fillScreen(BLACK); // black screen

  sliderState = 50;
  sliderLastState = 50;

  tft.setTextSize(2);
  tsPinMode();
  tsLastP = ts.getPoint();
  tsCurrentP = ts.getPoint();
}

//----------------------------------------//
//---- Touchscreen detection function ----//
//----------------------------------------//
void touchDetection() {
  tsPinMode();

  tsLastP = tsCurrentP;
  tsCurrentP = ts.getPoint();

  if (tsCurrentP.z < MINPRESSURE) {
    if (previousTouched == 1) {
      //Serial.println("Released PREVIOUS");
      sendCC(channel, previousMessage, MESSAGE_LOW);
      previousTouched = 0;
    }
    if (stopTouched == 1) {
      //Serial.println("Released STOP");
      sendCC(channel, stopMessage, MESSAGE_LOW);
      stopTouched = 0;
    }
    if (playTouched == 1) {
      //Serial.println("Released PLAY");
      sendCC(channel, playMessage, MESSAGE_LOW);
      playTouched = 0;
    }
    if (pauseTouched == 1) {
      //Serial.println("Released PAUSE");
      sendCC(channel, pauseMessage, MESSAGE_LOW);
      pauseTouched = 0;
    }
    if (nextTouched == 1) {
      //Serial.println("Released NEXT");
      sendCC(channel, nextMessage, MESSAGE_LOW);
      nextTouched = 0;
    }
    return; // stop if no touch is detected
  }

  // DEBUG CODE
  /*
    Serial.print("Current point: x = ");
    Serial.print(tsCurrentP.x);
    Serial.print(" y = ");
    Serial.print(tsCurrentP.y);
    Serial.print(" z = ");
    Serial.println(tsCurrentP.z);
  */

  int y = mapY(tsCurrentP.x) - 15;
  int x = mapX(tsCurrentP.y);

  if (y >= statusBarY && y <= statusBarY + statusButtonW) {
    // PREVIOUS BUTTON
    if (x > previousButtonX && x < previousButtonX + statusButtonW) {
      if (previousTouched == 0) {
        //Serial.println("Touched PREVIOUS");
        sendCC(channel, previousMessage, MESSAGE_HIGH);
        delay(POST_BUTTON_DELAY);
        previousTouched = 1;
      }
    } else {
      if (previousTouched == 1) {
        //Serial.println("Released PREVIOUS");
        sendCC(channel, previousMessage, MESSAGE_LOW);
        previousTouched = 0;
      }
    }
    // STOP BUTTON
    if (x > stopButtonX && x < stopButtonX + statusButtonW) {
      if (stopTouched == 0) {
        //Serial.println("Touched STOP");
        sendCC(channel, stopMessage, MESSAGE_HIGH);
        delay(POST_BUTTON_DELAY);
        stopTouched = 1;
      }
    } else {
      if (stopTouched == 1) {
        //Serial.println("Released STOP");
        sendCC(channel, stopMessage, MESSAGE_LOW);
        stopTouched = 0;
      }
    }
    // PLAY BUTTON
    if (x > playButtonX && x < playButtonX + statusButtonW) {
      if (playTouched == 0) {
        //Serial.println("Touched PLAY");
        sendCC(channel, playMessage, MESSAGE_HIGH);
        delay(POST_BUTTON_DELAY);
        playTouched = 1;
      }
    } else {
      if (playTouched == 1) {
        //Serial.println("Released PLAY");
        sendCC(channel, playMessage, MESSAGE_LOW);
        playTouched = 0;
      }
    }
    // PAUSE BUTTON
    if (x > pauseButtonX && x < pauseButtonX + statusButtonW) {
      if (pauseTouched == 0) {
        //Serial.println("Touched PAUSE");
        sendCC(channel, pauseMessage, MESSAGE_HIGH);
        delay(POST_BUTTON_DELAY);
        pauseTouched = 1;
      }
    } else {
      if (pauseTouched == 1) {
        //Serial.println("Released PAUSE");
        sendCC(channel, pauseMessage, MESSAGE_LOW);
        pauseTouched = 0;
      }
    }
    // NEXT BUTTON
    if (x > nextButtonX && x < nextButtonX + statusButtonW) {
      if (nextTouched == 0) {
        //Serial.println("Touched NEXT");
        sendCC(channel, nextMessage, MESSAGE_HIGH);
        delay(POST_BUTTON_DELAY);
        nextTouched = 1;
      }
    } else {
      if (nextTouched == 1) {
        //Serial.println("Released NEXT");
        sendCC(channel, nextMessage, MESSAGE_LOW);
        nextTouched = 0;
      }
    }
  }
  y = y + 15;
  // SLIDER
  if (y > sliderY - sliderH / 2 && y < sliderY + sliderH + sliderH / 2) {
    if (x > sliderX && x < sliderX + sliderW) {
      sliderLastState = sliderState;
      sliderState = map(x, sliderX, sliderX + sliderW, SLIDER_MIN, SLIDER_MAX);
      //Serial.println("Slider touched");
      if (sliderState != sliderLastState) {
        sendCC(channel, sliderMessage, sliderState);
        updateSlider();
      }
    }
  }

}

//----------------------------------------//
//---- TFT Display drawing functions -----//
//----------------------------------------//
void drawScreen() {
  tftPinMode();

  tft.setTextColor(WHITE);
  tft.setCursor(10, 10);
  tft.print("ArMIDIno");
  tft.setCursor(10, 30);
  tft.setTextSize(1);
  tft.print("Serial-MIDI controller");
  tft.setTextSize(2);

  // previous
  tft.fillRect(previousButtonX, statusBarY, statusButtonW, statusButtonW, A_CYAN);
  tft.drawRect(previousButtonX, statusBarY, statusButtonW, statusButtonW, WHITE);
  tft.setCursor(previousButtonX + 10, statusBarY + 14);
  tft.print("<<");
  tft.drawLine(previousButtonX + 19, statusBarY + 14, previousButtonX + 19, statusBarY + 27, WHITE);
  tft.drawLine(previousButtonX + 20, statusBarY + 14, previousButtonX + 20, statusBarY + 27, WHITE);
  tft.drawLine(previousButtonX + 31, statusBarY + 14, previousButtonX + 31, statusBarY + 27, WHITE);
  tft.drawLine(previousButtonX + 32, statusBarY + 14, previousButtonX + 32, statusBarY + 27, WHITE);
  // stop
  tft.fillRect(stopButtonX, statusBarY, statusButtonW, statusButtonW, A_CYAN);
  tft.drawRect(stopButtonX, statusBarY, statusButtonW, statusButtonW, WHITE);
  tft.fillRect(stopButtonX + 15, statusBarY + 15, 15, 15, WHITE);
  //play
  tft.fillRect(playButtonX, statusBarY, statusButtonW, statusButtonW, A_CYAN);
  tft.drawRect(playButtonX, statusBarY, statusButtonW, statusButtonW, WHITE);
  tft.setCursor(playButtonX + 18, statusBarY + 14);
  tft.print(">");
  tft.drawLine(playButtonX + 18, statusBarY + 14, playButtonX + 18, statusBarY + 27, WHITE);
  tft.drawLine(playButtonX + 19, statusBarY + 14, playButtonX + 19, statusBarY + 27, WHITE);
  // pause
  tft.fillRect(pauseButtonX, statusBarY, statusButtonW, statusButtonW, A_CYAN);
  tft.drawRect(pauseButtonX, statusBarY, statusButtonW, statusButtonW, WHITE);
  tft.drawLine(pauseButtonX + 15, statusBarY + 15, pauseButtonX + 15, statusBarY + 30, WHITE);
  tft.drawLine(pauseButtonX + 16, statusBarY + 15, pauseButtonX + 16, statusBarY + 30, WHITE);
  tft.drawLine(pauseButtonX + 17, statusBarY + 15, pauseButtonX + 17, statusBarY + 30, WHITE);
  tft.drawLine(pauseButtonX + 28, statusBarY + 15, pauseButtonX + 28, statusBarY + 30, WHITE);
  tft.drawLine(pauseButtonX + 29, statusBarY + 15, pauseButtonX + 29, statusBarY + 30, WHITE);
  tft.drawLine(pauseButtonX + 30, statusBarY + 15, pauseButtonX + 30, statusBarY + 30, WHITE);
  // next
  tft.fillRect(nextButtonX, statusBarY, statusButtonW, statusButtonW, A_CYAN);
  tft.drawRect(nextButtonX, statusBarY, statusButtonW, statusButtonW, WHITE);
  tft.setCursor(nextButtonX + 10, statusBarY + 14);
  tft.print(">>");
  tft.drawLine(nextButtonX + 11, statusBarY + 14, nextButtonX + 11, statusBarY + 27, WHITE);
  tft.drawLine(nextButtonX + 12, statusBarY + 14, nextButtonX + 12, statusBarY + 27, WHITE);
  tft.drawLine(nextButtonX + 23, statusBarY + 14, nextButtonX + 23, statusBarY + 27, WHITE);
  tft.drawLine(nextButtonX + 24, statusBarY + 14, nextButtonX + 24, statusBarY + 27, WHITE);

  // slider
  tft.fillRect(sliderX, sliderY, map(sliderState, SLIDER_MIN, SLIDER_MAX, 0, sliderW), sliderH, A_CYAN);
  tft.drawRect(sliderX, sliderY, sliderW, sliderH, WHITE);
  tft.fillCircle(sliderX + map(sliderState, SLIDER_MIN, SLIDER_MAX, 0, sliderW), sliderY + sliderH / 2, sliderH, A_CYAN);
  tft.drawCircle(sliderX + map(sliderState, SLIDER_MIN, SLIDER_MAX, 0, sliderW), sliderY + sliderH / 2, sliderH, WHITE);

  // signature
  tft.setTextSize(1);
  tft.setTextColor(A_CYAN);
  tft.setCursor(10, 230);
  tft.print("Sketch by Andrea Fioroni");
  tft.setTextSize(2);
}

void updateSlider() {
  tftPinMode();
  int sliderTemp = map(sliderLastState, SLIDER_MIN, SLIDER_MAX, sliderX, sliderX + sliderW);
  tft.fillRect(sliderTemp - sliderH, sliderY - sliderH - sliderH / 2, sliderH * 2 + 1, sliderH * 4, BLACK);
  tft.fillRect(sliderX, sliderY, map(sliderState, SLIDER_MIN, SLIDER_MAX, 0, sliderW), sliderH, A_CYAN);
  tft.drawRect(sliderX, sliderY, sliderW, sliderH, WHITE);
  tft.fillCircle(sliderX + map(sliderState, SLIDER_MIN, SLIDER_MAX, 0, sliderW), sliderY + sliderH / 2, sliderH, A_CYAN);
  tft.drawCircle(sliderX + map(sliderState, SLIDER_MIN, SLIDER_MAX, 0, sliderW), sliderY + sliderH / 2, sliderH, WHITE);
}

//----------------------------------------//
//-------- Serial MIDI functions ---------//
//----------------------------------------//
void sendCC(int statusByte, int data1Byte, int data2Byte) {
  Serial.write(statusByte);
  Serial.write(data1Byte);
  Serial.write(data2Byte);
}

//----------------------------------------//
//----- TFT Display custom functions -----//
//----------------------------------------//
void tsPinMode() { // set the Shared pins as input -> touchscreen mode
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
}
void tftPinMode() { // set the Shared pins as output -> display mode
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
}
int mapX(int value) { // map the value on the X_MIN - X_MAX range                             // TODO
  return map(value, X_MIN, X_MAX, 0, WIDTH);                                                  // BETTER SETUP MAPPING FUNCTIONS
}
int mapY(int value) { // map the value on the Y_MIN - Y_MAX range
  return map(value, Y_MIN, Y_MAX, 0, HEIGTH);
}


