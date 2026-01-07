"""
JD9165BA MIPI-DSI Display Example - Using C Extension Module

This example demonstrates how to use the jd9165ba_display C extension module
which bypasses the potentially broken Python display framework.

Hardware:
- ESP32-P4 Function Evaluation Board
- JD9165BA MIPI-DSI display (800x1280)
- Backlight on GPIO 23

Key Differences from Python Driver:
- Direct C-level MIPI-DSI initialization
- No dependency on broken display frameworks
- Automatic LVGL integration
- Better stability and performance

Author: Custom C Extension Example
Date: 2025
"""

import lvgl as lv
import jd9165ba_display as disp

print("=" * 50)
print("JD9165BA MIPI-DSI Display - C Extension Module")
print("=" * 50)

# Step 1: Initialize display (C extension handles everything)
print("\n[1/4] Initializing display...")
try:
    disp.init()
    print("✅ Display initialized successfully!")
except Exception as e:
    print(f"❌ Display initialization failed: {e}")
    raise

# Step 2: Turn on backlight
print("\n[2/4] Turning on backlight...")
disp.set_backlight(100)  # 100% brightness
print("✅ Backlight enabled")

# Step 3: Get display information
print("\n[3/4] Display information:")
width = disp.get_width()
height = disp.get_height()
print(f"  Resolution: {width}x{height}")
print(f"  Color format: RGB565 (16-bit)")
print(f"  Buffer: Double buffering in SPIRAM")
print(f"  Backlight: GPIO 23")

# Step 4: Create LVGL UI
print("\n[4/4] Creating LVGL UI...")

# Create a screen
scr = lv.obj()
scr.set_style_bg_color(lv.color_hex(0x003366), 0)  # Dark blue background

# Title label
title = lv.label(scr)
title.set_text("JD9165BA C Extension")
title.set_style_text_font(lv.font_montserrat_32, 0)
title.set_style_text_color(lv.color_hex(0xFFFFFF), 0)
title.align(lv.ALIGN.TOP_MID, 0, 20)

# Info box
info_box = lv.obj(scr)
info_box.set_size(700, 300)
info_box.center()
info_box.set_style_bg_color(lv.color_hex(0xFFFFFF), 0)
info_box.set_style_radius(15, 0)
info_box.set_style_shadow_width(10, 0)
info_box.set_style_shadow_color(lv.color_hex(0x000000), 0)

# Status indicators
status_y = -100

# Status 1: MIPI-DSI
status1 = lv.label(info_box)
status1.set_text("✅ MIPI-DSI: 2-lane @ 1000 Mbps")
status1.set_style_text_color(lv.color_hex(0x00AA00), 0)
status1.set_style_text_font(lv.font_montserrat_20, 0)
status1.align(lv.ALIGN.CENTER, 0, status_y)

# Status 2: Display
status2 = lv.label(info_box)
status2.set_text(f"✅ Display: {width}x{height} RGB565")
status2.set_style_text_color(lv.color_hex(0x00AA00), 0)
status2.set_style_text_font(lv.font_montserrat_20, 0)
status2.align(lv.ALIGN.CENTER, 0, status_y + 40)

# Status 3: LVGL
status3 = lv.label(info_box)
status3.set_text("✅ LVGL: Registered & Ready")
status3.set_style_text_color(lv.color_hex(0x00AA00), 0)
status3.set_style_text_font(lv.font_montserrat_20, 0)
status3.align(lv.ALIGN.CENTER, 0, status_y + 80)

# Status 4: Backlight
status4 = lv.label(info_box)
status4.set_text("✅ Backlight: GPIO 23 (100%)")
status4.set_style_text_color(lv.color_hex(0x00AA00), 0)
status4.set_style_text_font(lv.font_montserrat_20, 0)
status4.align(lv.ALIGN.CENTER, 0, status_y + 120)

# Test button
btn = lv.btn(scr)
btn.set_size(200, 60)
btn.align(lv.ALIGN.BOTTOM_MID, 0, -30)
btn.set_style_bg_color(lv.color_hex(0xFF6600), 0)
btn.set_style_radius(10, 0)

btn_label = lv.label(btn)
btn_label.set_text("Test Complete")
btn_label.set_style_text_font(lv.font_montserrat_24, 0)
btn_label.center()

def btn_event_cb(e):
    print("\n✅ Button clicked - Display working!")
    title.set_text("✅ Display Working!")

btn.add_event_cb(btn_event_cb, lv.EVENT.CLICKED, None)

# Load the screen
lv.scr_load(scr)

print("✅ UI created successfully!")
print("\n" + "=" * 50)
print("Display Test Complete!")
print("=" * 50)
print("\nYou should see the test UI on the display.")
print("Press the button to test touch interaction.")
print("\nTo adjust backlight:")
print("  disp.set_backlight(0)   # Turn off")
print("  disp.set_backlight(100) # Turn on")
print("\nTo sleep/wake:")
print("  disp.sleep()  # Enter sleep mode")
print("  disp.wake()   # Wake from sleep")

# Main loop
print("\nEntering main loop...")
while True:
    lv.task_handler()
    lv.delay_ms(5)
