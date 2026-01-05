/**
 * @file lv_conf_esp32p4.h
 * @brief LVGL Configuration for ESP32-P4 Panel Simulation
 * 
 * This configuration matches the ESP32-P4 Function Evaluation Board display:
 * - Resolution: 800x480 (landscape, rotated from 800x1280)
 * - Color: RGB565 (16-bit)
 * - Buffer: Double buffering in SPIRAM
 * 
 * Use this file for PC simulator to match ESP32-P4 hardware exactly.
 */

#ifndef LV_CONF_ESP32P4_H
#define LV_CONF_ESP32P4_H

/* ==========================================
 * ESP32-P4 Display Configuration
 * ========================================== */

/* Horizontal and vertical resolution of the display */
#define LV_HOR_RES_MAX          800
#define LV_VER_RES_MAX          480

/* Color depth: 16 (RGB565) or 32 (RGB888) */
#define LV_COLOR_DEPTH          16

/* Swap the 2 bytes of RGB565 color. Useful if the display has an 8-bit interface */
#define LV_COLOR_16_SWAP        0

/* ==========================================
 * Memory Configuration
 * ========================================== */

/* Size of the draw buffer in pixels (1/10 screen is typical) */
#define LV_VDB_SIZE             (800 * 480 / 10)

/* Use two draw buffers (double buffering) */
#define LV_VDB_DOUBLE           1

/* Set a custom `tick` source that tells the elapsed time in milliseconds */
#define LV_TICK_CUSTOM          1
#if LV_TICK_CUSTOM
#define LV_TICK_CUSTOM_INCLUDE  <stdint.h>         /* Header for the system time function */
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (SDL_GetTicks())   /* Expression evaluating to current system time in ms */
#endif   /*LV_TICK_CUSTOM*/

/* ==========================================
 * Feature Configuration
 * ========================================== */

/* Enable the GPU (not available on simulator) */
#define LV_USE_GPU              0

/* Enable performance monitor to show FPS */
#define LV_USE_PERF_MONITOR     1

/* Enable memory monitor to show memory usage */
#define LV_USE_MEM_MONITOR      1

/* Enable file system */
#define LV_USE_FS_STDIO         1
#define LV_FS_STDIO_LETTER      'S'

/* ==========================================
 * Font Configuration
 * ========================================== */

/* Montserrat fonts (default LVGL fonts) */
#define LV_FONT_MONTSERRAT_12   1
#define LV_FONT_MONTSERRAT_14   1
#define LV_FONT_MONTSERRAT_16   1
#define LV_FONT_MONTSERRAT_18   1
#define LV_FONT_MONTSERRAT_20   1
#define LV_FONT_MONTSERRAT_22   1
#define LV_FONT_MONTSERRAT_24   1
#define LV_FONT_MONTSERRAT_26   1
#define LV_FONT_MONTSERRAT_28   1
#define LV_FONT_MONTSERRAT_30   1
#define LV_FONT_MONTSERRAT_32   1
#define LV_FONT_MONTSERRAT_34   1
#define LV_FONT_MONTSERRAT_36   1
#define LV_FONT_MONTSERRAT_38   1
#define LV_FONT_MONTSERRAT_40   1
#define LV_FONT_MONTSERRAT_42   1
#define LV_FONT_MONTSERRAT_44   1
#define LV_FONT_MONTSERRAT_46   1
#define LV_FONT_MONTSERRAT_48   1

/* Default font */
#define LV_FONT_DEFAULT         &lv_font_montserrat_24

/* ==========================================
 * Widget Configuration
 * ========================================== */

/* Enable all widgets for full compatibility */
#define LV_USE_LABEL            1
#define LV_USE_BTN              1
#define LV_USE_IMG              1
#define LV_USE_SLIDER           1
#define LV_USE_SWITCH           1
#define LV_USE_CHECKBOX         1
#define LV_USE_DROPDOWN         1
#define LV_USE_ROLLER           1
#define LV_USE_TEXTAREA         1
#define LV_USE_KEYBOARD         1
#define LV_USE_CHART            1
#define LV_USE_TABLE            1
#define LV_USE_LIST             1
#define LV_USE_MENU             1
#define LV_USE_MSGBOX           1
#define LV_USE_SPINBOX          1
#define LV_USE_ARC              1
#define LV_USE_SPINNER          1
#define LV_USE_LED              1
#define LV_USE_BAR              1
#define LV_USE_CALENDAR         1
#define LV_USE_COLORWHEEL       1

/* ==========================================
 * Demo Configuration
 * ========================================== */

/* Enable LVGL demos */
#define LV_USE_DEMO_WIDGETS     1
#define LV_USE_DEMO_BENCHMARK   1
#define LV_USE_DEMO_MUSIC       1
#define LV_USE_DEMO_STRESS      1

/* ==========================================
 * Logging Configuration
 * ========================================== */

/* Enable the log module */
#define LV_USE_LOG              1
#if LV_USE_LOG
    /* How important log should be added:
     * LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
     * LV_LOG_LEVEL_INFO        Log important events
     * LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
     * LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
     * LV_LOG_LEVEL_USER        Only logs added by the user
     * LV_LOG_LEVEL_NONE        Do not log anything */
    #define LV_LOG_LEVEL        LV_LOG_LEVEL_WARN

    /* 1: Print the log with 'printf'; 0: User need to register a callback with `lv_log_register_print_cb()` */
    #define LV_LOG_PRINTF       1
#endif

/* ==========================================
 * ESP32-P4 Specific Notes
 * ========================================== */

/*
 * Physical Display: 800 x 1280 (portrait)
 * Logical Display:  800 x 480  (landscape - rotated 90°)
 * 
 * Color Format: RGB565 (16-bit)
 * Buffers:      Double buffering in SPIRAM
 * Refresh Rate: ~60 Hz
 * Interface:    MIPI-DSI 2-lane @ 1 Gbps
 * 
 * Frame Buffer Size:
 * - Single: 800 * 480 * 2 = 768 KB
 * - Double: 768 KB * 2 = 1.5 MB
 * 
 * Rotation: Software rotation enabled (.sw_rotate = true)
 */

#endif /* LV_CONF_ESP32P4_H */
