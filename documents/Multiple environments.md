Yes, you can configure your `platformio.ini` file to support both the **esp32doit-devkit-v1** and an **ESP32-C6** variant, allowing you to manually select which environment to build and upload. This is a common workflow in PlatformIO and is well-supported.

## How to Set Up Multiple Environments in `platformio.ini`

You define a separate `[env:...]` section for each board. For example:

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino

[env:esp32-c6-devkitc-1]
platform = espressif32
board = esp32-c6-devkitc-1
framework = espidf   ; See note below
```

**Manual selection** is then done either through the PlatformIO UI in VS Code (the "Project Tasks" panel lets you pick which environment to build/upload), or from the command line with:

```
pio run -e esp32doit-devkit-v1
pio run -e esp32-c6-devkitc-1
```

## Important Notes

- **Framework Support:** As of now, the ESP32-C6 boards in PlatformIO **do not support the Arduino framework**—only ESP-IDF is available for C6 chips[3]. Attempting to use `framework = arduino` for an ESP32-C6 board will result in an error.
- **Shared Code:** You can keep your code generic by using preprocessor macros to differentiate board-specific logic, for example:

  ```cpp
  #ifdef ARDUINO_ARCH_ESP32
    // ESP32-specific code
  #endif
  ```

- **Default Environment:** You can set a default environment (or multiple) to build by default using:

  ```ini
  [platformio]
  default_envs = esp32doit-devkit-v1
  ```

## Example `platformio.ini`

```ini
[platformio]
default_envs = esp32doit-devkit-v1

[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino

[env:esp32-c6-devkitc-1]
platform = espressif32
board = esp32-c6-devkitc-1
framework = espidf
```

## Workflow

- Open the "PlatformIO" panel in VS Code.
- Under "Project Tasks," you'll see both environments listed.
- Click "Build" or "Upload" under the environment you want to target.
- Alternatively, use the `pio run -e ` or `pio upload -e ` commands in the terminal.

## Summary Table

| Board                    | Frameworks Supported (as of 2025) | How to Select in PlatformIO        |
|--------------------------|-----------------------------------|------------------------------------|
| esp32doit-devkit-v1      | Arduino, ESP-IDF                  | `[env:esp32doit-devkit-v1]`        |
| esp32-c6-devkitc-1       | ESP-IDF only                      | `[env:esp32-c6-devkitc-1]`         |

## References

- Multiple environments in `platformio.ini` are standard and let you select which board to build for[4][5][1].
- ESP32-C6 Arduino support is not yet available in PlatformIO[3].

**In summary:**  
You can serve both boards from one `platformio.ini` using separate environments, and select which to use at build/upload time. Just be aware of the current framework limitations for ESP32-C6 in PlatformIO[3][4][5].

Citations:
[1] https://community.platformio.org/t/platformio-ini-with-env-custom-table-builds-twice/13486
[2] https://community.platformio.org/t/arduino-nano-esp32-multiple-board-project-setup/40171
[3] https://www.reddit.com/r/esp32/comments/1ananz5/esp32_c6_in_platformio_for_arduino_framework/
[4] https://community.platformio.org/t/how-to-use-multiple-target-boards-with-same-project/16041
[5] https://community.platformio.org/t/how-to-use-multiple-board-in-single-project/26323
[6] https://community.platformio.org/t/how-to-build-and-upload-all-when-multiple-environments/39561
[7] https://docs.platformio.org/en/latest/projectconf/index.html
[8] https://community.platformio.org/t/multiple-environments-from-one-project/44858
[9] https://community.platformio.org/t/esp32-board-selection/17331
[10] https://esp32.com/viewtopic.php?t=40275
[11] https://github.com/platformio/platformio-core/issues/1696
[12] https://community.platformio.org/t/config-file-for-multiple-boards-platforms-and-frameworks/11751
[13] https://docs.platformio.org/en/latest/boards/espressif32/esp32doit-devkit-v1.html
[14] https://community.platformio.org/t/how-to-select-sdkconfig-defaults-per-environment/23832
[15] https://community.platformio.org/t/how-do-you-set-up-a-new-board-esp32-s3-n16r8/32306
[16] https://docs.platformio.org/en/latest/platforms/espressif32.html
[17] https://randomnerdtutorials.com/vs-code-platformio-ide-esp32-esp8266-arduino/
[18] https://www.reddit.com/r/esp32/comments/10ofqlx/is_there_a_way_in_platformio_to_build_all/
[19] https://github.com/Photonsters/ESParaSite-ESP32/blob/master/platformio.ini
[20] https://www.instructables.com/Develop-ESP32-With-PlatformIO-IDE/

---
Answer from Perplexity: pplx.ai/share

*************************************
To combine the `platformio.ini` file for both environments and assign different GPIO pins, you'll need to define each environment separately and specify the configurations unique to each board. Here's the combined `platformio.ini` file:

```ini
[env:esp32doit-devkit-v1]
platform = espressif32@^6.7.0
platform_packages = 
    framework-arduinoespressif32 @ https://github.com/espressif/arduino-esp32.git#3.0.1
    framework-arduinoespressif32-libs @ https://github.com/espressif/arduino-esp32/releases/download/3.0.1/esp32-arduino-libs-3.0.1.zip
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200
monitor_filters = send_on_enter
monitor_echo = yes
monitor_eol = LF
upload_speed = 921600
upload_port = 192.168.0.234
upload_protocol = espota
board_build.filesystem = littlefs
build_flags = 
    -D CONFIG_ASYNC_TCP_RUNNING_CORE=1
    -D CONFIG_ASYNC_TCP_STACK_SIZE=4096
    -D GPIO_PIN_MODE_ESP32DOIT=17  ; Example GPIO pin assignment
lib_deps = 
    ESP32Async/AsyncTCP
    ESP32Async/ESPAsyncWebServer
    vrekrer/Vrekrer SCPI parser@^0.5.0
    thomasfredericks/Bounce2@^2.72

[env:esp32-c3-devkitm-1]
platform = espressif32
board = esp32-c3-devkitm-1
framework = arduino
monitor_speed = 115200
upload_speed = 921600
build_flags = 
    -D ARDUINO_USB_MODE=1
    -D ARDUINO_USB_CDC_ON_BOOT=1
    -D GPIO_PIN_MODE_ESP32C3=9    ; Example GPIO pin assignment
```

### How to select the environment
To choose the environment when building, uploading, or monitoring, you can use the `--environment` flag in the PlatformIO CLI. For example:

- To build for `esp32doit-devkit-v1`:
  ```
  pio run --environment esp32doit-devkit-v1
  ```

- To upload to `esp32-c3-devkitm-1`:
  ```
  pio run --target upload --environment esp32-c3-devkitm-1
  ```

When working in an IDE like Visual Studio Code, you can select the environment from the dropdown in the PlatformIO toolbar.

Feel free to let me know if you'd like further assistance customizing your GPIO pins or other configurations!

********************
; Common settings for all environments
[platformio]
default_envs = esp32doit-devkit-v1

[common]
lib_deps = 
    ESP32Async/AsyncTCP
    ESP32Async/ESPAsyncWebServer
    vrekrer/Vrekrer SCPI parser@^0.5.0
    thomasfredericks/Bounce2@^2.72

[env:esp32doit-devkit-v1]
platform = espressif32@^6.7.0
platform_packages = 
    framework-arduinoespressif32 @ https://github.com/espressif/arduino-esp32.git#3.0.1
    framework-arduinoespressif32-libs @ https://github.com/espressif/arduino-esp32/releases/download/3.0.1/esp32-arduino-libs-3.0.1.zip
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200
monitor_filters = send_on_enter
monitor_echo = yes
monitor_eol = LF
upload_speed = 921600
upload_port = 192.168.0.234
upload_protocol = espota
board_build.filesystem = littlefs
build_flags = 
    -D CONFIG_ASYNC_TCP_RUNNING_CORE=1
    -D CONFIG_ASYNC_TCP_STACK_SIZE=4096
    -D GPIO_PIN_MODE_ESP32DOIT=17
lib_deps = ${common.lib_deps}  ; Reference common lib_deps

[env:esp32-c3-devkitm-1]
platform = espressif32
board = esp32-c3-devkitm-1
framework = arduino
monitor_speed = 115200
upload_speed = 921600
build_flags = 
    -D ARDUINO_USB_MODE=1
    -D ARDUINO_USB_CDC_ON_BOOT=1
    -D GPIO_PIN_MODE_ESP32C3=9
lib_deps = ${common.lib_deps}  ; Reference common lib_deps

**********************************
GPIO (General Purpose Input/Output) assignment depends on the specific needs of your project, as well as the hardware features and pin mappings of each processor. Since you have 8 different GPIO pins for each processor, you can assign them in your code using conditional compilation based on the environment.

### Defining GPIO Pins for Each Environment
You can use preprocessor macros (`#define`) in your code and rely on the `build_flags` in your `platformio.ini` file to differentiate the assignments for each processor. Here's an example:

```cpp
#if defined(ESP32DOIT_DEVKIT_V1)
// GPIO assignments for esp32doit-devkit-v1
#define GPIO_PIN_1 17
#define GPIO_PIN_2 18
#define GPIO_PIN_3 19
#define GPIO_PIN_4 21
#define GPIO_PIN_5 22
#define GPIO_PIN_6 23
#define GPIO_PIN_7 25
#define GPIO_PIN_8 26
#elif defined(ESP32_C3_DEVKITM_1)
// GPIO assignments for esp32-c3-devkitm-1
#define GPIO_PIN_1 2
#define GPIO_PIN_2 3
#define GPIO_PIN_3 4
#define GPIO_PIN_4 5
#define GPIO_PIN_5 6
#define GPIO_PIN_6 7
#define GPIO_PIN_7 8
#define GPIO_PIN_8 9
#else
#error "No environment defined for GPIO assignment!"
#endif
```

### Linking GPIO Assignments to PlatformIO
You need to ensure that the `build_flags` in your `platformio.ini` file define unique macros for each environment. For example:

```ini
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
build_flags = 
    -D ESP32DOIT_DEVKIT_V1

[env:esp32-c3-devkitm-1]
platform = espressif32
board = esp32-c3-devkitm-1
framework = arduino
build_flags = 
    -D ESP32_C3_DEVKITM_1
```

### How It Works
1. **Environment Selection**: When you select an environment in PlatformIO (either `esp32doit-devkit-v1` or `esp32-c3-devkitm-1`), the corresponding `build_flags` define macros (`ESP32DOIT_DEVKIT_V1` or `ESP32_C3_DEVKITM_1`).
2. **Conditional Compilation**: The code uses these macros to determine which set of GPIO assignments to include during compilation.
3. **GPIO Usage**: You can use the defined GPIO pins in your code without worrying about mismatched pin mappings between the processors.

Let me know if you'd like further clarification or additional assistance!