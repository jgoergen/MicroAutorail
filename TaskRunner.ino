void startTask() {

    // reset values

    stepper.setSpeed(rpm);
    iteration = 0;
    newRotationOffset = 0;
    rotationOffset = 0;

    // chanage 'model lamp' to all white

    whiteOutPixels();
    pixels.show();

    u8g2.clearBuffer();
    u8g2.drawStr(0, 8, "RUNNING TASK...");
    u8g2.sendBuffer();

    // start running task

    taskRunning = true;
}

void runTask() {

    u8g2.clearBuffer();

    // have we run all the iterations for this job? or has the cancel button been pressed?
    
    if (iteration >= iterations || digitalRead(ROTARY_ENCODER_BUTTON_PIN) == LOW) {

        // notify the user
        
        u8g2.drawStr(0, 8, "Resetting...");
        u8g2.sendBuffer();

        // reset the stepper motor
        
        stepper.step(iterations * stepsPerIteration);

        // reset the servo motor
        
        if (rotationChange != 0) {
            
            if (rotationOffset < 0) {
                
                while (rotationOffset < 0) {
        
                    rotationOffset ++;
                    rotationServo.write(rotationPosition + rotationOffset);
                    delay(SERVO_MOVE_DELAY);
                } 
                
            } else {
                
                while (rotationOffset > 0) {
        
                    rotationOffset --;
                    rotationServo.write(rotationPosition + 0);
                    delay(SERVO_MOVE_DELAY);
                } 
            } 
            
            rotationOffset = 0;
            newRotationOffset = 0;
        }

        // stop the running task
        
        taskRunning = false;
        delay(1000);
        return;
    }

    u8g2.drawStr(0, 8, "RUNNING JOB...");
    u8g2.sendBuffer();
    
    // TODO: show iterations left

    // increment stepper motor position

    stepper.step(stepsPerIteration * -1);

    // increment servo motor position
    // NOTE: in order to keep ther servo from jerking the subject too fast, I use a while loop and a delay to slowly move the servo towards it's target position
    
    if (rotationChange != 0) {
        
        newRotationOffset += rotationChange;

        if (rotationOffset < newRotationOffset) {
            
            while (rotationOffset < newRotationOffset) {

                rotationOffset ++;
                rotationServo.write(rotationPosition + rotationOffset);
                delay(SERVO_MOVE_DELAY);
            } 
        
        } else {
            
            while (rotationOffset > newRotationOffset) {

                rotationOffset --;
                rotationServo.write(rotationPosition + rotationOffset);
                delay(SERVO_MOVE_DELAY);
            } 
        }
    }

    // wait a bit to cut down on vibration

    delay(prePhotoPause);

    // trigger the camera
    
    triggerCamera();

    // wait a bit to let the flash rest 

    delay(pause);

    iteration ++;
}
