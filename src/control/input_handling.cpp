// Micromouse course - evosoft

#include "input_handling.h"
#include "hal/micromouse.h"
#include <Arduino.h>
#include "globals.h"
#include "mouse.h"

void MM::read_sensors()
{
    g.ir_left = analogRead(MM::PINS::IR_1);
    g.ir_frontleft = analogRead(MM::PINS::IR_2);
    g.ir_frontright = analogRead(MM::PINS::IR_3);
    g.ir_right = analogRead(MM::PINS::IR_4);

    g.currentBatteryVoltage = analogRead(MM::PINS::BATT);

    g.rightEncoderValue = mouse.motor_right.getEncoderCount();
    g.leftEncoderValue =  mouse.motor_left.getEncoderCount();

    if( mouse.accelerometer.loadSensorValues() )
    {
        // mouse.accelerometer.serialPrint();
        // mouse.accelerometer.myAngRotMeter.refresh();
        g.currentOrientation = mouse.accelerometer.getCurrentYawValue();
        Serial.print("Current orientation: ");
        Serial.println(g.currentOrientation);
    }
    else
    {
        Serial.println("No IMU data, weird");
    }
}

void MM::pre_process_inputs()
{
    // Your code comes here
}