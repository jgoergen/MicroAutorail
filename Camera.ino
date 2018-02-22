void triggerCamera() {
  
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

    while(micros() < endPulse) {

        digitalWrite(IR_LED_PIN, HIGH);                       // turn IR on
        delayMicroseconds(13);                              // half the clock cycle for 38Khz (26.32×10-6s) - e.g. the 'on' part of our wave
        digitalWrite(IR_LED_PIN, LOW);                        // turn IR off
        delayMicroseconds(13);                              // delay for the other half of the cycle to generate wave/ oscillation
    }
}

void pulseOFF(unsigned long startDelay) {
  
    unsigned long endDelay = micros() + startDelay;       // create the microseconds to delay for
    while(micros() < endDelay);
}