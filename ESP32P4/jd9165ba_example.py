"""
JD9165BA MIPI-DSI Display Example for ESP32-P4 Function EV Board

This example demonstrates how to use the custom JD9165BA driver
with ESP32-P4's MIPI-DSI interface.

Hardware:
- ESP32-P4 Function Evaluation Board
- JD9165BA display controller (800x1280 MIPI-DSI)
- GT911 touch controller (optional)

Author: Custom Driver Example
Date: 2025
"""

import lvgl as lv
from machine import Pin, I2C
from lcd_bus import DSIBus
from jd9165ba import JD9165BA

# Initialize LVGL
lv.init()

print("Initializing JD9165BA MIPI-DSI Display...")

# Step 1: Create MIPI-DSI Bus
# Configure for 2-lane MIPI-DSI @ 1000 Mbps per lane
dsi_bus = DSIBus(
    bus_id=0,                   # MIPI-DSI bus 0
    data_lanes=2,               # 2 data lanes
    freq=1000,                  # 1000 Mbps (1 Gbps) per lane
    virtual_channel=0,          # Virtual channel 0
    # Display timing parameters for 800x1280 panel
    hsync_front_porch=40,       # Horizontal front porch
    hsync_back_porch=140,       # Horizontal back porch
    hsync_pulse_width=40,       # Horizontal sync pulse width
    vsync_front_porch=16,       # Vertical front porch
    vsync_back_porch=16,        # Vertical back porch
    vsync_pulse_width=4         # Vertical sync pulse width
)

print("DSI bus initialized")

# Step 2: Create Display Driver Instance
display = JD9165BA(
    data_bus=dsi_bus,
    display_width=800,           # Physical width
    display_height=1280,         # Physical height (portrait mode)
    backlight_pin=Pin(23, Pin.OUT),  # Backlight on GPIO 23
    backlight_on_state=JD9165BA.STATE_HIGH,
    color_space=lv.COLOR_FORMAT.RGB565,  # RGB565 color format
    rotation=90                  # Rotate 90° to landscape (800x480)
)

print("Display driver created")

# Step 3: Initialize Display
display.init()
print("Display initialized")

# Step 4: Set Backlight
display.set_backlight(100)  # 100% brightness
print("Backlight enabled")

# Step 5: Create LVGL Display
# After rotation, display is 800x480 (landscape)
print(f"Display size: {display.get_width()}x{display.get_height()}")

# Step 6 (Optional): Initialize Touch Controller (GT911)
try:
    i2c = I2C(0, scl=Pin(8), sda=Pin(7), freq=400000)
    devices = i2c.scan()
    print(f"I2C devices found: {[hex(d) for d in devices]}")

    # Import and initialize GT911 touch driver if available
    # from gt911 import GT911
    # touch = GT911(i2c)
    # print("Touch controller initialized")
except Exception as e:
    print(f"Touch controller not available: {e}")

# Step 7: Test Display with LVGL
print("Creating test UI...")

# Create a screen
scr = lv.obj()

# Create a label
label = lv.label(scr)
label.set_text("JD9165BA MIPI-DSI Display\nESP32-P4 Function EV Board\n800x1280 @ 90° = 800x480")
label.set_style_text_font(lv.font_montserrat_24, 0)
label.center()

# Create a colored box
box = lv.obj(scr)
box.set_size(200, 100)
box.set_pos(300, 50)
box.set_style_bg_color(lv.color_hex(0x2196F3), 0)
box.set_style_radius(10, 0)

box_label = lv.label(box)
box_label.set_text("LVGL Demo")
box_label.center()

# Create a button
btn = lv.btn(scr)
btn.set_size(150, 60)
btn.set_pos(325, 350)

btn_label = lv.label(btn)
btn_label.set_text("Click Me")
btn_label.center()

def btn_event_cb(e):
    print("Button clicked!")
    label.set_text("Button was clicked!\nJD9165BA Working!")

btn.add_event_cb(btn_event_cb, lv.EVENT.CLICKED, None)

# Load the screen
lv.scr_load(scr)

print("Display test complete!")
print("You should see the test UI on the display.")
print("\nDisplay info:")
print(f"  Resolution: 800x480 (landscape)")
print(f"  Color format: RGB565")
print(f"  Bus: MIPI-DSI 2-lane @ 1000 Mbps")
print(f"  Backlight: GPIO 23")

# Keep the program running
while True:
    lv.task_handler()
    lv.delay_ms(5)
