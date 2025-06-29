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

#include "commands/collision_avoidance_command.h"
#include "commands/linear_travel_command.h"
#include "commands/wall_centering_command.h"
#include "commands/rotation_command_pid.h"

#include "misc/debug_functions.h"

#include <vector>

void debug();

#ifdef BLE_LOGGING
BleSerial ble;
#endif

MM::Globals g {};
MM::Mouse mouse {};

MM::Task task_read_sensors        {MM::read_sensors, CONSTS::MAIN_CYCLE_TIME};
MM::Task task_measure_distances   {MM::measure_distances_alternately, CONSTS::DIST_MEASURE_CYCLE_TIME};
MM::Task task_update_battery      {MM::update_battery_voltage, CONSTS::BATTERY_CHECK_TIME};
MM::Task task_pre_process_inputs  {MM::pre_process_inputs, CONSTS::MAIN_CYCLE_TIME};
MM::Task task_control             {MM::control, CONSTS::MAIN_CYCLE_TIME};
MM::Task task_post_process        {MM::post_process, CONSTS::MAIN_CYCLE_TIME};
MM::Task task_update_outputs      {MM::update_outputs, CONSTS::MAIN_CYCLE_TIME};
MM::Task task_debug               {debug, CONSTS::DEBUG_CYCLE_TIME};

std::vector<MM::Task> task_signal_current_mode = {
  {[]() { mouse.dbg_green.toggle(); }, CONSTS::IDLE_BLINK_TIME},
  {[]() { mouse.dbg_green.toggle(); }, CONSTS::SPEED_RUN_BLINK_TIME},
  {[]() { mouse.dbg_green.toggle(); }, CONSTS::DISCOVERY_BLINK_TIME},
  {[]() { mouse.dbg_green.toggle(); }, CONSTS::MEASUREMENT_BLINK_TIME},
  {[]() { mouse.dbg_green.toggle(); }, CONSTS::MEASUREMENT_BLINK_TIME},
  {[]() { mouse.dbg_green.toggle(); }, CONSTS::MEASUREMENT_BLINK_TIME}
};

void debug()
{
  //MM::log_current_mm_values();
  mouse.dbg_red.toggle();
  if( g.mode_selector.get_current_mode() != CONSTS::MODES::MEASUREMENT && g.mode_selector.get_current_mode() != CONSTS::MODES::MEASUREMENT_SNAPSHOT  )
  {
    // MM::log_current_mm_values();
    // MM::log_mode_selector();
    //MM::log_active_command();
    //MM::log_current_motor_voltages_and_revs();
    // MM::log_current_orientation();
  }
  else
  {
    MM::log_current_mm_values();
    static bool new_snapshot = false;
    if( g.mode_selector.get_current_mode() == CONSTS::MODES::MEASUREMENT_SNAPSHOT )
    {
      if( !new_snapshot )
      {
        new_snapshot = true;
        LOG_INFO("NEXT SNAPSHOTS-->\n");
      }
      MM::log_current_mm_values();
    }
    else
    {
      new_snapshot = false;
    }
  }

}

void setup()
{
  LOGGING_BEGIN();
  mouse.dbg_green.on();

  // Serial monitor setup
  Serial.begin(38400); //Initializate Serial wo work well at 8MHz/16MHz
  
  // enable motors
  pinMode(MM::PINS::MOTOR_DRV_EN, OUTPUT);
  digitalWrite(MM::PINS::MOTOR_DRV_EN, true);

  // enable encoders
  ESP32Encoder::useInternalWeakPullResistors = puType::up;

  // enable I2C
  MM::Accelerometer::setupI2C();
  mouse.dbg_green.off();

  if( !mouse.accelerometer.init() ){
    Serial.println("MPU6050 connection failed");
  }
  else{
    Serial.println("MPU6050 connection successful");
    mouse.dbg_green.on();
  }

  // Wait 5 sec to be able to connect with mobile!
  delay(5000);

  if( g.mode_selector.get_current_mode() == CONSTS::MODES::TESTING )
  {
      // g.commandExecuter.parseRouteForSpeedRun("NEEEEEENNNNN");
      // g.commandExecuter.parseRouteForSpeedRunWithDiagonals("NNNNNNEEEENEESWSSESWSSESWWNNN");
  }
  //mouse.accelerometer.myAngRotMeter.startMeasurement();
  LOG_INFO("Setup Done\n");
}

void loop()
{
  if( MM::Accelerometer::MPUInterrupt )
  {
      // for debugging the cycle time
      //Serial.print("Current time: ");
      //Serial.print(millis());
      //Serial.print("\t");
      task_read_sensors();
      task_measure_distances();
      task_update_battery();
      //task_pre_process_inputs();
      task_control();
      task_post_process();
      task_update_outputs();
      task_debug();
      task_signal_current_mode[ g.mode_selector.get_current_mode() ]();
      MM::Accelerometer::MPUInterrupt = false;
  }  
}