// Micromouse course - evosoft

#include <Arduino.h>
#include "utils/logging.h"
#include "utils/task.h"
#include "hal/micromouse.h"
#include "globals.h"
#include "constants.h"
#include "mouse.h"
#include "control/controller.h"
#include "control/input_handling.h"
#include "control/output_handling.h"

#include "commands/linear_travel_command.h"
#include "commands/wall_centering_command.h"

void debug();

#ifdef BLE_LOGGING
BleSerial ble;
#endif

MM::Globals g {};
MM::Mouse mouse {};

MM::Task task_read_sensors        {MM::read_sensors, CONSTS::MAIN_CYCLE_TIME};
MM::Task task_pre_process_inputs  {MM::pre_process_inputs, CONSTS::MAIN_CYCLE_TIME};
MM::Task task_control             {MM::control, CONSTS::MAIN_CYCLE_TIME};
MM::Task task_post_process        {MM::post_process, CONSTS::MAIN_CYCLE_TIME};
MM::Task task_update_outputs      {MM::update_outputs, CONSTS::MAIN_CYCLE_TIME};
MM::Task task_debug               {debug, CONSTS::DEBUG_CYCLE_TIME};


void debug()
{
  mouse.dbg_red.toggle();
  LOG_INFO("LEDS-> FL: %d, L: %d, R: %d, FR: %d\n ", g.ir_frontleft,
                                                    g.ir_left,
                                                    g.ir_right,
                                                    g.ir_frontright);

  

  //LOG_INFO("BATTERY VOLTAGE: %d\n", g.currentBatteryVoltage);

  // LOG_INFO("MOTORS->   MOTOR_LEFT:   %d   MOTOR_RIGHT: %d\n ", g.leftMotorVoltage, g.rightMotorVoltage);
  // LOG_INFO("ENCODERS-> ENCODER LEFT: %d ENCODER RIGHT: %d\n", static_cast<int>( g.leftEncoderValue ), static_cast<int>( g.rightEncoderValue) );
  //LOG_INFO("DIR - ENCODER LEFT: %d ENCODER RIGHT: %d\n", static_cast<int>( mouse.encoder_right.getCount() ), static_cast<int>( mouse.encoder_left.getCount() ) );


  // wall centering
  LOG_INFO("Wall center output: %d error: %d FL: %d FR: %d\n", ( reinterpret_cast<MM::WallCenteringCommand*>( g.currentCommand )->getPidOutput() ), 
                                                               ( reinterpret_cast<MM::WallCenteringCommand*>( g.currentCommand )->getCurrentError() ),
                                                               ( reinterpret_cast<MM::WallCenteringCommand*>( g.currentCommand )->getFrontLeftSensor() ),
                                                               ( reinterpret_cast<MM::WallCenteringCommand*>( g.currentCommand )->getFrontRightSensor() ) );

/*  // Linear traveling data
  static MM::LinearTravelCommand const * const currentLinearTravelCommand = reinterpret_cast<MM::LinearTravelCommand*> ( reinterpret_cast<MM::WallCenteringCommand*>( g.currentCommand )->getWrappedObjectP() );
  LOG_INFO("ELAPS_T: %d TOT_T: %d DDIST: %d RDIST: %d ",
    currentLinearTravelCommand->getElapsedTime_ms(),
    currentLinearTravelCommand->getTotalTime_ms(), 
    static_cast<int> ( currentLinearTravelCommand->getDesiredCurrentPosition_um() ),
    static_cast<int> ( currentLinearTravelCommand->getRealCurrentPosition_um() )
  ); */

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

  // Wait 5 sec to be able to connect with mobile!
  unsigned long start_ms = millis();
  unsigned long now_ms = start_ms;
  while( now_ms < start_ms + 5000 )
  {
      now_ms = millis();
  }

  // Create a new linear travel command
  MM::MotionCommandIF* linTravelCommand = new MM::LinearTravelCommand(2000000, 150, 1, 1, g.leftEncoderValue, g.rightEncoderValue, g.leftMotorVoltage, g.rightMotorVoltage);
  MM::MotionCommandIF* wallCenteringCommand = new MM::WallCenteringCommand( linTravelCommand, g.ir_frontleft, g.ir_frontright, g.leftMotorVoltage, g.rightMotorVoltage);

  g.currentCommand = wallCenteringCommand;

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