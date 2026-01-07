/**
 * @file jd9165ba_display.h
 * @brief JD9165BA MIPI-DSI Display Driver - C Extension Module for MicroPython
 *
 * This module provides direct C-level MIPI-DSI initialization for JD9165BA displays
 * on ESP32-P4, bypassing the potentially broken generic display framework.
 *
 * Copyright (c) 2024 - 2025
 */

#ifndef JD9165BA_DISPLAY_H
#define JD9165BA_DISPLAY_H

#include "py/obj.h"
#include "py/runtime.h"

#if SOC_MIPI_DSI_SUPPORTED

#include "esp_lcd_panel_io.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "esp_heap_caps.h"

// Display configuration
#define JD9165BA_PHYSICAL_WIDTH     800
#define JD9165BA_PHYSICAL_HEIGHT    1280
#define JD9165BA_LOGICAL_WIDTH      800   // After 90° rotation
#define JD9165BA_LOGICAL_HEIGHT     480   // After 90° rotation

#define JD9165BA_COLOR_BITS         16    // RGB565
#define JD9165BA_BUFFER_SIZE        (JD9165BA_LOGICAL_WIDTH * JD9165BA_LOGICAL_HEIGHT * 2)

// MIPI-DSI configuration
#define JD9165BA_DSI_BUS_ID         0
#define JD9165BA_DSI_LANES          2     // 2 data lanes
#define JD9165BA_DSI_FREQ_MHZ       1000  // 1000 Mbps per lane
#define JD9165BA_VIRTUAL_CHANNEL    0

// Display timing (for 800x1280 panel)
#define JD9165BA_HSYNC_FRONT_PORCH  40
#define JD9165BA_HSYNC_BACK_PORCH   140
#define JD9165BA_HSYNC_PULSE_WIDTH  40
#define JD9165BA_VSYNC_FRONT_PORCH  16
#define JD9165BA_VSYNC_BACK_PORCH   16
#define JD9165BA_VSYNC_PULSE_WIDTH  4

// GPIO pins (ESP32-P4 Function EV Board)
#define JD9165BA_PIN_BACKLIGHT      23

// JD9165BA command definitions
#define JD9165BA_CMD_SLPOUT         0x11
#define JD9165BA_CMD_DISPON         0x29
#define JD9165BA_CMD_SLPIN          0x10
#define JD9165BA_CMD_DISPOFF        0x28
#define JD9165BA_CMD_SETEXTC        0xB9
#define JD9165BA_CMD_SETMIPI        0xBA
#define JD9165BA_CMD_SETPOWER       0xB1
#define JD9165BA_CMD_SETDISP        0xB2
#define JD9165BA_CMD_SETCYC         0xB4
#define JD9165BA_CMD_SETVCOM        0xB6
#define JD9165BA_CMD_SETPANEL       0xCC
#define JD9165BA_CMD_SETGAMMA       0xC7
#define JD9165BA_CMD_MADCTL         0x36
#define JD9165BA_CMD_COLMOD         0x3A

// Module functions
extern const mp_obj_type_t jd9165ba_display_type;

mp_obj_t jd9165ba_init(void);
mp_obj_t jd9165ba_deinit(void);
mp_obj_t jd9165ba_set_backlight(mp_obj_t brightness_obj);
mp_obj_t jd9165ba_get_width(void);
mp_obj_t jd9165ba_get_height(void);
mp_obj_t jd9165ba_sleep(void);
mp_obj_t jd9165ba_wake(void);

#endif // SOC_MIPI_DSI_SUPPORTED

#endif // JD9165BA_DISPLAY_H
