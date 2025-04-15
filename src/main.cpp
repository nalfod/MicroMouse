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
  /*LOG_INFO("LEDS-> FL: %d, L: %d, R: %d, FR: %d ", g.ir_frontleft,
                                                    g.ir_left,
                                                    g.ir_right,
                                                    g.ir_frontright);*/

  if( !g.commandBuffer.empty() )
  {
    g.commandBuffer.front()->print();
  }
  LOG_INFO("\n");


  //LOG_INFO("BATTERY VOLTAGE: %d\n", g.currentBatteryVoltage);

  // LOG_INFO("MOTORS->   MOTOR_LEFT:   %d   MOTOR_RIGHT: %d\n ", g.leftMotorVoltage, g.rightMotorVoltage);
  // LOG_INFO("ENCODERS-> ENCODER LEFT: %d ENCODER RIGHT: %d\n", static_cast<int>( g.leftEncoderValue ), static_cast<int>( g.rightEncoderValue) );
  //LOG_INFO("DIR - ENCODER LEFT: %d ENCODER RIGHT: %d\n", static_cast<int>( mouse.encoder_right.getCount() ), static_cast<int>( mouse.encoder_left.getCount() ) );

/*
  // wall centering
  LOG_INFO("Wall center output: %d error: %d FL: %d FR: %d\n", ( reinterpret_cast<MM::WallCenteringCommand*>( g.commandBuffer.front().get() )->getPidOutput() ), 
                                                               ( reinterpret_cast<MM::WallCenteringCommand*>( g.commandBuffer.front().get() )->getCurrentError() ),
                                                               ( reinterpret_cast<MM::WallCenteringCommand*>( g.commandBuffer.front().get() )->getFrontLeftSensor() ),
                                                               ( reinterpret_cast<MM::WallCenteringCommand*>( g.commandBuffer.front().get() )->getFrontRightSensor() ) );*/
/*
  // Linear traveling data
  static MM::LinearTravelCommand const * const currentLinearTravelCommand = reinterpret_cast<MM::LinearTravelCommand*> ( reinterpret_cast<MM::WallCenteringCommand*>( g.commandBuffer.front().get() )->getWrappedObjectP() );
  LOG_INFO("TOT_T: %d ELAPS_T: %d DDIST: %d RDIST: %d CURR_SPEED: %d",
    currentLinearTravelCommand->getTotalTime_ms(),
    currentLinearTravelCommand->getElapsedTime_ms(), 
    static_cast<int> ( currentLinearTravelCommand->getDesiredCurrentPosition_um() ),
    static_cast<int> ( currentLinearTravelCommand->getRealCurrentPosition_um() ),
    static_cast<int> ( currentLinearTravelCommand->mCurrentSpeed_UmPerMs )
  );*/ 

  // Motors
  /*LOG_INFO("M_LEFT: %d ENC_L: %d M_RIGHT: %d ENC_R: %d \n", g.leftMotorVoltage, static_cast<int>( mouse.motor_left.getEncoderCount() ), 
                                                            g.rightMotorVoltage, static_cast<int>( mouse.motor_right.getEncoderCount() ) );*/

  // Command queue:
  /*LOG_INFO("QUEUE SIZE: %d EMPTY?: %d \n", g.commandBuffer.size(), static_cast<int>( g.commandBuffer.empty() ) );*/

  // Accelerometer:
  LOG_INFO("RES: %d Ax: %d Ay: %d Az: %d Gx: %d Gy: %d Gz: %d \n", static_cast<int>( g.accel_result ), static_cast<int>( mouse.accelerometer.mAx ), static_cast<int>( mouse.accelerometer.mAy ), static_cast<int>( mouse.accelerometer.mAz ), 
    static_cast<int>( mouse.accelerometer.mGx ), static_cast<int>( mouse.accelerometer.mGy ), static_cast<int>( mouse.accelerometer.mGz ) );
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
      std::make_unique<MM::WallCenteringCommand>( 
      std::make_unique<MM::LinearTravelCommand>( 180000, 100, 1, 1, g.leftEncoderValue, g.rightEncoderValue, g.leftMotorVoltage, g.rightMotorVoltage ), 
      g.ir_frontleft, g.ir_frontright, g.leftMotorVoltage, g.rightMotorVoltage )
    );
  }
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