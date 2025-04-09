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

std::chrono::duration<uint32_t, std::micro> MAIN_CYCLE_TIME = 2ms;
std::chrono::duration<uint32_t, std::micro> DEBUG_CYCLE_TIME = 500ms;

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
  LOG_INFO("LEDS-> FL: %d, L: %d, R: %d, FR: %d\n ", g.ir_frontleft,
                                               g.ir_left,
                                               g.ir_right,
                                               g.ir_frontright);

  LOG_INFO("MOTORS-> MOTOR_LEFT: %d, MOTOR_RIGHT: %d\n ", g.leftMotorVoltage,
                                                          g.rightMotorVoltage);

  LOG_INFO("BATTERY VOLTAGE: %d\n", g.currentBatteryVoltage);
}

void setup()
{
  LOGGING_BEGIN();
  mouse.dbg_green.on();
  LOG_INFO("Setup Done\n");

  // enable motors
  pinMode(MM::PINS::MOTOR_DRV_EN, OUTPUT);
  digitalWrite(MM::PINS::MOTOR_DRV_EN, true);

  mouse.ir_led1.on();
  mouse.ir_led2.on();
  mouse.ir_led3.on();
  mouse.ir_led4.on();

  g.myStraightMovementCtrl.init(1, AUTOMATIC, -1000, 1000);
  //g.myStraightMovementCtrl.init((static_cast<int>(MAIN_CYCLE_TIME.count()) - 1), AUTOMATIC, -1000, 1000);
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