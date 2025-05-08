#pragma once

#include "globals.h"
#include "mouse.h"
#include "utils/logging.h"

namespace MM {

void log_active_command()
{
    if( !g.commandBuffer.empty() )
    {
      g.commandBuffer.front()->print();
    }
}

void log_current_led_values()
{
    LOG_INFO("LEDS-> FL: %d, L: %d, R: %d, FR: %d \n", g.ir_frontleft,
                                                       g.ir_left,
                                                       g.ir_right,
                                                       g.ir_frontright);
}

void log_current_mm_values()
{
    LOG_INFO("DIST-> FL: %d, L: %d, R: %d, FR: %d \n", g.dist_frontleft_mm,
                                                       g.dist_left_mm,
                                                       g.dist_right_mm,
                                                       g.dist_frontright_mm);
}

void log_current_orientation()
{
    LOG_INFO("ORI: %d\n",static_cast<int>(g.currentOrientation));
}

void log_current_battery_voltage()
{
    LOG_INFO("BATTERY VOLTAGE: %d\n", g.currentBatteryVoltage);
}

void log_current_motor_voltages_and_revs()
{
    LOG_INFO("M_LEFT: %d ENC_L: %d M_RIGHT: %d ENC_R: %d \n", g.leftMotorVoltage, static_cast<int>( g.leftEncoderValue ), 
                                                              g.rightMotorVoltage, static_cast<int>( g.rightEncoderValue ) );
}

void log_command_queue_state()
{
    LOG_INFO("QUEUE SIZE: %d EMPTY?: %d \n", g.commandBuffer.size(), static_cast<int>( g.commandBuffer.empty() ) );
}

// for this dont forget to call the mouse.accelerometer.myAngRotMeter.startMeasurement() function
void log_angular_velocity_values()
{
    LOG_INFO("ANG_VEL: %d CURRANG: %d HALFCIRC: %d\n"
        , static_cast<int>( mouse.accelerometer.myAngRotMeter.getAngularVelocity_miliRevPerS() )
        , static_cast<int>( mouse.accelerometer.myAngRotMeter.currentYawValueR * 57 )
        , static_cast<int>( mouse.accelerometer.myAngRotMeter.traveledHalfCircles ) );
}

void log_mode_selector()
{
    LOG_INFO("CURRENT_MODE: %d \n", static_cast<int>( g.mode_selector.get_current_mode() ) );
}



} // namespace MM