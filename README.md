# MM Framework

This project is intented to provide a basis for FW development on the evosoft micromouse KIT.


## Getting started

The project is written using PlatformIO with Arduino framework.
You can read more about PlatformIO here:
[PlatformIO](https://platformio.org/)

PlatformIO can be installed as an [extension](https://platformio.org/install/ide?install=vscode) in VSCode;


## Project structure

PlatformIO can be configured with the `platformio.ini` file. Two configurations are provided in this project. `Release` is intended for the final version of the code. In this configuraion debug features such as logging are disabled. The `Debug` target is intended for development. In this target loggign is enabled.

## Logging

In the `Debug` target the LOG_xxx macros will output the log message on the selected logging channel. Two channels are supported: UART or Bluetooth Low Energy (BLE). BLE can be enabled by defining the BLE_LOGGING macro. This is done in `platfromio.ini`:

```ini
[env:debug]
build_flags =
    -D LOGGING_ENABLED
    -D BLE_LOGGING # Comment out for UART logging
```

When BLE logging is enabled the device's name can be configured in `utils/logging.h`:

```cpp
ble.begin("EvoMouse");
```

The following macros are supported:

```cpp
LOGGING_BEGIN(...)
LOG_FATAL(...)
LOG_ERROR(...)
LOG_WARNING(...)
LOG_INFO(...)
LOG_TRACE(...)
LOG_DEBUG(...)
```

All of these macros take `printf()` like formatting.


## Hardware abstraction

For convinience the evosoft micromouse KIT's pinout is defined in `hal/micromouse.h`

Furthermore, a global structure, `Mouse` is provided in `mouse.h` file. This is intended to encpsulate all HW component drives of the mouse, such as IR sensor, motors, etc. As an example a simple LED driver is implemented for the debug LEDs.


## Global variables

For variables that are used across multiple compnents a structue named `Globals` is used, with the instantiation `g` in `main.cpp`. This can be used e.g. to read in sensor values, store internal states and calculated output values and log these in the `debug` function in `main.cpp`.


## Task utility

For easier periodic function calls the project provides the `Task` class. The constructor of this class takes a function, a period and an optinal start offset as arguments. The object should be called periodically in the `loop()` function and if the defined period has elapsed it will call the specified callback.

```cpp
void debug() {}

MM::Task task_debug {debug, 500ms};

void loop()
{
    debug();
}
```

The predefined tasks are:

- task_read_sensors: intented for reading in sensor data
- task_pre_process_inputs: intended for processing sensor data, e.g. filtering
- task_control: the main control logic
- task_post_process: intented for post_processing the outputs of the control logic, e.g. mapping to a different range
- task_update_outputs: updating the outputs
- task_debug:  intended for debug features, e.g. logging

