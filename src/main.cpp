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

using namespace std::chrono_literals;

void debug();

#ifdef BLE_LOGGING
BleSerial ble;
#endif

MM::Globals g {};
MM::Mouse mouse {};

MM::Task task_read_sensors        {MM::read_sensors, 2ms};
MM::Task task_pre_process_inputs  {MM::pre_process_inputs, 2ms};
MM::Task task_control             {MM::control, 2ms};
MM::Task task_post_process        {MM::post_process, 2ms};
MM::Task task_update_outputs      {MM::update_outputs, 2ms};
MM::Task task_debug               {debug, 100ms};


void debug()
{
  mouse.dbg_red.toggle();
  /*LOG_INFO("FL: %d, L: %d, R: %d, FR: %d\n ", g.ir_frontleft,
                                               g.ir_left,
                                               g.ir_right,
                                               g.ir_frontright);

  LOG_INFO("MOTOR_LEFT: %d, MOTOR_RIGHT: %d\n ", g.pwmPercentLeft,
                                                 g.pwmPercentRight);

  LOG_INFO("BATTERY VOLTAGE: %d\n", g.currentBatteryVoltage);
  
  LOG_INFO("OUTPUT: %d, errorBetweenSides: %d, result: %d\n ", static_cast<int>( g.output ),
                                                  static_cast<int>( g.errorBetweenSides ),
                                                  static_cast<int>( g.result ));*/

  LOG_INFO("ENCODER1              %d       ,  ENCODER2             %d      \n", static_cast<int>(g.encoder1.getCount()),  static_cast<int>(g.encoder2.getCount()));
}

void setup()
{
  LOGGING_BEGIN();
  mouse.dbg_green.on();
  LOG_INFO("Setup Done\n");

  pinMode(MM::PINS::MOTOR_DRV_EN, OUTPUT);
  digitalWrite(MM::PINS::MOTOR_DRV_EN, true);

  mouse.ir_led1.on();
  mouse.ir_led2.on();
  mouse.ir_led3.on();
  mouse.ir_led4.on();

  g.myStraightMovementCtrl.SetSampleTime(1);
  g.myStraightMovementCtrl.SetMode(AUTOMATIC);
  g.myStraightMovementCtrl.SetOutputLimits(-255,255);

  ESP32Encoder::useInternalWeakPullResistors = puType::up;

  g.encoder1.attachHalfQuad(MM::PINS::MOTOR_1_ENC_A, MM::PINS::MOTOR_1_ENC_B);
  g.encoder2.attachHalfQuad(MM::PINS::MOTOR_2_ENC_A, MM::PINS::MOTOR_2_ENC_B);
  g.encoder1.clearCount();
  g.encoder2.clearCount();
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