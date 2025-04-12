// Micromouse course - evosoft

#include <Arduino.h>
#include "utils/logging.h"
#include "utils/task.h"
#include "hal/micromouse.h"
#include "globals.h"
#include "mouse.h"
#include "control/controller.h"
#include "control/input_handling.h"
#include "control/output_handling.h"

#include "commands/linear_travel_command.h"

using namespace std::chrono_literals;

std::chrono::duration<uint32_t, std::micro> MAIN_CYCLE_TIME = 2ms;
std::chrono::duration<uint32_t, std::micro> DEBUG_CYCLE_TIME = 100ms;

void debug();

#ifdef BLE_LOGGING
BleSerial ble;
#endif

MM::Globals g {};
MM::Mouse mouse {};

MM::Task task_read_sensors        {MM::read_sensors, MAIN_CYCLE_TIME};
MM::Task task_pre_process_inputs  {MM::pre_process_inputs, MAIN_CYCLE_TIME};
MM::Task task_control             {MM::control, MAIN_CYCLE_TIME};
MM::Task task_post_process        {MM::post_process, MAIN_CYCLE_TIME};
MM::Task task_update_outputs      {MM::update_outputs, MAIN_CYCLE_TIME};
MM::Task task_debug               {debug, DEBUG_CYCLE_TIME};


void debug()
{
  mouse.dbg_red.toggle();
  /*LOG_INFO("LEDS-> FL: %d, L: %d, R: %d, FR: %d\n ", g.ir_frontleft,
                                               g.ir_left,
                                               g.ir_right,
                                               g.ir_frontright);

  

  LOG_INFO("BATTERY VOLTAGE: %d\n", g.currentBatteryVoltage);*/

  // LOG_INFO("MOTORS->   MOTOR_LEFT:   %d   MOTOR_RIGHT: %d\n ", g.leftMotorVoltage, g.rightMotorVoltage);
  // LOG_INFO("ENCODERS-> ENCODER LEFT: %d ENCODER RIGHT: %d\n", static_cast<int>( g.leftEncoderValue ), static_cast<int>( g.rightEncoderValue) );
  //LOG_INFO("DIR - ENCODER LEFT: %d ENCODER RIGHT: %d\n", static_cast<int>( mouse.encoder_right.getCount() ), static_cast<int>( mouse.encoder_left.getCount() ) );


  // Linear traveling data
  LOG_INFO("ELAPS_T: %d TOT_T: %d DDIST: %d RDIST: %d ",
    g.currentCommand->getElapsedTime_ms(),
    g.currentCommand->getTotalTime_ms(), 
    static_cast<int> ( g.currentCommand->getDesiredCurrentPosition_um() ),
    static_cast<int> ( g.currentCommand->getRealCurrentPosition_um() )
  ); 

  // Motors
  LOG_INFO("M_LEFT: %d ENC_L: %d M_RIGHT: %d ENC_R: %d \n", g.leftMotorVoltage, static_cast<int>( mouse.motor_left.getEncoderCount() ), 
                                                                    g.rightMotorVoltage, static_cast<int>( mouse.motor_right.getEncoderCount() ) );
}

void setup()
{
  LOGGING_BEGIN();
  mouse.dbg_green.on();
  
  // enable motors
  pinMode(MM::PINS::MOTOR_DRV_EN, OUTPUT);
  digitalWrite(MM::PINS::MOTOR_DRV_EN, true);

  // enable encoders
  ESP32Encoder::useInternalWeakPullResistors = puType::up;

  // turn on leds
  mouse.ir_led1.on();
  mouse.ir_led2.on();
  mouse.ir_led3.on();
  mouse.ir_led4.on();

  // Initializing the balancer controller
  // NOTE: count() will return the value in ns!
  g.myStraightMovementCtrl.init((static_cast<int>(MAIN_CYCLE_TIME.count()) / 1000), AUTOMATIC, -750, 750);

  // Wait 5 sec to be able to connect with mobile!
  unsigned long start_ms = millis();
  unsigned long now_ms = start_ms;
  while( now_ms < start_ms + 5000 )
  {
      now_ms = millis();
  }

  // Create a new linear travel command
  g.currentCommand = new MM::LinearTravelCommand(2000000, 150, 1, 1, g.leftEncoderValue, g.rightEncoderValue, g.leftMotorVoltage, g.rightMotorVoltage);

  LOG_INFO("Setup Done\n");
}

void loop()
{
  task_read_sensors();
  task_pre_process_inputs();
  task_control();
  task_post_process();
  task_update_outputs();
  task_debug();
}