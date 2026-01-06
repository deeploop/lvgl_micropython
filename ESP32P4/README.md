# ESP32-P4 LVGL MicroPython Firmware

This directory contains documentation for building LVGL MicroPython firmware for ESP32-P4 based boards.

## ⚠️ Important: MIPI-DSI Display Support Status

**Current Limitation:** Board-specific MIPI-DSI configuration via TOML is **NOT YET SUPPORTED**.

- ✅ MIPI-DSI **C code support exists** (`ext_mod/lcd_bus/esp32_src/dsi_bus.c`)
- ❌ MIPI-DSI **TOML integration is incomplete** (not in `builder/toml_reader.py`)
- ⚠️ Generic firmware with `DISPLAY=all` **may crash** on ESP32-P4 Function EV Board

### Why Generic Firmware May Crash

The ESP32-P4 Function Evaluation Board uses a **MIPI-DSI display** (ili9881c, 800x1280) which requires:
- Exact 2-lane configuration @ 1000 Mbps
- Specific timing parameters (HSYNC/VSYNC)
- Proper panel initialization sequence

Generic firmware tries to initialize ALL display drivers, which can cause:
```
Guru Meditation Error: Core 1 panic'ed (Load access fault)
MEPC: 0x40086948
```

## 🎯 Quick Start - Download Pre-Built Firmware

Pre-built generic firmware is available from GitHub Actions:

1. Go to [Actions Tab](../../actions)
2. Select the latest successful workflow run
3. Download artifacts:
   - **Linux builds:** `lvgl_micropy_ESP32`
   - **Windows builds:** `lvgl_micropy_ESP32_Windows`

### Available Firmware Files

| Firmware File | Flash Size | Description | ESP32-P4 EV Board Status |
|--------------|------------|-------------|-------------------------|
| `ESP32_GENERIC_P4-8.bin` | 8 MB | Generic ESP32-P4, all drivers | ⚠️ May crash |
| `ESP32_GENERIC_P4-C6_WIFI-8.bin` | 8 MB | ESP32-P4 with C6 WiFi | ⚠️ May crash |

## 🔧 Building Firmware Locally

### Generic Build (All Display Drivers)

```bash
# Clone repository
git clone https://github.com/lvgl-micropython/lvgl_micropython
cd lvgl_micropython

# Generic ESP32-P4 with 8MB flash
python3 make.py esp32 BOARD=ESP32_GENERIC_P4 --flash-size=8 DISPLAY=all INDEV=all EXPANDER=all

# Output: build/ESP32_GENERIC_P4-8.bin
```

**⚠️ Warning:** Generic builds include all display drivers and may crash on ESP32-P4 Function EV Board due to MIPI-DSI initialization issues!

## 📦 Flashing Firmware

### Using esptool

```bash
# Erase flash first
python -m esptool --chip esp32p4 --port COM7 erase_flash

# Flash firmware
python -m esptool --chip esp32p4 --port COM7 --baud 921600 write_flash -z 0 build/ESP32_GENERIC_P4-8.bin
```

Replace `COM7` with your serial port:
- **Windows:** `COM3`, `COM7`, etc.
- **Linux:** `/dev/ttyUSB0`, `/dev/ttyACM0`
- **macOS:** `/dev/cu.usbserial-*`

## 🐛 Known Issues & Troubleshooting

### Issue 1: Firmware Crashes on Boot

**Symptom:**
```
Guru Meditation Error: Core 1 panic'ed (Load access fault)
MEPC: 0x40086948
```

**Root Cause:**
- Generic firmware with `DISPLAY=all` tries to initialize all display drivers
- MIPI-DSI (ESP32-P4 EV Board) requires exact hardware-specific configuration
- Display driver initialization fails and crashes before MicroPython starts

**Current Workarounds:**

1. **Use plain MicroPython (no LVGL):**
   - Download from: https://micropython.org/download/ESP32_GENERIC_P4/
   - No display support, but system is stable
   - Can use GPIO, I2C, UART, etc.

2. **Use ESP-IDF with C/C++:**
   - Full MIPI-DSI support in ESP-IDF
   - Espressif provides ESP32-P4 MIPI-DSI examples
   - https://github.com/espressif/esp-idf

3. **Wait for TOML integration:**
   - MIPI-DSI TOML support needs to be added to `builder/toml_reader.py`
   - Track development at: https://github.com/lvgl-micropython/lvgl_micropython

### Issue 2: Display Not Working

If firmware boots but display doesn't work:

**Test backlight:**
```python
from machine import Pin
backlight = Pin(23, Pin.OUT)
backlight.value(1)  # Turn on backlight
```

**Test I2C touch:**
```python
from machine import I2C, Pin
i2c = I2C(0, scl=Pin(8), sda=Pin(7), freq=400000)
devices = i2c.scan()
print("I2C devices:", [hex(d) for d in devices])
# Should see: ['0x5d'] or ['0x14'] (GT911 touch controller)
```

### Issue 3: Build Fails

**ESP-IDF not installed:**
```bash
git submodule update --init --depth 1 --jobs 4 -- lib/esp-idf
cd lib/esp-idf
./install.sh all
. ./export.sh
```

**Missing submodules:**
```bash
git submodule update --init --depth 1 -- lib/pycparser
git submodule update --init --depth 1 --jobs 4 -- lib/micropython
git submodule update --init --depth 1 --jobs 4 -- lib/lvgl
```

## 📋 ESP32-P4 Function EV Board Hardware

### Display (MIPI-DSI)
- **Panel:** ili9881c
- **Resolution:** 800x1280 (physical, portrait)
- **Rotation:** 90° to 800x480 landscape
- **Color:** RGB565 (16-bit)
- **Lanes:** 2 data lanes @ 1000 Mbps each
- **Backlight:** GPIO 23

### Touch (I2C)
- **Controller:** GT911
- **Interface:** I2C0 (GPIO 7/8)
- **Address:** 0x5D or 0x14
- **Points:** Multi-touch (up to 5)

### Memory
- **Flash:** 16 MB (typical)
- **SPIRAM:** 16 MB @ 200 MHz
- **Frame Buffer:** Double buffering in SPIRAM

### Display Timing Parameters
```
HSYNC: Front=40, Back=140, Pulse=40
VSYNC: Front=16, Back=16, Pulse=4
```

## 📚 Additional Documentation

- **[ESP32_P4_HARDWARE_SUMMARY.md](ESP32_P4_HARDWARE_SUMMARY.md)** - Complete hardware specifications
- **[ESP32_P4_MICROPYTHON_HARDWARE_REFERENCE.md](ESP32_P4_MICROPYTHON_HARDWARE_REFERENCE.md)** - MicroPython API reference
- **[ESP32P4_PANEL_CONFIG.md](ESP32P4_PANEL_CONFIG.md)** - Display panel configuration details
- **[lv_conf_esp32p4.h](lv_conf_esp32p4.h)** - LVGL configuration for PC simulator

## 🚀 GitHub Actions

Automated builds run on every push:

### Workflows
- **Unix build** ([unix.yml](../.github/workflows/unix.yml)) - Linux builds
- **Windows build** ([windows.yml](../.github/workflows/windows.yml)) - Windows builds
- **macOS build** ([macOS.yml](../.github/workflows/macOS.yml)) - macOS builds

### Build Matrix

Each workflow builds:
1. ESP32_GENERIC_P4 8MB (Generic - ⚠️ may crash on EV Board)
2. ESP32_GENERIC_P4 C6_WIFI 8MB (Generic with WiFi - ⚠️ may crash)
3. ESP32_GENERIC_S3 variants (Compatibility testing)

## 🔗 Useful Links

- [ESP32-P4 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-p4_datasheet_en.pdf)
- [ESP32-P4 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-p4_technical_reference_manual_en.pdf)
- [EV Board User Guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/user_guide.html)
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/)
- [LVGL Documentation](https://docs.lvgl.io/)
- [MicroPython Documentation](https://docs.micropython.org/)

## 🛠️ Developer Information

### MIPI-DSI Support Status

**C Code Implementation:** ✅ Complete
- Location: `ext_mod/lcd_bus/esp32_src/dsi_bus.c`
- Header: `ext_mod/lcd_bus/esp32_include/dsi_bus.h`
- Supports: 2/4 lane configuration, DPI panel, frame buffers

**TOML Integration:** ❌ Not Implemented
- Location: `builder/toml_reader.py`
- Supported displays: RGBDisplay, SDLDisplay
- Missing: DSIDisplay, MIPIDSIDisplay
- Supported buses: RGBBus, I2C, SPI
- Missing: DSIBus, MIPIDSIBus

**To Add MIPI-DSI TOML Support:**

1. Add DSIBus support to `toml_reader.py`:
   ```python
   if self.name == 'DSIBus':
       return 'dsi_bus.DSIBus'
   ```

2. Add DSIDisplay support:
   ```python
   if self.name == 'DSIDisplay':
       return 'dsi_display.DSIDisplay'
   ```

3. Create Python display driver wrapper for MIPI-DSI panels

4. Test with ESP32-P4 Function EV Board configuration

## 📄 License

Copyright (c) 2024 - 2025 Kevin G. Schlosser

See [LICENSE](../LICENSE) for details.

---

**Status:** Generic ESP32-P4 builds work but may crash with MIPI-DSI displays. Board-specific MIPI-DSI configuration via TOML awaits integration work.
