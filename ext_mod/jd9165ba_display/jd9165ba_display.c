/**
 * @file jd9165ba_display.c
 * @brief JD9165BA MIPI-DSI Display Driver - C Implementation
 *
 * Direct C-level MIPI-DSI initialization for JD9165BA displays on ESP32-P4
 */

#include "jd9165ba_display.h"

#if SOC_MIPI_DSI_SUPPORTED

#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "JD9165BA";

// Global handles
static esp_lcd_dsi_bus_handle_t dsi_bus = NULL;
static esp_lcd_panel_io_handle_t panel_io = NULL;
static esp_lcd_panel_handle_t panel = NULL;
static lv_disp_t *lv_disp = NULL;

// Frame buffers
static uint8_t *fb1 = NULL;
static uint8_t *fb2 = NULL;

/**
 * @brief Send initialization command to JD9165BA
 */
static void jd9165ba_send_cmd(uint8_t cmd, const uint8_t *params, uint8_t param_count, uint32_t delay_ms)
{
    if (panel_io == NULL) {
        ESP_LOGE(TAG, "Panel IO not initialized");
        return;
    }

    esp_lcd_panel_io_tx_param(panel_io, cmd, params, param_count);

    if (delay_ms > 0) {
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

/**
 * @brief Initialize JD9165BA display controller
 */
static esp_err_t jd9165ba_controller_init(void)
{
    ESP_LOGI(TAG, "Initializing JD9165BA controller...");

    // Enable extended commands
    const uint8_t setextc_params[] = {0xF1, 0x12, 0x83};
    jd9165ba_send_cmd(JD9165BA_CMD_SETEXTC, setextc_params, 3, 0);

    // Set MIPI mode
    const uint8_t setmipi_params[] = {0x33};
    jd9165ba_send_cmd(JD9165BA_CMD_SETMIPI, setmipi_params, 1, 0);

    // Set power control
    const uint8_t setpower_params[] = {0x00, 0x09, 0x0C, 0x0C, 0x33, 0x33};
    jd9165ba_send_cmd(JD9165BA_CMD_SETPOWER, setpower_params, 6, 0);

    // Set display cycle
    const uint8_t setcyc_params[] = {0x80, 0x08, 0x04, 0x26, 0x26, 0x04, 0x00};
    jd9165ba_send_cmd(JD9165BA_CMD_SETCYC, setcyc_params, 7, 0);

    // Set VCOM voltage
    const uint8_t setvcom_params[] = {0x87};
    jd9165ba_send_cmd(JD9165BA_CMD_SETVCOM, setvcom_params, 1, 0);

    // Set panel characteristics
    const uint8_t setpanel_params[] = {0x0B};
    jd9165ba_send_cmd(JD9165BA_CMD_SETPANEL, setpanel_params, 1, 0);

    // Set gamma curve
    const uint8_t setgamma_params[] = {
        0x00, 0x04, 0x09, 0x0C, 0x10, 0x15, 0x19, 0x1F,
        0x24, 0x2E, 0x38, 0x3E, 0x4A, 0x56, 0x5F, 0x66,
        0x6E, 0x76, 0x7F, 0x85, 0x8C, 0x94, 0x9C, 0xA5,
        0xAF, 0xB9, 0xC5, 0xD1, 0xDD, 0xE9, 0xF5, 0xFF
    };
    jd9165ba_send_cmd(JD9165BA_CMD_SETGAMMA, setgamma_params, 32, 0);

    // Set pixel format to RGB565
    const uint8_t colmod_params[] = {0x55};  // 16-bit/pixel
    jd9165ba_send_cmd(JD9165BA_CMD_COLMOD, colmod_params, 1, 0);

    // Set rotation (90° for landscape)
    const uint8_t madctl_params[] = {0x60};  // MV=1, MY=1 for 90° rotation
    jd9165ba_send_cmd(JD9165BA_CMD_MADCTL, madctl_params, 1, 0);

    // Sleep out
    jd9165ba_send_cmd(JD9165BA_CMD_SLPOUT, NULL, 0, 120);

    // Display on
    jd9165ba_send_cmd(JD9165BA_CMD_DISPON, NULL, 0, 20);

    ESP_LOGI(TAG, "JD9165BA controller initialized");
    return ESP_OK;
}

/**
 * @brief LVGL flush callback
 */
static void jd9165ba_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    // For MIPI-DSI DPI mode, flush is handled automatically by hardware
    // Just mark as flushed
    lv_disp_flush_ready(drv);
}

/**
 * @brief Initialize MIPI-DSI bus
 */
static esp_err_t jd9165ba_init_dsi_bus(void)
{
    ESP_LOGI(TAG, "Initializing MIPI-DSI bus...");

    // Configure DSI bus
    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = JD9165BA_DSI_BUS_ID,
        .num_data_lanes = JD9165BA_DSI_LANES,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = JD9165BA_DSI_FREQ_MHZ,
    };

    ESP_ERROR_CHECK(esp_lcd_new_dsi_bus(&bus_config, &dsi_bus));
    ESP_LOGI(TAG, "DSI bus initialized (%d lanes @ %d Mbps)",
             JD9165BA_DSI_LANES, JD9165BA_DSI_FREQ_MHZ);

    return ESP_OK;
}

/**
 * @brief Initialize panel IO
 */
static esp_err_t jd9165ba_init_panel_io(void)
{
    ESP_LOGI(TAG, "Initializing panel IO...");

    esp_lcd_dbi_io_config_t io_config = {
        .virtual_channel = JD9165BA_VIRTUAL_CHANNEL,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_io_dsi(dsi_bus, &io_config, &panel_io));
    ESP_LOGI(TAG, "Panel IO initialized");

    return ESP_OK;
}

/**
 * @brief Initialize DPI panel
 */
static esp_err_t jd9165ba_init_dpi_panel(void)
{
    ESP_LOGI(TAG, "Initializing DPI panel...");

    esp_lcd_dpi_panel_config_t panel_config = {
        .virtual_channel = JD9165BA_VIRTUAL_CHANNEL,
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = JD9165BA_DSI_FREQ_MHZ,
        .pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565,
        .video_timing = {
            .h_size = JD9165BA_LOGICAL_WIDTH,
            .v_size = JD9165BA_LOGICAL_HEIGHT,
            .hsync_back_porch = JD9165BA_HSYNC_BACK_PORCH,
            .hsync_pulse_width = JD9165BA_HSYNC_PULSE_WIDTH,
            .hsync_front_porch = JD9165BA_HSYNC_FRONT_PORCH,
            .vsync_back_porch = JD9165BA_VSYNC_BACK_PORCH,
            .vsync_pulse_width = JD9165BA_VSYNC_PULSE_WIDTH,
            .vsync_front_porch = JD9165BA_VSYNC_FRONT_PORCH,
        },
        .num_fbs = 2,  // Double buffering
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_dpi(dsi_bus, &panel_config, &panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel));

    ESP_LOGI(TAG, "DPI panel initialized (%dx%d)",
             JD9165BA_LOGICAL_WIDTH, JD9165BA_LOGICAL_HEIGHT);

    return ESP_OK;
}

/**
 * @brief Initialize backlight GPIO
 */
static esp_err_t jd9165ba_init_backlight(void)
{
    ESP_LOGI(TAG, "Initializing backlight (GPIO %d)...", JD9165BA_PIN_BACKLIGHT);

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << JD9165BA_PIN_BACKLIGHT),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&io_conf));
    gpio_set_level(JD9165BA_PIN_BACKLIGHT, 0);  // Start with backlight off

    ESP_LOGI(TAG, "Backlight initialized");
    return ESP_OK;
}

/**
 * @brief Initialize LVGL display driver
 */
static esp_err_t jd9165ba_init_lvgl(void)
{
    ESP_LOGI(TAG, "Initializing LVGL display driver...");

    // Allocate LVGL draw buffers
    fb1 = heap_caps_malloc(JD9165BA_BUFFER_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    fb2 = heap_caps_malloc(JD9165BA_BUFFER_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    if (!fb1 || !fb2) {
        ESP_LOGE(TAG, "Failed to allocate framebuffers");
        return ESP_ERR_NO_MEM;
    }

    ESP_LOGI(TAG, "Framebuffers allocated (2 x %d bytes in SPIRAM)", JD9165BA_BUFFER_SIZE);

    // Initialize LVGL
    lv_init();

    // Create LVGL display driver
    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, fb1, fb2,
                          JD9165BA_LOGICAL_WIDTH * JD9165BA_LOGICAL_HEIGHT);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = JD9165BA_LOGICAL_WIDTH;
    disp_drv.ver_res = JD9165BA_LOGICAL_HEIGHT;
    disp_drv.flush_cb = jd9165ba_lvgl_flush_cb;
    disp_drv.draw_buf = &draw_buf;

    lv_disp = lv_disp_drv_register(&disp_drv);

    ESP_LOGI(TAG, "LVGL display registered (%dx%d)",
             JD9165BA_LOGICAL_WIDTH, JD9165BA_LOGICAL_HEIGHT);

    return ESP_OK;
}

//=============================================================================
// MicroPython API Functions
//=============================================================================

/**
 * @brief Initialize JD9165BA display (MicroPython: jd9165ba_display.init())
 */
mp_obj_t jd9165ba_init(void)
{
    ESP_LOGI(TAG, "=== JD9165BA Display Initialization ===");

    // Check if already initialized
    if (dsi_bus != NULL) {
        ESP_LOGW(TAG, "Display already initialized");
        return mp_const_none;
    }

    // Step 1: Initialize MIPI-DSI bus
    if (jd9165ba_init_dsi_bus() != ESP_OK) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to initialize DSI bus"));
    }

    // Step 2: Initialize panel IO
    if (jd9165ba_init_panel_io() != ESP_OK) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to initialize panel IO"));
    }

    // Step 3: Initialize DPI panel
    if (jd9165ba_init_dpi_panel() != ESP_OK) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to initialize DPI panel"));
    }

    // Step 4: Initialize JD9165BA controller
    if (jd9165ba_controller_init() != ESP_OK) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to initialize JD9165BA controller"));
    }

    // Step 5: Initialize backlight
    if (jd9165ba_init_backlight() != ESP_OK) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to initialize backlight"));
    }

    // Step 6: Initialize LVGL
    if (jd9165ba_init_lvgl() != ESP_OK) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed to initialize LVGL"));
    }

    ESP_LOGI(TAG, "=== JD9165BA Display Initialized Successfully ===");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(jd9165ba_init_obj, jd9165ba_init);

/**
 * @brief Set backlight brightness (MicroPython: jd9165ba_display.set_backlight(brightness))
 */
mp_obj_t jd9165ba_set_backlight(mp_obj_t brightness_obj)
{
    int brightness = mp_obj_get_int(brightness_obj);

    if (brightness < 0 || brightness > 100) {
        mp_raise_ValueError(MP_ERROR_TEXT("Brightness must be 0-100"));
    }

    // For now, simple on/off control
    // TODO: Implement PWM for brightness levels
    int level = (brightness > 0) ? 1 : 0;
    gpio_set_level(JD9165BA_PIN_BACKLIGHT, level);

    ESP_LOGI(TAG, "Backlight set to %d%% (GPIO level: %d)", brightness, level);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(jd9165ba_set_backlight_obj, jd9165ba_set_backlight);

/**
 * @brief Get display width
 */
mp_obj_t jd9165ba_get_width(void)
{
    return mp_obj_new_int(JD9165BA_LOGICAL_WIDTH);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(jd9165ba_get_width_obj, jd9165ba_get_width);

/**
 * @brief Get display height
 */
mp_obj_t jd9165ba_get_height(void)
{
    return mp_obj_new_int(JD9165BA_LOGICAL_HEIGHT);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(jd9165ba_get_height_obj, jd9165ba_get_height);

/**
 * @brief Enter sleep mode
 */
mp_obj_t jd9165ba_sleep(void)
{
    ESP_LOGI(TAG, "Entering sleep mode");
    jd9165ba_send_cmd(JD9165BA_CMD_SLPIN, NULL, 0, 120);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(jd9165ba_sleep_obj, jd9165ba_sleep);

/**
 * @brief Wake from sleep mode
 */
mp_obj_t jd9165ba_wake(void)
{
    ESP_LOGI(TAG, "Waking from sleep");
    jd9165ba_send_cmd(JD9165BA_CMD_SLPOUT, NULL, 0, 120);
    jd9165ba_send_cmd(JD9165BA_CMD_DISPON, NULL, 0, 20);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(jd9165ba_wake_obj, jd9165ba_wake);

/**
 * @brief Deinitialize display
 */
mp_obj_t jd9165ba_deinit(void)
{
    ESP_LOGI(TAG, "Deinitializing display");

    // Turn off backlight
    gpio_set_level(JD9165BA_PIN_BACKLIGHT, 0);

    // Turn off display
    jd9165ba_send_cmd(JD9165BA_CMD_DISPOFF, NULL, 0, 20);
    jd9165ba_send_cmd(JD9165BA_CMD_SLPIN, NULL, 0, 120);

    // Free framebuffers
    if (fb1) {
        heap_caps_free(fb1);
        fb1 = NULL;
    }
    if (fb2) {
        heap_caps_free(fb2);
        fb2 = NULL;
    }

    // Delete panel
    if (panel) {
        esp_lcd_panel_del(panel);
        panel = NULL;
    }

    // Delete panel IO
    if (panel_io) {
        esp_lcd_panel_io_del(panel_io);
        panel_io = NULL;
    }

    // Delete DSI bus
    if (dsi_bus) {
        esp_lcd_del_dsi_bus(dsi_bus);
        dsi_bus = NULL;
    }

    ESP_LOGI(TAG, "Display deinitialized");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(jd9165ba_deinit_obj, jd9165ba_deinit);

//=============================================================================
// MicroPython Module Definition
//=============================================================================

STATIC const mp_rom_map_elem_t jd9165ba_display_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_jd9165ba_display) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&jd9165ba_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&jd9165ba_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_backlight), MP_ROM_PTR(&jd9165ba_set_backlight_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_width), MP_ROM_PTR(&jd9165ba_get_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_height), MP_ROM_PTR(&jd9165ba_get_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_sleep), MP_ROM_PTR(&jd9165ba_sleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_wake), MP_ROM_PTR(&jd9165ba_wake_obj) },
};
STATIC MP_DEFINE_CONST_DICT(jd9165ba_display_globals, jd9165ba_display_globals_table);

const mp_obj_module_t jd9165ba_display_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&jd9165ba_display_globals,
};

MP_REGISTER_MODULE(MP_QSTR_jd9165ba_display, jd9165ba_display_module);

#endif // SOC_MIPI_DSI_SUPPORTED
