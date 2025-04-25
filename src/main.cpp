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
#include "commands/rotation_command_ff.h"
#include "commands/rotation_command_pid.h"

#include "misc/debug_functions.h"

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
  MM::log_current_led_values();
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

  // turn on leds
  mouse.ir_led1.on();
  mouse.ir_led2.on();
  mouse.ir_led3.on();
  mouse.ir_led4.on();

  // Wait 5 sec to be able to connect with mobile!
  delay(5000);

  // Creating initial commands
  for( int i = 0; i < 5; i++)
  {
    g.commandBuffer.push( 
      std::make_unique<MM::CollisionAvoidanceCommand>
      ( 
        std::make_unique<MM::WallCenteringCommand>
        ( 
          std::make_unique<MM::LinearTravelCommand>
          ( 
            180000, 100, 1, 1, g.leftEncoderValue, g.rightEncoderValue, g.leftMotorVoltage, g.rightMotorVoltage
          ), 
          g.ir_frontleft, g.ir_frontright, g.currentOrientation, g.leftMotorVoltage, g.rightMotorVoltage 
        ),
        g.ir_left, g.ir_right, g.leftMotorVoltage, g.rightMotorVoltage
      )
    );

    g.commandBuffer.push
    ( 
      std::make_unique<MM::RotationCommandPid>( 90, g.currentOrientation, g.leftMotorVoltage, g.rightMotorVoltage)
    );
  }

  //mouse.accelerometer.myAngRotMeter.startMeasurement();
  LOG_INFO("Setup Done\n");
}

void loop()
{
  if( MM::Accelerometer::MPUInterrupt )
  {
      Serial.print(millis());
      Serial.print("\t");
      task_read_sensors();
      task_pre_process_inputs();
      task_control();
      task_post_process();
      task_update_outputs();
      task_debug();
      MM::Accelerometer::MPUInterrupt = false;
  }  
}