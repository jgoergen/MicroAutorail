void startTask() {

    // reset values

    stepper.setSpeed(rpm);
    iteration = 0;

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

    // rpm stepsPerIteration stepsPerRotation iterations pause

    if (iteration >= iterations) {

        stepper.step(iterations * stepsPerIteration);
        taskRunning = false;
        return;
    }
    
    // TODO: show iterations left

    // TODO: allow rotary encoder button press to cancel task

    // increment motor position

    stepper.step(stepsPerIteration * -1);

    // wait a bit to cut down on vibration

    delay(prePhotoPause);

    // trigger the camera
    
    triggerCamera();

    // wait a bit to let the flash rest 

    delay(pause);

    iteration ++;
}
