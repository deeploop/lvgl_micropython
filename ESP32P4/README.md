# ESP32-P4 LVGL MicroPython Firmware

This directory contains documentation and configuration files for building LVGL MicroPython firmware for ESP32-P4 based boards.

## 🎯 Quick Start - Download Pre-Built Firmware

Pre-built firmware is available from GitHub Actions:

1. Go to [Actions Tab](../../actions)
2. Select the latest successful workflow run
3. Download artifacts:
   - **Linux builds:** `lvgl_micropy_ESP32`
   - **Windows builds:** `lvgl_micropy_ESP32_Windows`

### Available Firmware Files

| Firmware File | Description | Board | Display |
|--------------|-------------|-------|---------|
| `ESP32_GENERIC_P4-16.bin` | ESP32-P4 Function EV Board | ESP32-P4 | MIPI-DSI ili9881c |
| `ESP32_GENERIC_P4-8.bin` | Generic ESP32-P4 | ESP32-P4 | Generic/All |
| `ESP32_GENERIC_P4-C6_WIFI-8.bin` | ESP32-P4 with C6 WiFi | ESP32-P4 | Generic/All |

**⚠️ Important:** For ESP32-P4 Function Evaluation Board, use `ESP32_GENERIC_P4-16.bin` which has proper MIPI-DSI configuration!

## 🔧 Building Firmware Locally

### Option 1: Using TOML Configuration (Recommended)

Build with board-specific configuration for ESP32-P4 Function EV Board:

```bash
# Clone repository
git clone https://github.com/lvgl-micropython/lvgl_micropython
cd lvgl_micropython

# Build with ESP32-P4 Function EV Board configuration
python3 make.py --toml=display_configs/ESP32-P4-Function-EV-Board.toml

# Output: build/ESP32_GENERIC_P4-16.bin
```

This builds firmware specifically configured for:
- **Display:** ili9881c MIPI-DSI (800x1280 rotated to 800x480)
- **Touch:** GT911 I2C
- **Flash:** 16MB
- **Backlight:** GPIO 23
- **I2C:** GPIO 7 (SDA), GPIO 8 (SCL)

### Option 2: Generic Build

Build generic firmware with all display drivers:

```bash
# Generic ESP32-P4 with 8MB flash
python3 make.py esp32 BOARD=ESP32_GENERIC_P4 --flash-size=8 DISPLAY=all INDEV=all EXPANDER=all

# With C6 WiFi variant
python3 make.py esp32 BOARD=ESP32_GENERIC_P4 --flash-size=8 BOARD_VARIANT=C6_WIFI DISPLAY=all INDEV=all EXPANDER=all

# Output: build/ESP32_GENERIC_P4-8.bin
```

**⚠️ Warning:** Generic builds may crash on ESP32-P4 Function EV Board due to incompatible display initialization!

## 📦 Flashing Firmware

### Using esptool

```bash
# Erase flash first
python -m esptool --chip esp32p4 --port COM7 erase_flash

# Flash firmware
python -m esptool --chip esp32p4 --port COM7 --baud 921600 write_flash -z 0 build/ESP32_GENERIC_P4-16.bin
```

Replace `COM7` with your serial port:
- **Windows:** `COM3`, `COM7`, etc.
- **Linux:** `/dev/ttyUSB0`, `/dev/ttyACM0`
- **macOS:** `/dev/cu.usbserial-*`

### Verify Flash

```bash
# Connect to MicroPython REPL
mpremote connect COM7

# You should see:
# MicroPython vX.XX on YYYY-MM-DD; ESP32-P4 module with ESP32P4
# >>>
```

## 🐛 Troubleshooting

### Problem: Crash on Boot (Guru Meditation Error)

**Symptom:**
```
Guru Meditation Error: Core 1 panic'ed (Load access fault)
MEPC: 0x40086948
```

**Cause:** Using generic firmware without proper MIPI-DSI configuration

**Solution:**
1. Use the board-specific firmware built with TOML: `ESP32_GENERIC_P4-16.bin`
2. OR build with: `python3 make.py --toml=display_configs/ESP32-P4-Function-EV-Board.toml`

### Problem: Display Not Working

**Checklist:**
- ✅ Using correct firmware (`ESP32_GENERIC_P4-16.bin`)
- ✅ Backlight is configured (GPIO 23)
- ✅ MIPI-DSI is properly initialized
- ✅ Touch controller is detected on I2C

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

### Problem: Build Fails

**Common Issues:**

1. **ESP-IDF not installed:**
   ```bash
   git submodule update --init --depth 1 --jobs 4 -- lib/esp-idf
   cd lib/esp-idf
   ./install.sh all
   . ./export.sh
   ```

2. **Missing submodules:**
   ```bash
   git submodule update --init --depth 1 -- lib/pycparser
   git submodule update --init --depth 1 --jobs 4 -- lib/micropython
   git submodule update --init --depth 1 --jobs 4 -- lib/lvgl
   ```

3. **Python dependencies:**
   ```bash
   pip install pyparsing pycparser
   ```

## 📋 Board Configuration Details

The ESP32-P4 Function EV Board configuration (`display_configs/ESP32-P4-Function-EV-Board.toml`) includes:

### Display (MIPI-DSI)
- **Panel:** ili9881c
- **Resolution:** 800x1280 (physical, portrait)
- **Logical:** 800x480 (rotated 90° to landscape)
- **Color:** RGB565 (16-bit)
- **Lanes:** 2 data lanes @ 1000 Mbps each
- **Backlight:** GPIO 23

### Touch (I2C)
- **Controller:** GT911
- **Interface:** I2C0 (GPIO 7/8)
- **Address:** 0x5D or 0x14
- **Points:** Multi-touch (up to 5)

### Memory
- **Flash:** 16 MB
- **SPIRAM:** 16 MB @ 200 MHz (HEX mode)
- **Frame Buffer:** Double buffering in SPIRAM

### Timing Parameters
```
HSYNC: Front=40, Back=140, Pulse=40
VSYNC: Front=16, Back=16, Pulse=4
```

## 📚 Documentation Files

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
1. ESP32-P4 Function EV Board (MIPI-DSI) - **Recommended**
2. ESP32_GENERIC_P4 8MB (Generic)
3. ESP32_GENERIC_P4 C6_WIFI 8MB (Generic with WiFi)
4. ESP32_GENERIC_S3 variants (Compatibility)

## 🔗 Useful Links

- [ESP32-P4 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-p4_datasheet_en.pdf)
- [ESP32-P4 Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-p4_technical_reference_manual_en.pdf)
- [EV Board User Guide](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/user_guide.html)
- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/)
- [LVGL Documentation](https://docs.lvgl.io/)
- [MicroPython Documentation](https://docs.micropython.org/)

## ⚙️ Advanced: Custom Board Configuration

To create your own board configuration:

1. Copy the example TOML:
   ```bash
   cp display_configs/ESP32-P4-Function-EV-Board.toml display_configs/my-custom-board.toml
   ```

2. Edit your configuration:
   - Adjust GPIO pins for your hardware
   - Modify display resolution and timing
   - Configure touch controller settings
   - Set flash size and SPIRAM options

3. Build with your configuration:
   ```bash
   python3 make.py --toml=display_configs/my-custom-board.toml
   ```

## 📄 License

Copyright (c) 2024 - 2025 Kevin G. Schlosser

See [LICENSE](../LICENSE) for details.
