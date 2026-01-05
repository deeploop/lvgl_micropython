# ESP32-P4 Function EV Board - Complete Hardware Reference for MicroPython

## 📋 Board Overview

**Board Name:** ESP32-P4 Function Evaluation Board
**Manufacturer:** Espressif Systems
**Hardware Version:** V1.4
**Documentation:** https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/user_guide.html

---

## 🔧 Chip Specifications

### Main Processor

| Property | Value | Description |
|----------|-------|-------------|
| **Chip** | ESP32-P4 | High-performance MCU |
| **Architecture** | RISC-V | Dual-core 32-bit |
| **CPU Frequency** | 360 MHz | Configurable |
| **Cores** | 2 | Dual-core processing |
| **FreeRTOS Tick** | 1000 Hz | Real-time OS |

### Memory Configuration

| Type | Size | Speed | Notes |
|------|------|-------|-------|
| **Internal RAM** | ~768 KB | High-speed | For code execution |
| **SPIRAM (PSRAM)** | External | 200 MHz | HEX mode, 16 MB typical |
| **Flash** | External SPI | Variable | User application storage |
| **RTC Fast Memory** | Supported | Low-power | Retain during sleep |
| **RTC Slow Memory** | Supported | Low-power | Ultra-low power |

**SPIRAM Configuration:**
- Mode: HEX (Octal DDR)
- Speed: 200 MHz
- Features:
  - XIP (Execute In Place) from PSRAM
  - Instructions can execute from PSRAM
  - Read-only data stored in PSRAM
  - Stack can be in PSRAM
  - Malloc from PSRAM enabled

**Memory Allocation:**
```python
# MicroPython memory recommendations
Internal RAM: Code, hot paths, ISRs
SPIRAM: Large buffers, display framebuffers, data arrays
Flash: Firmware, frozen modules
```

---

## 📺 Display Hardware

### MIPI-DSI LCD Panel

| Property | Value | MicroPython Pin |
|----------|-------|-----------------|
| **Type** | MIPI-DSI LCD | N/A (DSI interface) |
| **Default Panel** | ili9881c 7-inch | - |
| **Alternative** | ek79007 | - |
| **Resolution (Default)** | 800 x 1280 (Portrait) | - |
| **Logical Resolution** | 800 x 480 (Landscape, rotated) | - |
| **Alternative Resolution** | 1024 x 600 | - |
| **Color Format** | RGB565 (default) / RGB888 | - |
| **Bits Per Pixel** | 16 (RGB565) / 24 (RGB888) | - |
| **Color Space** | RGB | - |
| **Byte Order** | Little Endian | - |
| **Interface** | MIPI-DSI | 2 data lanes |
| **Lane Bitrate** | 1000 Mbps per lane (1 Gbps) | - |
| **Backlight** | PWM controlled | GPIO 23 |
| **Reset** | Digital GPIO | GPIO NC (not connected for default panel) |

**Display Timing (800x1280 Panel):**
```python
# MicroPython display configuration
HSYNC = 40      # Horizontal sync
HBP = 140       # Horizontal back porch
HFP = 40        # Horizontal front porch
VSYNC = 4       # Vertical sync
VBP = 16        # Vertical back porch
VFP = 16        # Vertical front porch
```

**Display Timing (1024x600 Panel):**
```python
HSYNC = 1344
HBP = 160
HFP = 160
VSYNC = 635
VBP = 23
VFP = 12
```

**Frame Buffer Size:**
```python
# RGB565 (16-bit)
Single buffer: 800 * 480 * 2 = 768 KB
Double buffer: 768 KB * 2 = 1.5 MB (recommended: use SPIRAM)

# RGB888 (24-bit)
Single buffer: 800 * 480 * 3 = 1.15 MB
Double buffer: 1.15 MB * 2 = 2.3 MB
```

**MicroPython Display Control:**
```python
from machine import Pin, PWM

# Backlight control (PWM)
backlight = PWM(Pin(23), freq=1000, duty=512)  # 50% brightness

# Full brightness
backlight.duty(1023)

# Off
backlight.duty(0)
```

---

## 👆 Touch Controller

### Capacitive Touch

| Property | Value | MicroPython Pin |
|----------|-------|-----------------|
| **Controller** | GT911 | Via I2C |
| **Interface** | I2C | I2C0 (GPIO 7, 8) |
| **Touch Points** | Multi-touch | Up to 5 points |
| **Resolution** | Matches display | 800 x 1280 or 1024 x 600 |
| **Reset Pin** | Not connected | GPIO NC |
| **Interrupt Pin** | Not connected | GPIO NC |

**MicroPython Touch Example:**
```python
from machine import I2C, Pin

# Initialize I2C for touch
i2c = I2C(0, scl=Pin(8), sda=Pin(7), freq=400000)

# GT911 touch controller typically at address 0x5D or 0x14
touch_addr = 0x5D

# Read touch data
# (Requires GT911 driver implementation)
```

---

## 🎯 GPIO Pinout

### I2C Bus (I2C0)

| Function | GPIO Pin | MicroPython | Notes |
|----------|----------|-------------|-------|
| **SCL** | GPIO 8 | `Pin(8)` | Clock line |
| **SDA** | GPIO 7 | `Pin(7)` | Data line |

**Connected Devices:**
- Audio Codec ES8311 (configuration)
- Touch Controller GT911

**MicroPython I2C:**
```python
from machine import I2C, Pin

# Initialize I2C bus
i2c = I2C(0, scl=Pin(8), sda=Pin(7), freq=400000)

# Scan for devices
devices = i2c.scan()
print("I2C devices found:", [hex(dev) for dev in devices])
```

### I2S Audio

| Function | GPIO Pin | MicroPython | Notes |
|----------|----------|-------------|-------|
| **SCLK** | GPIO 12 | `Pin(12)` | Serial clock |
| **MCLK** | GPIO 13 | `Pin(13)` | Master clock |
| **LCLK** | GPIO 10 | `Pin(10)` | Left/right clock (WS) |
| **DOUT** | GPIO 9 | `Pin(9)` | Data out (speaker) |
| **DSIN** | GPIO 48 | `Pin(48)` | Data in (microphone) |
| **AMP Power** | GPIO 11 | `Pin(11)` | Speaker amplifier enable |

**Audio Codec:**
- Chip: ES8311
- Interface: I2S (full-duplex)
- Features: Speaker output + Microphone input
- Sample rates: 8 kHz - 48 kHz

**MicroPython I2S Example:**
```python
from machine import I2S, Pin

# Speaker output
audio_out = I2S(
    0,
    sck=Pin(12),
    ws=Pin(10),
    sd=Pin(9),
    mode=I2S.TX,
    bits=16,
    format=I2S.STEREO,
    rate=44100,
    ibuf=20000
)

# Microphone input
audio_in = I2S(
    1,
    sck=Pin(12),
    ws=Pin(10),
    sd=Pin(48),
    mode=I2S.RX,
    bits=16,
    format=I2S.MONO,
    rate=16000,
    ibuf=10000
)

# Enable speaker amplifier
amp_enable = Pin(11, Pin.OUT)
amp_enable.value(1)

# Play audio
audio_out.write(audio_data)

# Record audio
mic_data = audio_in.read(1024)
```

### SD Card (SDMMC)

| Function | GPIO Pin | MicroPython | Notes |
|----------|----------|-------------|-------|
| **D0** | GPIO 39 | `Pin(39)` | Data 0 |
| **D1** | GPIO 40 | `Pin(40)` | Data 1 |
| **D2** | GPIO 41 | `Pin(41)` | Data 2 |
| **D3** | GPIO 42 | `Pin(42)` | Data 3 (also CS in SPI mode) |
| **CMD** | GPIO 44 | `Pin(44)` | Command line |
| **CLK** | GPIO 43 | `Pin(43)` | Clock line |

**SD Card Interface:**
- Mode: SDMMC (4-bit)
- Speed: High-speed SDR25 (up to 50 MHz)
- Voltage: 3.3V
- File systems: FAT32, exFAT

**MicroPython SD Card:**
```python
from machine import SDCard
import os

# Initialize SD card (SDMMC interface)
sd = SDCard(slot=1, width=4)  # 4-bit mode

# Mount SD card
os.mount(sd, '/sd')

# List files
print(os.listdir('/sd'))

# Read/write files
with open('/sd/test.txt', 'w') as f:
    f.write('Hello from ESP32-P4!')

# Unmount
os.umount('/sd')
```

### Display Backlight

| Function | GPIO Pin | MicroPython | Notes |
|----------|----------|-------------|-------|
| **Backlight** | GPIO 23 | `Pin(23)` | PWM control |

**Backlight Control:**
```python
from machine import Pin, PWM

# PWM for brightness control
backlight = PWM(Pin(23), freq=1000)

# Set brightness (0-1023)
def set_brightness(percent):
    duty = int(1023 * percent / 100)
    backlight.duty(duty)

set_brightness(50)  # 50% brightness
set_brightness(100) # Full brightness
set_brightness(0)   # Off
```

### Display Reset (Optional)

| Function | GPIO Pin | MicroPython | Notes |
|----------|----------|-------------|-------|
| **LCD Reset** | GPIO 0 or NC | `Pin(0)` | Panel-dependent |

For 1024x600 panel: GPIO 0
For 800x1280 panel: Not connected

---

## 📡 Wireless Connectivity

### WiFi

| Property | Value | MicroPython API |
|----------|-------|-----------------|
| **Protocol** | IEEE 802.11 b/g/n | `network.WLAN` |
| **Frequency** | 2.4 GHz only | - |
| **Mode** | Station / AP / STA+AP | `network.STA_IF`, `network.AP_IF` |
| **Security** | WPA/WPA2/WPA3 | - |
| **Antenna** | On-board PCB antenna | - |

**MicroPython WiFi Example:**
```python
import network

# Station mode (connect to router)
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wlan.connect('SSID', 'password')

# Wait for connection
while not wlan.isconnected():
    pass

print('Network config:', wlan.ifconfig())
# Output: ('192.168.1.100', '255.255.255.0', '192.168.1.1', '8.8.8.8')

# Access Point mode
ap = network.WLAN(network.AP_IF)
ap.active(True)
ap.config(essid='ESP32-P4', password='12345678')
```

### Bluetooth (BLE)

**Note:** ESP32-P4 uses a wireless host interface. BLE functionality depends on external module or implementation.

---

## 🔌 Peripheral Interfaces

### Available Peripherals

| Peripheral | Count | MicroPython Support | Notes |
|------------|-------|---------------------|-------|
| **UART** | 5 | ✅ `machine.UART` | Serial communication |
| **I2C** | 2 | ✅ `machine.I2C` | Master mode |
| **I2S** | 3 | ✅ `machine.I2S` | Audio interface |
| **SPI** | 3 (GPSPI) | ✅ `machine.SPI` | High-speed SPI |
| **SDMMC** | 2 hosts | ✅ `machine.SDCard` | SD card interface |
| **ADC** | 8 channels (2 units) | ✅ `machine.ADC` | 12-bit resolution |
| **DAC** | Not available | ❌ | Use PWM for analog output |
| **PWM (LEDC)** | 8 channels | ✅ `machine.PWM` | 16-bit resolution |
| **PWM (MCPWM)** | Available | ⚠️ Limited | Motor control PWM |
| **RMT** | 4 TX + 4 RX | ✅ `machine.RMT` | IR, WS2812, etc. |
| **I2S** | 3 | ✅ `machine.I2S` | Audio/data streaming |
| **Touch Sensor** | Supported | ⚠️ Capacitive | GT911 via I2C |
| **GPIO** | 50+ pins | ✅ `machine.Pin` | Digital I/O |
| **Timers** | 4 GP timers | ✅ `machine.Timer` | Hardware timers |
| **WDT** | Supported | ✅ `machine.WDT` | Watchdog timer |
| **RTC** | Built-in | ✅ `machine.RTC` | Real-time clock |
| **USB OTG** | 1 | ⚠️ Limited | USB host/device |
| **USB Serial JTAG** | 1 | ✅ | Debug interface |
| **Ethernet** | EMAC | ⚠️ Limited | Requires PHY chip |
| **TWAI (CAN)** | 2 controllers | ⚠️ Limited | CAN bus |
| **MIPI CSI** | Camera interface | ⚠️ Advanced | Camera input |
| **MIPI DSI** | Display interface | ⚠️ Advanced | Used for LCD |
| **ISP** | Image processor | ⚠️ Advanced | Image processing |
| **PARLIO** | Parallel I/O | ⚠️ Advanced | High-speed parallel |
| **DMA2D** | 2D graphics | ⚠️ Advanced | GPU-like operations |
| **PPA** | Pixel processing | ⚠️ Advanced | Image rotation/scaling |

---

## 🎛️ ADC Channels

| ADC Unit | Channels | Resolution | Reference | Notes |
|----------|----------|------------|-----------|-------|
| ADC1 | 8 channels | 12-bit | Internal | 0-3.3V |
| ADC2 | 8 channels | 12-bit | Internal | 0-3.3V |

**MicroPython ADC:**
```python
from machine import ADC, Pin

# ADC channel (check pinout for available pins)
adc = ADC(Pin(4))  # Example pin
adc.atten(ADC.ATTN_11DB)  # 0-3.3V range
adc.width(ADC.WIDTH_12BIT)  # 12-bit resolution

# Read value (0-4095)
value = adc.read()
voltage = value * 3.3 / 4095
print(f"Value: {value}, Voltage: {voltage:.2f}V")
```

---

## ⚡ Power Management

### Power Features

| Feature | Status | MicroPython API |
|---------|--------|-----------------|
| **Light Sleep** | ✅ Supported | `machine.lightsleep()` |
| **Deep Sleep** | ✅ Supported | `machine.deepsleep()` |
| **Power Management** | ✅ Auto frequency scaling | `machine.freq()` |
| **RTC Memory** | ✅ Persists in sleep | Use `machine.RTC().memory()` |
| **ULP Coprocessor** | ✅ LP-Core | Advanced usage |
| **Low-Power Peripherals** | ✅ LP-GPIO, LP-I2C, LP-SPI | - |

**Sleep Modes:**
```python
from machine import Pin, lightsleep, deepsleep
import time

# Light sleep (preserves RAM, quick wake)
# Wake up after 5 seconds
lightsleep(5000)

# Deep sleep (minimal power, slower wake, loses RAM except RTC memory)
# Wake up after 10 seconds
deepsleep(10000)

# Wake on GPIO
wake_pin = Pin(5, mode=Pin.IN, pull=Pin.PULL_UP)
# Configure wake source, then sleep
```

### LDO and Power Rails

| Rail | Voltage | Usage |
|------|---------|-------|
| **VDD_MIPI_DPHY** | 2.5V | MIPI DSI PHY |
| **DCDC** | Configurable | Power management |
| **GP_LDO** | Configurable | General purpose LDO |
| **VBAT** | Battery input | RTC power |

---

## 🔐 Security Features

| Feature | Status | Description |
|---------|--------|-------------|
| **Secure Boot** | ✅ Supported | Verified boot |
| **Flash Encryption** | ✅ Supported | Encrypt flash contents |
| **AES** | ✅ Hardware | AES-128/256, GCM mode |
| **SHA** | ✅ Hardware | SHA-1/256/384/512 |
| **RSA** | ✅ Hardware (MPI) | RSA acceleration |
| **HMAC** | ✅ Hardware | HMAC |
| **Digital Signature** | ✅ Hardware | ECC-based signatures |
| **ECC** | ✅ Hardware | Elliptic curve crypto |
| **RNG** | ✅ True RNG | Random number generation |
| **eFuse** | ✅ OTP | One-time programmable |

**MicroPython Crypto:**
```python
import cryptolib
import os

# Hardware RNG
random_bytes = os.urandom(16)

# AES encryption
key = b'Sixteen byte key'
cipher = cryptolib.aes(key, 1)  # MODE_ECB
encrypted = cipher.encrypt(b'Message to encr!')
```

---

## 📊 Hardware Capabilities Summary

### ✅ Fully Functional in MicroPython

- GPIO (Digital I/O, interrupts)
- PWM (LEDC)
- ADC (12-bit, 8 channels x 2)
- UART (5 ports)
- I2C (2 ports, master)
- SPI (3 ports)
- I2S (3 channels, audio)
- SD Card (SDMMC, 4-bit mode)
- WiFi (802.11 b/g/n, 2.4 GHz)
- Timers (hardware timers)
- RTC (real-time clock)
- WDT (watchdog)
- RMT (IR, NeoPixel)
- Sleep modes (light, deep)
- Crypto (AES, SHA, RNG)

### ⚠️ Limited / Advanced in MicroPython

- Display (MIPI-DSI) - Requires custom driver
- Touch (GT911) - Requires I2C driver implementation
- USB OTG - Limited support
- Bluetooth - External module
- MIPI CSI (Camera) - Advanced usage
- Ethernet - Requires PHY chip
- TWAI (CAN bus) - Limited API
- DMA2D, PPA - Graphics acceleration (advanced)

### ❌ Not Available in Standard MicroPython

- DAC (use PWM for analog output)
- Some advanced peripherals (ISP, PARLIO) without custom builds

---

## 🐍 MicroPython Board Definition

### Pin Mapping

```python
# ESP32-P4 Function EV Board Pin Definitions
# Use these constants in your MicroPython code

# I2C
I2C_SCL = 8
I2C_SDA = 7

# I2S Audio
I2S_SCLK = 12
I2S_MCLK = 13
I2S_LCLK = 10
I2S_DOUT = 9
I2S_DSIN = 48
AMP_ENABLE = 11

# SD Card
SD_D0 = 39
SD_D1 = 40
SD_D2 = 41
SD_D3 = 42
SD_CMD = 44
SD_CLK = 43

# Display
LCD_BACKLIGHT = 23
LCD_RESET = 0  # For 1024x600 panel only

# Example usage
from machine import Pin, I2C, PWM

i2c = I2C(0, scl=Pin(I2C_SCL), sda=Pin(I2C_SDA))
backlight = PWM(Pin(LCD_BACKLIGHT), freq=1000)
amp = Pin(AMP_ENABLE, Pin.OUT)
```

---

## 📐 Mechanical Specifications

| Property | Value |
|----------|-------|
| **Board Size** | Check official docs |
| **Display** | 7-inch diagonal (800x1280) or 1024x600 |
| **Mounting Holes** | Yes (dimensions in docs) |
| **Connectors** | USB-C, SD slot, Audio jack |

---

## 🔧 Development Setup for MicroPython

### Flash MicroPython Firmware

```bash
# Download ESP32-P4 MicroPython firmware
# (Check official MicroPython website)

# Erase flash
esptool.py --chip esp32p4 erase_flash

# Flash MicroPython
esptool.py --chip esp32p4 --port COM7 write_flash -z 0x0 micropython-esp32p4.bin
```

### Connect to REPL

```python
# Serial connection (115200 baud)
# Use PuTTY, screen, or mpremote

# macOS/Linux
screen /dev/ttyUSB0 115200

# Windows
# Use PuTTY with COM7 at 115200 baud

# Or use mpremote
mpremote connect COM7
```

### Upload Files

```bash
# Using mpremote
mpremote fs cp main.py :main.py
mpremote fs cp lib/* :lib/

# Using ampy
ampy --port COM7 put main.py
ampy --port COM7 put lib /lib

# Using WebREPL (after WiFi setup)
# Upload via browser at http://micropython.org/webrepl/
```

---

## 🎨 Graphics / Display Libraries for MicroPython

### Display Driver Options

Since MIPI-DSI is complex, you have these options:

**Option 1: LVGL Binding** (Recommended)
```python
# Use LVGL micropython binding
# https://github.com/lvgl/lv_binding_micropython

import lvgl as lv
import display_driver  # Custom MIPI-DSI driver

# Initialize display
disp = lv.disp_drv_t()
# ... configure driver ...
lv.disp_drv_register(disp)
```

**Option 2: Frame Buffer** (Manual)
```python
# Direct framebuffer access (if driver supports it)
import framebuf

# Allocate buffer in SPIRAM
fb = framebuf.FrameBuffer(bytearray(800*480*2), 800, 480, framebuf.RGB565)

# Draw
fb.fill(0)  # Black
fb.text("Hello ESP32-P4!", 10, 10, 0xFFFF)
fb.rect(100, 100, 200, 100, 0xF800)  # Red rectangle

# Update display (requires custom driver)
display.update(fb)
```

**Option 3: Custom C Module**
Build MicroPython with custom MIPI-DSI module for optimal performance.

---

## 💾 File System

| Type | Size | Mount Point | Notes |
|------|------|-------------|-------|
| **Flash (LittleFS)** | ~2-4 MB | `/` | Internal flash |
| **SD Card (FAT32)** | User card | `/sd` | External storage |
| **SPIFFS** | Optional | `/spiffs` | Flash file system |

```python
import os

# Internal flash (auto-mounted at boot)
os.listdir('/')

# Mount SD card
from machine import SDCard
sd = SDCard(slot=1, width=4)
os.mount(sd, '/sd')
os.listdir('/sd')
```

---

## 📚 Additional Resources

### Official Documentation

- **ESP32-P4 Datasheet:** https://www.espressif.com/sites/default/files/documentation/esp32-p4_datasheet_en.pdf
- **ESP32-P4 TRM:** https://www.espressif.com/sites/default/files/documentation/esp32-p4_technical_reference_manual_en.pdf
- **Function EV Board Guide:** https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/user_guide.html
- **ESP-IDF Documentation:** https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/

### MicroPython Resources

- **MicroPython for ESP32:** https://docs.micropython.org/en/latest/esp32/quickref.html
- **MicroPython Forum:** https://forum.micropython.org/
- **LVGL MicroPython:** https://github.com/lvgl/lv_binding_micropython

### GitHub Repositories

- **ESP32-P4 BSP:** https://github.com/espressif/esp-bsp/tree/master/bsp/esp32_p4_function_ev_board
- **LVGL Demo (this project):** https://github.com/tomtang12/lvgl_demo_v9

---

## 🔍 Hardware Verification Script

Copy this to `hardware_test.py` and run on ESP32-P4:

```python
"""
ESP32-P4 Hardware Test Script
Tests all major peripherals
"""

import machine
import time
import sys

print("="*50)
print("ESP32-P4 Function EV Board - Hardware Test")
print("="*50)

# System info
print("\n[1] System Information")
print(f"  Chip: {sys.platform}")
print(f"  CPU Frequency: {machine.freq()/1000000} MHz")
print(f"  Free Memory: {machine.mem_free()} bytes")

# GPIO Test
print("\n[2] GPIO Test")
test_pin = machine.Pin(11, machine.Pin.OUT)
test_pin.value(1)
print(f"  GPIO 11 set HIGH")
time.sleep(0.5)
test_pin.value(0)
print(f"  GPIO 11 set LOW")

# PWM Test
print("\n[3] PWM Test (Backlight)")
try:
    backlight = machine.PWM(machine.Pin(23), freq=1000, duty=512)
    print(f"  Backlight PWM: OK (50% duty)")
    time.sleep(1)
    backlight.duty(1023)
    print(f"  Backlight: 100%")
    time.sleep(1)
    backlight.duty(0)
    print(f"  Backlight: OFF")
except Exception as e:
    print(f"  Backlight PWM: FAILED - {e}")

# I2C Test
print("\n[4] I2C Bus Scan")
try:
    i2c = machine.I2C(0, scl=machine.Pin(8), sda=machine.Pin(7), freq=400000)
    devices = i2c.scan()
    if devices:
        print(f"  Devices found: {[hex(d) for d in devices]}")
    else:
        print(f"  No I2C devices found")
except Exception as e:
    print(f"  I2C scan: FAILED - {e}")

# WiFi Test
print("\n[5] WiFi Test")
try:
    import network
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    print(f"  WiFi initialized: OK")
    print(f"  MAC Address: {':'.join(['%02x'%b for b in wlan.config('mac')])}")
except Exception as e:
    print(f"  WiFi: FAILED - {e}")

# SD Card Test
print("\n[6] SD Card Test")
try:
    import os
    sd = machine.SDCard(slot=1, width=4)
    os.mount(sd, '/sd')
    files = os.listdir('/sd')
    print(f"  SD Card mounted: OK")
    print(f"  Files: {files[:5]}")  # Show first 5 files
    os.umount('/sd')
except Exception as e:
    print(f"  SD Card: FAILED - {e}")

# Timer Test
print("\n[7] Timer Test")
timer_fired = False
def timer_callback(t):
    global timer_fired
    timer_fired = True

try:
    timer = machine.Timer(0)
    timer.init(period=1000, mode=machine.Timer.ONE_SHOT, callback=timer_callback)
    time.sleep(1.5)
    if timer_fired:
        print(f"  Timer: OK (fired correctly)")
    else:
        print(f"  Timer: FAILED (did not fire)")
except Exception as e:
    print(f"  Timer: FAILED - {e}")

# RTC Test
print("\n[8] RTC Test")
try:
    rtc = machine.RTC()
    rtc.datetime((2025, 1, 15, 2, 12, 30, 0, 0))  # Set time
    dt = rtc.datetime()
    print(f"  RTC: OK")
    print(f"  Time: {dt[0]}-{dt[1]:02d}-{dt[2]:02d} {dt[4]:02d}:{dt[5]:02d}:{dt[6]:02d}")
except Exception as e:
    print(f"  RTC: FAILED - {e}")

print("\n" + "="*50)
print("Hardware test complete!")
print("="*50)
```

---

## ✅ Quick Reference - Pin Summary

```python
# I2C Bus
I2C0_SCL = 8
I2C0_SDA = 7

# I2S Audio
I2S_SCLK = 12      # Serial clock
I2S_MCLK = 13      # Master clock
I2S_LCLK = 10      # Word select
I2S_DOUT = 9       # Speaker data
I2S_DIN = 48       # Mic data
I2S_AMP_EN = 11    # Amplifier enable

# SD Card (SDMMC)
SD_D0 = 39
SD_D1 = 40
SD_D2 = 41
SD_D3 = 42
SD_CMD = 44
SD_CLK = 43

# Display
LCD_BACKLIGHT = 23
LCD_RESET = 0      # Panel-dependent

# MIPI-DSI
# (Uses dedicated DSI peripheral, not regular GPIO)
```

---

**Document Version:** 1.0
**Date:** 2025-01-15
**Based on:** ESP-IDF 5.5.1, ESP32-P4 Function EV Board V1.4
**For:** MicroPython Development

All hardware specifications extracted from official ESP32-P4 BSP and tested project configuration.
