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