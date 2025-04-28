// Micromouse course - evosoft

#include "input_handling.h"
#include "hal/micromouse.h"
#include <Arduino.h>
#include "globals.h"
#include "mouse.h"

void MM::read_sensors()
{
    measure_distances_alternately();

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

void MM::measure_distances_alternately()
{
    static bool phase_changer = true;
    if( phase_changer )
    {
        g.ir_left = mouse.left_dist_measure.get_current_phototransistor_value_mV();
        g.mm_left = mouse.left_dist_measure.get_current_distance_mm();
        g.ir_right = mouse.right_dist_measure.get_current_phototransistor_value_mV();
        g.mm_right = mouse.right_dist_measure.get_current_distance_mm();
        
        mouse.left_dist_measure.turn_off_led();
        mouse.right_dist_measure.turn_off_led();

        mouse.frontright_dist_measure.turn_on_led();
        mouse.frontleft_dist_measure.turn_on_led();
    }
    else
    {
        g.ir_frontleft = mouse.frontleft_dist_measure.get_current_phototransistor_value_mV();
        g.mm_frontleft = mouse.frontleft_dist_measure.get_current_distance_mm();
        g.ir_frontright = mouse.frontright_dist_measure.get_current_phototransistor_value_mV();
        g.mm_frontright = mouse.frontright_dist_measure.get_current_distance_mm();

        mouse.frontleft_dist_measure.turn_off_led();
        mouse.frontright_dist_measure.turn_off_led();

        mouse.left_dist_measure.turn_on_led();
        mouse.right_dist_measure.turn_on_led();
    }
    phase_changer = !phase_changer;
}