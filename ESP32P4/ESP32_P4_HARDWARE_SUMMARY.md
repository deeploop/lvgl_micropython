# ESP32-P4 Function EV Board - Hardware Summary

Quick reference for all hardware specifications extracted from BSP.

## 🔧 Main Specifications

| Property | Value |
|----------|-------|
| **Chip** | ESP32-P4 |
| **Architecture** | RISC-V Dual-core 32-bit |
| **CPU Frequency** | 360 MHz (configurable) |
| **Internal RAM** | ~768 KB |
| **SPIRAM (PSRAM)** | 16 MB typical, 200 MHz (HEX mode) |
| **Flash** | External SPI (size varies) |
| **WiFi** | IEEE 802.11 b/g/n (2.4 GHz only) |
| **Bluetooth** | Wireless host interface (external module) |

---

## 📺 Display

| Property | Value |
|----------|-------|
| **Type** | MIPI-DSI LCD |
| **Default Panel** | ili9881c (7-inch, 800x1280) |
| **Alternative Panel** | ek79007 (1024x600) |
| **Logical Resolution** | 800x480 (landscape, rotated 90°) |
| **Physical Resolution** | 800x1280 (portrait) |
| **Color Format** | RGB565 (16-bit) or RGB888 (24-bit) |
| **MIPI Lanes** | 2 data lanes |
| **Lane Bitrate** | 1000 Mbps (1 Gbps per lane) |
| **Backlight Control** | GPIO 23 (PWM) |
| **Reset Pin** | GPIO 0 (1024x600 only) or NC (default) |

---

## 👆 Touch

| Property | Value |
|----------|-------|
| **Controller** | GT911 (I2C) |
| **Interface** | I2C0 (GPIO 7, 8) |
| **Touch Points** | Multi-touch (up to 5 points) |
| **I2C Address** | 0x5D or 0x14 |
| **Resolution** | Matches display |

---

## 🎯 GPIO Pin Assignments

### I2C (I2C0)

| Function | GPIO | Notes |
|----------|------|-------|
| **SCL** | 8 | Clock |
| **SDA** | 7 | Data |

**Connected Devices:** Audio codec ES8311, Touch GT911

### I2S Audio

| Function | GPIO | Notes |
|----------|------|-------|
| **SCLK** | 12 | Serial clock (BCK) |
| **MCLK** | 13 | Master clock |
| **LCLK** | 10 | Left/right clock (WS) |
| **DOUT** | 9 | Speaker data out |
| **DSIN** | 48 | Microphone data in |
| **AMP Enable** | 11 | Speaker amplifier power |

**Audio Codec:** ES8311 (I2S full-duplex, I2C control)

### SD Card (SDMMC)

| Function | GPIO | Notes |
|----------|------|-------|
| **D0** | 39 | Data 0 |
| **D1** | 40 | Data 1 |
| **D2** | 41 | Data 2 |
| **D3** | 42 | Data 3 (CS in SPI mode) |
| **CMD** | 44 | Command line |
| **CLK** | 43 | Clock |

**Mode:** 4-bit SDMMC (high-speed SDR25, up to 50 MHz)

---

## 🔌 Peripheral Summary

| Peripheral | Count | Available | MicroPython | ESP-IDF |
|------------|-------|-----------|-------------|---------|
| **GPIO** | 50+ | ✅ | ✅ | ✅ |
| **UART** | 5 | ✅ | ✅ | ✅ |
| **I2C** | 2 | ✅ | ✅ | ✅ |
| **SPI (GPSPI)** | 3 | ✅ | ✅ | ✅ |
| **I2S** | 3 | ✅ | ✅ | ✅ |
| **SDMMC** | 2 hosts | ✅ | ✅ | ✅ |
| **ADC** | 8 ch x 2 | ✅ | ✅ | ✅ |
| **DAC** | 0 | ❌ | ❌ | ❌ |
| **PWM (LEDC)** | 8 | ✅ | ✅ | ✅ |
| **PWM (MCPWM)** | 2 units | ✅ | ⚠️ | ✅ |
| **RMT** | 4 TX + 4 RX | ✅ | ✅ | ✅ |
| **Timer** | 4 general | ✅ | ✅ | ✅ |
| **Watchdog** | 2 | ✅ | ✅ | ✅ |
| **RTC** | 1 | ✅ | ✅ | ✅ |
| **Touch Sensor** | Capacitive | ✅ | Via I2C | ✅ |
| **USB OTG** | 1 | ✅ | ⚠️ | ✅ |
| **USB Serial JTAG** | 1 | ✅ | ✅ | ✅ |
| **Ethernet (EMAC)** | 1 | ✅ | ⚠️ | ✅ |
| **TWAI (CAN)** | 2 | ✅ | ⚠️ | ✅ |
| **MIPI CSI** | 1 | ✅ | ❌ | ✅ |
| **MIPI DSI** | 1 | ✅ | ❌ | ✅ |
| **ISP** | 1 | ✅ | ❌ | ✅ |
| **PARLIO** | 1 | ✅ | ❌ | ✅ |
| **DMA2D** | 1 | ✅ | ❌ | ✅ |
| **PPA** | 1 | ✅ | ❌ | ✅ |
| **I3C** | 1 master | ✅ | ❌ | ✅ |

**Legend:**
- ✅ Fully supported
- ⚠️ Limited/Advanced
- ❌ Not available

---

## ⚡ Power & Clock

| Property | Value |
|----------|-------|
| **CPU Frequency** | 360 MHz (default) |
| **SPIRAM Speed** | 200 MHz |
| **FreeRTOS Tick** | 1000 Hz |
| **Crystal** | 40 MHz |
| **Light Sleep** | Supported |
| **Deep Sleep** | Supported |
| **ULP Coprocessor** | LP-Core supported |
| **Low-Power Peripherals** | LP-GPIO, LP-I2C, LP-SPI, LP-UART |

---

## 🔐 Security Features

| Feature | Status |
|---------|--------|
| **Secure Boot** | ✅ |
| **Flash Encryption** | ✅ |
| **AES** | ✅ Hardware (128/256, GCM) |
| **SHA** | ✅ Hardware (1/256/384/512) |
| **RSA** | ✅ Hardware (MPI) |
| **HMAC** | ✅ Hardware |
| **Digital Signature** | ✅ Hardware (ECC) |
| **ECC** | ✅ Hardware |
| **RNG** | ✅ True RNG |
| **eFuse** | ✅ OTP |

---

## 📊 ADC Specifications

| Property | Value |
|----------|-------|
| **Units** | 2 (ADC1, ADC2) |
| **Channels per Unit** | 8 |
| **Resolution** | 12-bit (0-4095) |
| **Voltage Range** | 0-3.3V (with attenuation) |
| **DMA Support** | Yes |

---

## 💾 Memory Map

| Type | Size/Speed | Usage |
|------|------------|-------|
| **Internal RAM** | ~768 KB | Code execution, hot paths |
| **SPIRAM (PSRAM)** | 16 MB @ 200MHz | Large buffers, display FB |
| **RTC Fast Memory** | Available | Retained in sleep |
| **RTC Slow Memory** | Available | Ultra low-power retention |
| **Flash** | External SPI | Firmware, data storage |

**SPIRAM Configuration:**
- Mode: HEX (Octal DDR)
- XIP: Execute in place enabled
- Instructions: Can execute from PSRAM
- Stack: Can be in PSRAM
- Malloc: Automatic allocation to PSRAM

---

## 📁 Board Capabilities (BSP)

| Feature | Status |
|---------|--------|
| **Display** | ✅ (MIPI-DSI, 800x480) |
| **Touch** | ✅ (GT911 capacitive) |
| **Buttons** | ❌ (none on board) |
| **Audio** | ✅ (ES8311 codec) |
| **Speaker** | ✅ (via I2S DOUT + AMP) |
| **Microphone** | ✅ (via I2S DSIN) |
| **SD Card** | ✅ (4-bit SDMMC) |
| **IMU** | ❌ (not included) |

---

## 🎨 Display Timing

### 800x1280 Panel (Default)

| Parameter | Value |
|-----------|-------|
| **H Res** | 800 px |
| **V Res** | 1280 px (rotated to 480) |
| **HSYNC** | 40 |
| **HBP** | 140 |
| **HFP** | 40 |
| **VSYNC** | 4 |
| **VBP** | 16 |
| **VFP** | 16 |

### 1024x600 Panel (Alternative)

| Parameter | Value |
|-----------|-------|
| **H Res** | 1024 px |
| **V Res** | 600 px |
| **HSYNC** | 1344 |
| **HBP** | 160 |
| **HFP** | 160 |
| **VSYNC** | 635 |
| **VBP** | 23 |
| **VFP** | 12 |

---

## 📏 Frame Buffer Size

### RGB565 (16-bit)

| Configuration | Size |
|---------------|------|
| **Single buffer** (800x480) | 768 KB |
| **Double buffer** (800x480) | 1.5 MB |
| **Single buffer** (1024x600) | 1.2 MB |
| **Double buffer** (1024x600) | 2.4 MB |

### RGB888 (24-bit)

| Configuration | Size |
|---------------|------|
| **Single buffer** (800x480) | 1.15 MB |
| **Double buffer** (800x480) | 2.3 MB |
| **Single buffer** (1024x600) | 1.8 MB |
| **Double buffer** (1024x600) | 3.6 MB |

**Recommendation:** Use RGB565 + double buffering in SPIRAM for smooth rendering.

---

## 🌐 WiFi Specifications

| Property | Value |
|----------|-------|
| **Protocol** | IEEE 802.11 b/g/n |
| **Frequency** | 2.4 GHz only (no 5 GHz) |
| **Modes** | Station, AP, STA+AP |
| **Security** | WPA/WPA2/WPA3 |
| **Antenna** | On-board PCB |
| **Max TX Power** | 20 dBm |

---

## 🔊 Audio Specifications

| Property | Value |
|----------|-------|
| **Codec** | ES8311 |
| **Interface** | I2S (full-duplex) + I2C (control) |
| **Sample Rates** | 8-48 kHz |
| **Bit Depth** | 16/24/32 bit |
| **Channels** | Mono / Stereo |
| **Speaker Output** | Via amplifier (GPIO 11 enable) |
| **Microphone Input** | Built-in or external |
| **I2C Address** | 0x18 (ES8311) |

---

## 📖 Documentation Links

| Resource | URL |
|----------|-----|
| **ESP32-P4 Datasheet** | https://www.espressif.com/sites/default/files/documentation/esp32-p4_datasheet_en.pdf |
| **ESP32-P4 TRM** | https://www.espressif.com/sites/default/files/documentation/esp32-p4_technical_reference_manual_en.pdf |
| **EV Board User Guide** | https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/user_guide.html |
| **ESP-IDF Docs** | https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/ |
| **MicroPython Docs** | https://docs.micropython.org/en/latest/esp32/quickref.html |
| **ESP-BSP GitHub** | https://github.com/espressif/esp-bsp/tree/master/bsp/esp32_p4_function_ev_board |

---

## 🐍 MicroPython Resources

All MicroPython code, examples, and libraries are in:
- **Hardware Reference:** `ESP32_P4_MICROPYTHON_HARDWARE_REFERENCE.md`
- **Board Library:** `micropython_examples/esp32p4_board.py`
- **Examples:** `micropython_examples/example_basic.py`
- **README:** `micropython_examples/README.md`

---

## 📋 Quick Pin Reference

```python
# Copy-paste ready pin definitions

# I2C
I2C_SCL = 8
I2C_SDA = 7

# I2S Audio
I2S_SCLK = 12
I2S_MCLK = 13
I2S_LCLK = 10
I2S_DOUT = 9
I2S_DIN = 48
AMP_EN = 11

# SD Card
SD_D0 = 39
SD_D1 = 40
SD_D2 = 41
SD_D3 = 42
SD_CMD = 44
SD_CLK = 43

# Display
LCD_BL = 23
LCD_RST = 0  # Panel-dependent
```

---

**Extracted from:** ESP32-P4 BSP v3.x, ESP-IDF 5.5.1
**Board Version:** V1.4
**Date:** 2025-01-15
