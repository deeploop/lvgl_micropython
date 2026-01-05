# ESP32-P4 Panel Configuration for LVGL Simulator

## 📺 Display Hardware Specifications

### ESP32-P4 Function Evaluation Board Display

**Panel Type:** MIPI-DSI LCD  
**Model:** 1280x800 (Default) or 1024x600 (Alternative)

---

## 🎨 Current Project Configuration

Based on `main/main.c` and BSP settings, this project uses:

### Display Resolution (Rotated)
```c
Width:  800 pixels
Height: 480 pixels  // Note: Actually 1280, but rotated 90° for landscape
```

### Color Configuration
```c
Color Format:    RGB565 (default) or RGB888
Bits Per Pixel:  16 bits (RGB565)
Color Space:     RGB
Byte Order:      Little Endian (BSP_LCD_BIGENDIAN = 0)
```

### Buffer Configuration
```c
Buffer Size:     480 * 800 = 384,000 pixels
Double Buffer:   Yes (BSP_LCD_DRAW_BUFF_DOUBLE)
Buffer Location: SPIRAM
DMA:             Disabled
Software Rotate: Enabled
```

---

## 🖥️ LVGL Simulator Configuration

### For SDL2-based LVGL Simulator

Create or update `lv_conf.h`:

```c
/* Display Configuration */
#define LV_HOR_RES_MAX          800
#define LV_VER_RES_MAX          480
#define LV_COLOR_DEPTH          16      /* 16-bit RGB565 */

/* Buffer Configuration */
#define LV_VDB_SIZE             (800 * 480 / 10)  /* 10% of screen */
#define LV_VDB_DOUBLE           1                   /* Double buffering */

/* Color Settings */
#define LV_COLOR_16_SWAP        0      /* No byte swapping */

/* Features */
#define LV_USE_GPU              0      /* Software rendering */
#define LV_USE_PERF_MONITOR     1      /* Show FPS */
#define LV_USE_MEM_MONITOR      1      /* Show memory usage */
```

### SDL2 Window Setup

```c
// In your simulator main.c
#define WINDOW_WIDTH   800
#define WINDOW_HEIGHT  480
#define DISP_HOR_RES   800
#define DISP_VER_RES   480

SDL_Init(SDL_INIT_VIDEO);
SDL_Window *window = SDL_CreateWindow(
    "ESP32-P4 LVGL Simulator",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    0
);
```

---

## 📊 Detailed Panel Specifications

### 1280x800 Configuration (Default - Rotated to 800x480)

```c
/* Physical Display */
#define BSP_LCD_H_RES              800      // Horizontal resolution
#define BSP_LCD_V_RES              1280     // Vertical resolution (rotated)

/* Timing Parameters (MIPI-DSI) */
#define BSP_LCD_MIPI_DSI_LCD_HSYNC    40   // Horizontal sync
#define BSP_LCD_MIPI_DSI_LCD_HBP      140  // Horizontal back porch
#define BSP_LCD_MIPI_DSI_LCD_HFP      40   // Horizontal front porch
#define BSP_LCD_MIPI_DSI_LCD_VSYNC    4    // Vertical sync
#define BSP_LCD_MIPI_DSI_LCD_VBP      16   // Vertical back porch
#define BSP_LCD_MIPI_DSI_LCD_VFP      16   // Vertical front porch

/* MIPI Interface */
#define BSP_LCD_MIPI_DSI_LANE_NUM          2      // 2 data lanes
#define BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS 1000   // 1 Gbps per lane
```

### 1024x600 Configuration (Alternative)

```c
/* Physical Display */
#define BSP_LCD_H_RES              1024    // Horizontal resolution
#define BSP_LCD_V_RES              600     // Vertical resolution

/* Timing Parameters (MIPI-DSI) */
#define BSP_LCD_MIPI_DSI_LCD_HSYNC    1344
#define BSP_LCD_MIPI_DSI_LCD_HBP      160
#define BSP_LCD_MIPI_DSI_LCD_HFP      160
#define BSP_LCD_MIPI_DSI_LCD_VSYNC    635
#define BSP_LCD_MIPI_DSI_LCD_VBP      23
#define BSP_LCD_MIPI_DSI_LCD_VFP      12
```

---

## 🎯 Quick Simulator Setup Guide

### Step 1: Choose Simulator Type

**Option A: Eclipse-based LVGL Simulator (Recommended)**
- Platform: Windows/Linux/Mac
- Language: C
- IDE: Eclipse or VS Code
- Repo: https://github.com/lvgl/lv_port_pc_eclipse

**Option B: Visual Studio Simulator**
- Platform: Windows only
- Language: C
- IDE: Visual Studio
- Repo: https://github.com/lvgl/lv_port_win_visual_studio

**Option C: CMake-based Simulator**
- Platform: Cross-platform
- Build: CMake + SDL2
- Repo: https://github.com/lvgl/lv_sim_visual_studio

### Step 2: Configure Display Size

Edit `lv_conf.h` or simulator configuration:

```c
#define LV_HOR_RES_MAX  800
#define LV_VER_RES_MAX  480
#define LV_COLOR_DEPTH  16
```

### Step 3: Add Your UI Code

Copy your LVGL UI code from ESP32-P4 project to simulator:

```c
// In simulator main.c
void app_main(void) {
    // Initialize LVGL
    lv_init();
    
    // Your ESP32-P4 UI code here
    lv_obj_t *screen = lv_scr_act();
    
    // Create your widgets
    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "ESP32-P4 RTSP Viewer");
    lv_obj_center(label);
}
```

---

## 🔧 Memory Configuration

### ESP32-P4 Actual Memory

```c
Total Frame Buffer Size:
- Single Buffer: 800 * 480 * 2 bytes = 768 KB (RGB565)
- Double Buffer: 800 * 480 * 2 * 2 = 1.5 MB

Buffer Size in main.c:
.buffer_size = 480 * 800 = 384,000 pixels
Actual memory = 384,000 * 2 bytes = 768 KB per buffer

SPIRAM Usage:
.buff_spiram = true  // Uses external PSRAM
```

### Simulator Memory

```c
// Allocate similar buffer on PC
static lv_color_t buf1[800 * 480 / 10];  // 10% buffer
static lv_color_t buf2[800 * 480 / 10];  // Double buffering

// Or full screen buffer
static lv_color_t buf1[800 * 480];
static lv_color_t buf2[800 * 480];
```

---

## 📱 Display Orientation

### ESP32-P4 Project (Rotated)

```c
// main.c configuration
.sw_rotate = true    // Software rotation enabled

// Actual rendering:
Logical:   800 x 480  (landscape)
Physical:  800 x 1280 (portrait panel, rotated in software)
```

### Simulator

```c
// Simulate the rotated display
SDL_CreateWindow("ESP32-P4 Simulator", ..., 800, 480, ...);

// Or match physical orientation
SDL_CreateWindow("ESP32-P4 Simulator", ..., 800, 1280, ...);
```

---

## 🎨 Color Format Details

### RGB565 (16-bit) - Default

```c
Red:   5 bits (0-31)
Green: 6 bits (0-63)
Blue:  5 bits (0-31)

Total: 16 bits per pixel
Memory: 2 bytes per pixel

Example color:
RGB888: (255, 128, 64)
RGB565: (31, 32, 8) = 0xFC08
```

### RGB888 (24-bit) - Optional

```c
Red:   8 bits (0-255)
Green: 8 bits (0-255)
Blue:  8 bits (0-255)

Total: 24 bits per pixel
Memory: 3 bytes per pixel (or 4 with padding)

To enable: Set CONFIG_BSP_LCD_COLOR_FORMAT_RGB888=y in menuconfig
```

---

## 🚀 Quick Start - LVGL Simulator

### 1. Download LVGL Simulator

```bash
git clone https://github.com/lvgl/lv_port_pc_eclipse.git
cd lv_port_pc_eclipse
git submodule update --init --recursive
```

### 2. Configure for ESP32-P4

Edit `lv_conf.h`:

```c
#define LV_HOR_RES_MAX  800
#define LV_VER_RES_MAX  480
#define LV_COLOR_DEPTH  16
#define LV_DPI_DEF      100  // Adjust for your monitor
```

### 3. Build and Run

```bash
# Linux/Mac
make
./bin/main

# Windows (Visual Studio)
Open .sln file and build
```

### 4. Test Your ESP32-P4 UI

Copy widgets and UI code from your ESP32-P4 project to simulator.

---

## 📖 Reference Files

In ESP32-P4 project:
- `components/espressif__esp32_p4_function_ev_board/include/bsp/display.h`
- `main/main.c` (lines 26-36)
- `components/bsp_extra/include/bsp_board_extra.h`

Panel specs:
- Resolution: 800x480 (landscape, rotated from 800x1280)
- Color: RGB565 (16-bit)
- Refresh: ~60 Hz
- Interface: MIPI-DSI 2-lane @ 1 Gbps

---

## ✅ Verification Checklist

For accurate simulation:
- ✅ Window size: 800x480 pixels
- ✅ Color depth: 16-bit (RGB565)
- ✅ Buffer: Double buffering enabled
- ✅ Refresh: ~16ms per frame (60 FPS)
- ✅ Orientation: Landscape
- ✅ Font: Same fonts as ESP32-P4
- ✅ Assets: Same images/resources

Now you can develop and test your LVGL UI on PC before flashing to ESP32-P4!
