// Micromouse course - evosoft

#include "input_handling.h"
#include "hal/micromouse.h"
#include <Arduino.h>
#include "globals.h"
#include "mouse.h"

void MM::read_sensors()
{
    g.ir_left = 0;
    unsigned long start = millis();
    mouse.left_dist_measure.turn_on_led();
    Serial.print("Start time: ");
    Serial.print(start);
    Serial.print("\t");
    while( g.ir_left < 1400 )
    {
        g.ir_left = mouse.left_dist_measure.get_current_phototransistor_value_mV();
    }
    Serial.print("End time: ");
    Serial.print(millis());
    Serial.println();
    //measure_distances_alternately();
    mouse.left_dist_measure.turn_off_led();

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
        // Serial.println("No IMU data, weird");
    }
}

void MM::pre_process_inputs()
{
    // Your code comes here
}

void MM::measure_distances()
{
    mouse.left_dist_measure.turn_on_led();
    mouse.frontright_dist_measure.turn_on_led();

    g.ir_left = mouse.left_dist_measure.get_current_phototransistor_value_mV();
    mouse.left_dist_measure.turn_off_led();
    g.ir_frontright = mouse.frontright_dist_measure.get_current_phototransistor_value_mV();
    mouse.frontright_dist_measure.turn_off_led();

    mouse.frontleft_dist_measure.turn_on_led();
    mouse.right_dist_measure.turn_on_led();

    g.ir_frontleft = mouse.frontleft_dist_measure.get_current_phototransistor_value_mV();
    mouse.frontleft_dist_measure.turn_off_led();
    g.ir_right = mouse.right_dist_measure.get_current_phototransistor_value_mV();
    mouse.right_dist_measure.turn_off_led();
}

void MM::measure_distances_alternately()
{
    static bool phase_changer = true;
    if( phase_changer )
    {
        g.ir_left = mouse.left_dist_measure.get_current_phototransistor_value_mV();
        g.ir_right = mouse.right_dist_measure.get_current_phototransistor_value_mV();
        
        mouse.left_dist_measure.turn_off_led();
        mouse.right_dist_measure.turn_off_led();

        mouse.frontright_dist_measure.turn_on_led();
        mouse.frontleft_dist_measure.turn_on_led();
    }
    else
    {
        g.ir_frontleft = mouse.frontleft_dist_measure.get_current_phototransistor_value_mV();
        g.ir_frontright = mouse.frontright_dist_measure.get_current_phototransistor_value_mV();

        mouse.frontleft_dist_measure.turn_off_led();
        mouse.frontright_dist_measure.turn_off_led();

        mouse.left_dist_measure.turn_on_led();
        mouse.right_dist_measure.turn_on_led();
    }
    phase_changer = !phase_changer;
}