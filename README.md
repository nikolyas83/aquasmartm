# AquaSmart (PlatformIO project)

[![CI](https://github.com/your-username/your-repo/actions/workflows/ci.yml/badge.svg)](https://github.com/your-username/your-repo/actions/workflows/ci.yml)

Quick notes to build and run this project:

1. Install PlatformIO (VS Code extension) or PlatformIO Core CLI:
   - VS Code: Install "PlatformIO IDE" extension
   - CLI: `pip install -U platformio`

2. From project root run:
   - `pio run` (build)
   - `pio run -t upload` (upload to device)
   - `pio device monitor -b 115200` (open serial monitor)

3. Dependencies:
   - The `platformio.ini` has `lib_deps` entries for required libraries (Async WebServer, AsyncTCP, AsyncOta, OneWire, DallasTemperature, RTClib, ArduinoJson, PCA9685 driver).
   - If a lib fails to be found, verify the exact package name in PlatformIO library registry or use GitHub repo path.

4. Notes and suggestions:
   - `/set-wifi` handler is non-blocking (it starts a connection and replies immediately).
   - PWM values are clamped to the PCA9685 12-bit maximum (4095) to prevent overflow.
   - Consider minimizing dynamic `String` usage in server handlers to avoid heap fragmentation on constrained MCUs.

If you'd like, I can try a build next (if `pio` CLI is available on your system) and address any compile errors.