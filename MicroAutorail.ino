#include <Stepper.h>  
#include <Wire.h>
#include <U8g2lib.h> // download from here https://www.arduinolibraries.info/libraries/u8g2
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

// pin definitions

#define MOTOR_1_A1_PIN              5
#define MOTOR_1_A2_PIN              3
#define MOTOR_1_B1_PIN              6
#define MOTOR_1_B2_PIN              4
#define NEOPIXEL_PIN                8
#define TFT_SDA_PIN                 4
#define TFT_SCL_PIN                 5
#define ROTARY_ENCODER_A_PIN        2
#define ROTARY_ENCODER_B_PIN        7
#define ROTARY_ENCODER_BUTTON_PIN   9
#define IR_LED_PIN                  10
#define SERVO_PIN                   11

// config

#define NUMPIXELS                   16
#define BASE_STEPS_PER_ROTATION     100
#define BASE_STEPS_PER_ITERATION    1
#define BASE_RPM                    30
#define BASE_PAUSE                  2000
#define BASE_ITERATIONS             25
#define BASE_PRE_PHOTO_PAUSE        2000
#define BASE_ROTATION_POSITION      90    // 0 - 180
#define BASE_ROTATION_CHANGE        0
#define SERVO_MOVE_DELAY            30

volatile unsigned int encoder0Pos = 5000;
bool lastButtonVal = false;
int buttonState = 0;  
int menuIndex = 5;
int lastRotVal = 0;
int rotChange = 0; 
bool dataEntry = false;
bool taskRunning = false;
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
int rotationPosition = BASE_ROTATION_POSITION;
int rotationChange = BASE_ROTATION_CHANGE;
int rotationOffset = 0;
int newRotationOffset = 0;

Servo rotationServo;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
Stepper stepper(BASE_STEPS_PER_ROTATION, MOTOR_1_B2_PIN, MOTOR_1_A2_PIN, MOTOR_1_B1_PIN, MOTOR_1_A1_PIN);

void setup() {

    // setup rotary encoder

    pinMode(ROTARY_ENCODER_A_PIN, INPUT);
    pinMode(ROTARY_ENCODER_B_PIN, INPUT);
    attachInterrupt(0, doEncoder, CHANGE);  // encoder pin on interrupt 0 - pin 2
    pinMode(ROTARY_ENCODER_BUTTON_PIN, INPUT);
    
    // setup ir led

    pinMode(IR_LED_PIN, OUTPUT);

    // setup stepper motor

    stepper.setSpeed(BASE_RPM);

    // setup oled display

    u8g2_prepare();

    // start neopixels

    pixels.begin();
}

void loop() {

    // the app has 2 states, running a task, or in menu ( well, maybe 3, because in menu has data entry or data selection. )

    if (taskRunning)
        runTask();
    else
        menuLoop();    
}

void doEncoder() {
  
    if (digitalRead(ROTARY_ENCODER_A_PIN) == digitalRead(ROTARY_ENCODER_B_PIN))
        encoder0Pos++;
    else
        encoder0Pos--;
}

void u8g2_prepare(void) {
  
    u8g2.begin();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
}

void whiteOutPixels() {

    for (int i = 0; i < NUMPIXELS; i++) {

        delay(1);
        pixels.setPixelColor(i, pixels.Color(255,255,255)); 
    }
}
