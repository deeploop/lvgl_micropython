# Copyright (c) 2024 - 2025 Kevin G. Schlosser
# JD9165BA MIPI-DSI Display Driver for LVGL MicroPython
# Compatible with ESP32-P4 MIPI-DSI interface

from micropython import const  # NOQA
import rgb_display_framework
import lvgl as lv
from . import _jd9165ba_init as init_cmds


STATE_HIGH = rgb_display_framework.STATE_HIGH
STATE_LOW = rgb_display_framework.STATE_LOW
STATE_PWM = rgb_display_framework.STATE_PWM

BYTE_ORDER_RGB = rgb_display_framework.BYTE_ORDER_RGB
BYTE_ORDER_BGR = rgb_display_framework.BYTE_ORDER_BGR

# Command constants
_MADCTL = const(0x36)


class JD9165BA(rgb_display_framework.RGBDisplayDriver):
    """
    JD9165BA MIPI-DSI Display Driver

    This driver supports JD9165BA-based MIPI-DSI displays (typically 800x1280)
    connected via ESP32-P4 MIPI-DSI interface.

    Example usage:
        from lcd_bus import DSIBus
        from jd9165ba import JD9165BA
        import lvgl as lv
        from machine import Pin

        # Create DSI bus
        dsi_bus = DSIBus(
            bus_id=0,
            data_lanes=2,
            freq=1000,  # 1000 Mbps per lane
            virtual_channel=0,
            hsync_front_porch=40,
            hsync_back_porch=140,
            hsync_pulse_width=40,
            vsync_front_porch=16,
            vsync_back_porch=16,
            vsync_pulse_width=4
        )

        # Initialize display
        display = JD9165BA(
            data_bus=dsi_bus,
            display_width=800,
            display_height=1280,
            backlight_pin=Pin(23, Pin.OUT),
            color_space=lv.COLOR_FORMAT.RGB565,
            rotation=90  # Rotate to landscape 800x480
        )

        display.init()
        display.set_backlight(100)
    """

    def __init__(
        self,
        data_bus,
        display_width,
        display_height,
        frame_buffer1=None,
        frame_buffer2=None,
        reset_pin=None,
        reset_state=STATE_HIGH,
        power_pin=None,
        power_on_state=STATE_HIGH,
        backlight_pin=None,
        backlight_on_state=STATE_HIGH,
        offset_x=0,
        offset_y=0,
        color_byte_order=BYTE_ORDER_RGB,
        color_space=lv.COLOR_FORMAT.RGB565,  # NOQA
        rgb565_byte_swap=False,
        rotation=0,  # 0, 90, 180, 270 degrees
    ):
        """
        Initialize JD9165BA MIPI-DSI display

        Args:
            data_bus: DSIBus instance
            display_width: Physical display width in pixels
            display_height: Physical display height in pixels
            frame_buffer1: First frame buffer (optional, auto-allocated if None)
            frame_buffer2: Second frame buffer for double buffering (optional)
            reset_pin: Reset pin (Pin object or None)
            reset_state: Active state for reset (STATE_HIGH or STATE_LOW)
            power_pin: Power control pin (Pin object or None)
            power_on_state: Active state for power (STATE_HIGH or STATE_LOW)
            backlight_pin: Backlight control pin (Pin object or None)
            backlight_on_state: Active state for backlight (STATE_HIGH, STATE_LOW, STATE_PWM)
            offset_x: X offset for display window (default 0)
            offset_y: Y offset for display window (default 0)
            color_byte_order: RGB or BGR byte order
            color_space: LVGL color format (RGB565 or RGB888)
            rgb565_byte_swap: Swap RGB565 bytes (default False)
            rotation: Display rotation in degrees (0, 90, 180, 270)
        """

        self._rotation = rotation
        self._color_byte_order = color_byte_order

        # MIPI-DSI uses DPI mode, no byte swap needed
        rgb565_byte_swap = False

        super().__init__(
            data_bus=data_bus,
            display_width=display_width,
            display_height=display_height,
            frame_buffer1=frame_buffer1,
            frame_buffer2=frame_buffer2,
            reset_pin=reset_pin,
            reset_state=reset_state,
            power_pin=power_pin,
            power_on_state=power_on_state,
            backlight_pin=backlight_pin,
            backlight_on_state=backlight_on_state,
            offset_x=offset_x,
            offset_y=offset_y,
            color_byte_order=color_byte_order,
            color_space=color_space,
            rgb565_byte_swap=rgb565_byte_swap,
            _cmd_bits=8,
            _param_bits=8,
            _init_bus=True
        )

    def init(self):
        """
        Initialize the display

        Sends initialization command sequence to JD9165BA controller
        and sets up rotation/color order.
        """
        # Get initialization commands
        cmds = init_cmds.JD9165BAInitCmd.get_init_cmds()

        # Execute init sequence
        init_sequence = []
        for cmd_data in cmds:
            if len(cmd_data) == 3:
                cmd, params, delay = cmd_data

                # Override MADCTL for rotation if needed
                if cmd == _MADCTL:
                    params = [init_cmds.JD9165BAInitCmd.get_rotation_cmd(
                        self._rotation,
                        self._color_byte_order == BYTE_ORDER_BGR
                    )]

                init_sequence.append((cmd, params, delay))

        # Call parent init with command sequence
        rgb_display_framework.RGBDisplayDriver.init(self, init_sequence)

    def set_rotation(self, rotation):
        """
        Set display rotation

        Args:
            rotation: Rotation angle (0, 90, 180, 270 degrees)
        """
        self._rotation = rotation
        madctl_value = init_cmds.JD9165BAInitCmd.get_rotation_cmd(
            rotation,
            self._color_byte_order == BYTE_ORDER_BGR
        )

        # Send MADCTL command
        self._data_bus.tx_param(_MADCTL, [madctl_value])

    def get_rotation(self):
        """
        Get current display rotation

        Returns:
            Current rotation angle in degrees
        """
        return self._rotation

    def sleep(self):
        """Enter sleep mode"""
        self._data_bus.tx_param(0x10, [])  # SLPIN

    def wake(self):
        """Exit sleep mode"""
        self._data_bus.tx_param(0x11, [])  # SLPOUT
        import time
        time.sleep_ms(120)  # Wait for panel to wake up
