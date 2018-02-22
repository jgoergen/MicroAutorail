#include <Stepper.h>  
#include <Wire.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>


#define m1a1  5
#define m1a2  3
#define m1b1  6
#define m1b2  4
#define neopixel_pin 8
#define tft_sda 4
#define tft_scl 5
#define encoder_a 2
#define encoder_b 7
#define encoder_button 9
#define ir  10

#define NUMPIXELS      16
#define BASE_STEPS_PER_ROTATION 100  // Max steps for one revolution
#define BASE_STEPS_PER_ITERATION 1
#define BASE_RPM 30     // Max RPM
#define BASE_PAUSE 2000    // Delay to allow Wifi to work
#define BASE_ITERATIONS 25
#define BASE_PRE_PHOTO_PAUSE 2000

volatile unsigned int encoder0Pos = 5000;
bool lastButtonVal = false;
int buttonState = 0;  
int menuIndex = 5;
int lastRotVal = 0;
int rotChange = 0; 
bool dataEntry = false;
bool jobRunning = false;
int rpm = BASE_RPM; 
int stepsPerIteration = BASE_STEPS_PER_ITERATION;
int stepsPerRotation = BASE_STEPS_PER_ROTATION;
int iterations = BASE_ITERATIONS;
int iteration = 0;
int pause = BASE_PAUSE;
int prePhotoPause = BASE_PRE_PHOTO_PAUSE;
int bedPos = 0;
int lightVal = 150;
int lvChange = 5;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, neopixel_pin, NEO_GRB + NEO_KHZ800);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
Stepper stepper(BASE_STEPS_PER_ROTATION, m1b2, m1a2, m1b1, m1a1);

void setup() {

  pinMode(encoder_a, INPUT);
  pinMode(encoder_b, INPUT);
  attachInterrupt(0, doEncoder, CHANGE);  // encoder pin on interrupt 0 - pin 2

  pinMode(encoder_button, INPUT);
  pinMode(ir, OUTPUT);
  
  pixels.begin(); // This initializes the NeoPixel library

  u8g2_prepare();
  
  stepper.setSpeed(BASE_RPM);
  stepper.step(5);
  stepper.step(-5);
}

void loop() {

  if (jobRunning)
    runLoop();
  else
    menuLoop();
}

void menuLoop() {

  lightVal += lvChange;
  if (lightVal > 250 || lightVal < 1)
    lvChange *= -1;
    
  buttonState = digitalRead(encoder_button);

  if (buttonState == LOW && lastButtonVal == false) {
    
    lastButtonVal = true;
    
  } else if (buttonState == HIGH && lastButtonVal == true) {
    
    lastButtonVal = false;

    if (dataEntry)
      dataEntry = false;
    else
      dataEntry = true;
  }

  rotChange = (lastRotVal > encoder0Pos) ? -1 : (lastRotVal < encoder0Pos) ? 1 : 0;
  lastRotVal = encoder0Pos;

  for (int i = 0; i < NUMPIXELS; i++) {

    delay(1);
    pixels.setPixelColor(i, pixels.Color(255,255,255)); 
  }

  u8g2.clearBuffer();
  
  if (dataEntry) {

    int dispVal = 0;
     
    if (int(menuIndex) == 5) {

      dataEntry = false;
      stepper.setSpeed(rpm);
      iteration = 0;
      
      for (int i = 0; i < NUMPIXELS; i++) {

        delay(1);
        pixels.setPixelColor(i, pixels.Color(255,255,255)); 
      }
        
      pixels.show();

      jobRunning = true;
      return;
      
    } else if(int(menuIndex) == 0) {

      stepsPerIteration += rotChange;
      dispVal = stepsPerIteration;
      
    } else if(int(menuIndex) == 1) {

      rpm += rotChange;
      stepper.setSpeed(rpm);
      dispVal = rpm;
      
    } else if(int(menuIndex) == 2) {

      bedPos += rotChange;
      stepper.step(rotChange);
      dispVal = bedPos;
      
    } else if(int(menuIndex) == 3) {

      pause += rotChange;
      dispVal = pause;
      
    } else if(int(menuIndex) == 4) {

      iterations += rotChange;
      dispVal = iterations;
      
    }
    
    char cstr[6];
    itoa(dispVal, cstr, 10);
    u8g2.drawStr(0, 0, cstr);
    
    pixels.setPixelColor(menuIndex, pixels.Color(0,0,lightVal)); 
    
  } else {

    menuIndex += rotChange;
    
    if (menuIndex > 5)
      menuIndex = 5;

    if (menuIndex < 0)
      menuIndex = 0;
      
    if (menuIndex == 0)
      u8g2.drawStr(0, 0, "Steps per iteration");
    else if (menuIndex == 1)
      u8g2.drawStr(0, 0, "RPM");
    else if (menuIndex == 2)
      u8g2.drawStr(0, 0, "Adjust bed");
    else if (menuIndex == 3)
      u8g2.drawStr(0, 0, "Pause time");
    else if (menuIndex == 4)
      u8g2.drawStr(0, 0, "Iterations");
    else if (menuIndex == 5) {
      
      u8g2.drawStr(0, 0, "START JOB");

      char output[24+1];
      snprintf(output, 30, "%s %d %s %d %s %d %s %d", "S", stepsPerIteration, "R", rpm, "I", iterations, "P", pause);
      u8g2.drawStr(0, 10, output);
    }

    
    pixels.setPixelColor(menuIndex, pixels.Color(0,lightVal,0)); 
  }
  
  u8g2.sendBuffer();
  pixels.show();
  delay(100);
}

void runLoop() {
  // rpm stepsPerIteration stepsPerRotation iterations pause

  if (iteration >= iterations) {

    stepper.step(iterations * stepsPerIteration);
    jobRunning = false;
    return;
  }

  u8g2.clearBuffer();
  u8g2.drawStr(0, 8, "RUNNING JOB...");
  u8g2.sendBuffer();
  stepper.step(stepsPerIteration * -1);
  delay(prePhotoPause);
  takePicture();
  delay(pause);

  iteration ++;
}

void takePicture() {
  
  for (int i=0; i < 2; i++) {
    
    pulseON(2000);                                      // pulse for 2000 uS (Microseconds)
    pulseOFF(27850);                                    // turn pulse off for 27850 us
    pulseON(390);                                       // and so on
    pulseOFF(1580);
    pulseON(410);
    pulseOFF(3580);
    pulseON(400);
    pulseOFF(63200);
  }                                                     // loop the signal twice.
}

// sets the pulse of the IR signal.
void pulseON(int pulseTime) {
  
  unsigned long endPulse = micros() + pulseTime;        // create the microseconds to pulse for
  
  while( micros() < endPulse) {
    
    digitalWrite(ir, HIGH);                       // turn IR on
    delayMicroseconds(13);                              // half the clock cycle for 38Khz (26.32×10-6s) - e.g. the 'on' part of our wave
    digitalWrite(ir, LOW);                        // turn IR off
    delayMicroseconds(13);                              // delay for the other half of the cycle to generate wave/ oscillation
  }

}

void pulseOFF(unsigned long startDelay) {
  
  unsigned long endDelay = micros() + startDelay;       // create the microseconds to delay for
  while(micros() < endDelay);
}

void doEncoder() {
  
  if (digitalRead(encoder_a) == digitalRead(encoder_b)) {
    
    encoder0Pos++;
  } else {
    
    encoder0Pos--;
  }
}

void u8g2_prepare(void) {
  
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}
