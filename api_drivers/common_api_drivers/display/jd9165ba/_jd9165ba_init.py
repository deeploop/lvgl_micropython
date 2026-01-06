# Copyright (c) 2024 - 2025 Kevin G. Schlosser
# JD9165BA MIPI-DSI Display Initialization Commands
# Resolution: 800x1280 (commonly used with ESP32-P4)

from micropython import const  # NOQA

# JD9165BA Register Commands
_SLPOUT = const(0x11)  # Sleep Out
_DISPON = const(0x29)  # Display On
_SLPIN = const(0x10)  # Sleep In
_DISPOFF = const(0x28)  # Display Off

# Extended commands for JD9165BA
_SETEXTC = const(0xB9)  # Set extension command
_SETMIPI = const(0xBA)  # Set MIPI related register
_SETPOWER = const(0xB1)  # Set power control
_SETDISP = const(0xB2)  # Set display related register
_SETCYC = const(0xB4)  # Set display cycle timing
_SETVCOM = const(0xB6)  # Set VCOM voltage
_SETTE = const(0xB7)  # Set TE signal
_SETGIP0 = const(0xB8)  # Set GIP Option 0
_SETGIP1 = const(0xBA)  # Set GIP Option 1
_SETGIP2 = const(0xBC)  # Set GIP Option 2
_SETGIP3 = const(0xBE)  # Set GIP Option 3
_SETGAMMA = const(0xC7)  # Set Gamma
_SETPANEL = const(0xCC)  # Set panel related register
_SETCABC = const(0xC9)  # Set CABC control
_MADCTL = const(0x36)  # Memory Data Access Control
_COLMOD = const(0x3A)  # Interface Pixel Format

# Rotation constants
_MADCTL_MH = const(0x04)  # Refresh direction
_MADCTL_BGR = const(0x08)  # Color order
_MADCTL_ML = const(0x10)  # Line address order
_MADCTL_MV = const(0x20)  # Row/column exchange
_MADCTL_MX = const(0x40)  # Column address order
_MADCTL_MY = const(0x80)  # Row address order


class JD9165BAInitCmd:
    """
    JD9165BA initialization command sequence
    Based on ESP-IDF examples and JD9165BA datasheet
    """

    # Basic initialization sequence
    # Format: (command, [params...], delay_ms)
    init_cmds = [
        # Enable extended commands
        (_SETEXTC, [0xF1, 0x12, 0x83], 0),

        # Set MIPI mode
        (_SETMIPI, [0x33], 0),

        # Set power control
        # VRH, VCL, VGH, VGL settings
        (_SETPOWER, [
            0x00,  # VRH
            0x09,  # BT
            0x0C,  # VSPR
            0x0C,  # VSNR
            0x33,  # AP
            0x33,  # FS
        ], 0),

        # Set display cycle timing
        (_SETCYC, [
            0x80,  # NW
            0x08,  # RTN
            0x04,  # DIV
            0x26,  # DUM
            0x26,  # DUM
            0x04,  # GDON
            0x00,  # GDOFF
        ], 0),

        # Set VCOM voltage
        (_SETVCOM, [0x87], 0),

        # Set panel characteristics
        (_SETPANEL, [0x0B], 0),

        # Set gamma curve
        (_SETGAMMA, [
            0x00, 0x04, 0x09, 0x0C, 0x10, 0x15, 0x19, 0x1F,
            0x24, 0x2E, 0x38, 0x3E, 0x4A, 0x56, 0x5F, 0x66,
            0x6E, 0x76, 0x7F, 0x85, 0x8C, 0x94, 0x9C, 0xA5,
            0xAF, 0xB9, 0xC5, 0xD1, 0xDD, 0xE9, 0xF5, 0xFF,
        ], 0),

        # Set pixel format to RGB565 (16-bit)
        (_COLMOD, [0x55], 0),  # 0x55 = 16-bit/pixel RGB565

        # Set address mode (rotation, color order)
        # Default: Portrait, RGB order
        (_MADCTL, [0x00], 0),

        # Sleep out
        (_SLPOUT, [], 120),  # Wait 120ms after sleep out

        # Display on
        (_DISPON, [], 20),   # Wait 20ms after display on
    ]

    @staticmethod
    def get_init_cmds():
        """Get initialization command sequence"""
        return JD9165BAInitCmd.init_cmds

    @staticmethod
    def get_rotation_cmd(rotation, color_order_bgr=False):
        """
        Get MADCTL command for rotation

        Args:
            rotation: 0, 1, 2, or 3 (0°, 90°, 180°, 270°)
            color_order_bgr: True for BGR, False for RGB

        Returns:
            MADCTL register value
        """
        rotation_table = [
            0x00,  # 0°: Normal
            _MADCTL_MV | _MADCTL_MY,  # 90°: Row/col exchange, Y-flip
            _MADCTL_MX | _MADCTL_MY,  # 180°: X-flip, Y-flip
            _MADCTL_MV | _MADCTL_MX,  # 270°: Row/col exchange, X-flip
        ]

        value = rotation_table[rotation & 0x03]

        if color_order_bgr:
            value |= _MADCTL_BGR

        return value
