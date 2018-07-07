void menuLoop() {

    // for color fading

    rotatePixelValues();
    
    buttonState = digitalRead(ROTARY_ENCODER_BUTTON_PIN);

    // debouncing the button presses off the rotary encoder

    if (buttonState == LOW && lastButtonVal == false) {
    
        lastButtonVal = true;
    
    } else if (buttonState == HIGH && lastButtonVal == true) {
    
        lastButtonVal = false;

        if (dataEntry)
            dataEntry = false;
        else
            dataEntry = true;
    }

    // to keep the rotary value movement consisten and reasonable we just check to see if it's went up or down since the last loop.
    // then we add or subtract 1 from our relative value

    rotChange = (lastRotVal > encoder0Pos) ? -1 : (lastRotVal < encoder0Pos) ? 1 : 0;
    lastRotVal = encoder0Pos;

    whiteOutPixels();
    u8g2.clearBuffer();
    
    // we're either selecting a value for editing, or editing a value. ( ok, or selecting a function. )

    if (dataEntry) 
        runDataEntry();
    else 
        runDataSelection();

    // display oled buffer, display neopixels
    
    u8g2.sendBuffer();
    pixels.show();
    delay(33);
}

void runDataEntry() {

    int dispVal = 0;
     
    if (int(menuIndex) == 7) {

        // special selection, this actually starts running the task.

        dataEntry = false;
        startTask();
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

      rotationPosition += rotChange;
      rotationServo.write(rotationPosition);
      dispVal = rotationPosition;
      
    } else if(int(menuIndex) == 4) {

      rotationChange += rotChange;
      dispVal = rotationChange;
      
    } else if(int(menuIndex) == 5) {

        pause += rotChange;
        dispVal = pause;
    
    } else if(int(menuIndex) == 6) {

        iterations += rotChange;
        dispVal = iterations;
    }
    
    // make byte array out of the display value and display it on the oled

    char cstr[6];
    itoa(dispVal, cstr, 10);
    u8g2.drawStr(0, 0, cstr);
    
    pixels.setPixelColor(menuIndex, pixels.Color(0, 0, lightVal)); 
}

void runDataSelection() {

    menuIndex += rotChange;
    
    // data selection bounds

    if (menuIndex > 7)
        menuIndex = 0;

    if (menuIndex < 0)
        menuIndex = 7;

    // display selected data
    
    if (menuIndex == 0)
        u8g2.drawStr(0, 0, "Steps per iteration");
    else if (menuIndex == 1)
        u8g2.drawStr(0, 0, "RPM");
    else if (menuIndex == 2)
        u8g2.drawStr(0, 0, "Adjust bed");
    else if (menuIndex == 3)
        u8g2.drawStr(0, 0, "Adjust rotation");
    else if (menuIndex == 4)
        u8g2.drawStr(0, 0, "Rotation change");
    else if (menuIndex == 5)
        u8g2.drawStr(0, 0, "Pause time");
    else if (menuIndex == 6)
        u8g2.drawStr(0, 0, "Iterations");
    else if (menuIndex == 7) {
    
        u8g2.drawStr(0, 0, "START JOB");

        // display an overview of the settings for this task

        char output[24+1];
        snprintf(output, 30, "%s %d %s %d %s %d %s %d", "S", stepsPerIteration, "R", rpm, "I", iterations, "P", pause);
        u8g2.drawStr(0, 10, output);
    }
    
    // disco disco

    pixels.setPixelColor(menuIndex, pixels.Color(0, lightVal, 0)); 
}

void rotatePixelValues() {

    lightVal += lvChange;

    if (lightVal > 250 || lightVal < 1)
        lvChange *= -1;
}
