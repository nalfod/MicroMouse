// Micromouse course - evosoft

#include "input_handling.h"
#include "hal/micromouse.h"
#include <Arduino.h>
#include "globals.h"
#include "mouse.h"

void MM::read_sensors()
{
    g.ir_left = mouse.left_dist_measure.get_current_phototransistor_value_mV();
    g.ir_frontleft = mouse.frontleft_dist_measure.get_current_phototransistor_value_mV();
    g.ir_frontright = mouse.frontright_dist_measure.get_current_phototransistor_value_mV();
    g.ir_right = mouse.right_dist_measure.get_current_phototransistor_value_mV();

    g.currentBatteryVoltage = analogRead(MM::PINS::BATT);

    g.rightEncoderValue = mouse.motor_right.getEncoderCount();
    g.leftEncoderValue =  mouse.motor_left.getEncoderCount();

    if( mouse.accelerometer.loadSensorValues() )
    {
        // mouse.accelerometer.serialPrint();
        // mouse.accelerometer.myAngRotMeter.refresh();
        g.currentOrientation = mouse.accelerometer.getCurrentYawValue();
        //Serial.print("Current orientation: ");
        //Serial.println(g.currentOrientation);
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