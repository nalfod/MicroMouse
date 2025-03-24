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

MM::Task task_read_sensors        {MM::read_sensors, 1s};
MM::Task task_pre_process_inputs  {MM::pre_process_inputs, 1s};
MM::Task task_control             {MM::control, 1s};
MM::Task task_post_process        {MM::post_process, 1s};
MM::Task task_update_outputs      {MM::update_outputs, 1s};
MM::Task task_debug               {debug, 500ms};


void debug()
{
  mouse.dbg_red.toggle();
  LOG_INFO("FL: %d, L: %d, R: %d, FR: %d\n, ", g.ir_frontleft,
                                               g.ir_left,
                                               g.ir_right,
                                               g.ir_frontright);
}

void setup()
{
  LOGGING_BEGIN();
  mouse.dbg_green.on();
  LOG_INFO("Setup Done\n");

  mouse.ir_led1.on();
  mouse.ir_led2.on();
  mouse.ir_led3.on();
  mouse.ir_led4.on();
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