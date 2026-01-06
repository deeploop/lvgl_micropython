# 自定义 MIPI-DSI 显示驱动指南
# Custom MIPI-DSI Display Driver Guide for ESP32-P4

本指南展示如何为 LVGL MicroPython 添加自定义 MIPI-DSI 显示驱动。
This guide shows how to add custom MIPI-DSI display drivers to LVGL MicroPython.

---

## 📋 目录 / Table of Contents

1. [概述 / Overview](#overview)
2. [JD9165BA 驱动示例 / JD9165BA Driver Example](#jd9165ba-example)
3. [驱动结构 / Driver Structure](#driver-structure)
4. [构建固件 / Building Firmware](#building-firmware)
5. [使用驱动 / Using the Driver](#using-the-driver)
6. [调试技巧 / Debugging Tips](#debugging-tips)

---

## <a name="overview"></a>📖 概述 / Overview

### 为什么需要自定义驱动？/ Why Custom Driver?

ESP32-P4 的 MIPI-DSI 接口支持多种显示控制器，但 MicroPython LVGL 默认不包含所有驱动。
ESP32-P4's MIPI-DSI interface supports various display controllers, but MicroPython LVGL doesn't include all drivers by default.

**已实现 / Implemented:**
- ✅ MIPI-DSI 总线支持 (C 层) / MIPI-DSI bus support (C layer)
- ✅ DPI 面板接口 / DPI panel interface

**需要添加 / Need to Add:**
- ❌ 特定显示控制器初始化 / Specific display controller initialization
- ❌ Python 驱动包装 / Python driver wrapper

### 支持的显示控制器 / Supported Display Controllers

本指南提供 **JD9165BA** 驱动示例，你可以按照相同模式添加其他控制器：
This guide provides **JD9165BA** driver example. You can add other controllers following the same pattern:

- JD9165BA (已实现 / Implemented ✅)
- ili9881c (可参考 JD9165BA / Can reference JD9165BA)
- ek79007 (可参考 JD9165BA / Can reference JD9165BA)
- 其他 MIPI-DSI 控制器 / Other MIPI-DSI controllers

---

## <a name="jd9165ba-example"></a>🎯 JD9165BA 驱动示例 / JD9165BA Driver Example

### 文件结构 / File Structure

```
api_drivers/common_api_drivers/display/jd9165ba/
├── __init__.py              # 模块入口 / Module entry
├── jd9165ba.py              # 主驱动类 / Main driver class
└── _jd9165ba_init.py        # 初始化命令 / Initialization commands
```

### 核心组件 / Core Components

#### 1. 初始化命令 (`_jd9165ba_init.py`)

```python
# 定义显示控制器寄存器 / Define display controller registers
_SLPOUT = const(0x11)  # Sleep Out
_DISPON = const(0x29)  # Display On
_SETEXTC = const(0xB9)  # Set extension command
# ... 更多寄存器 / More registers

class JD9165BAInitCmd:
    # 初始化命令序列 / Initialization command sequence
    init_cmds = [
        (_SETEXTC, [0xF1, 0x12, 0x83], 0),  # (命令, [参数], 延迟ms)
        (_SETPOWER, [0x00, 0x09, 0x0C], 0),
        # ... 更多命令 / More commands
        (_SLPOUT, [], 120),  # 等待 120ms / Wait 120ms
        (_DISPON, [], 20),   # 等待 20ms / Wait 20ms
    ]
```

**关键点 / Key Points:**
- 命令格式：`(command_code, [param1, param2, ...], delay_ms)`
- 必须包含：Sleep Out (`0x11`) 和 Display On (`0x29`)
- 延迟时间根据数据手册设置 / Delay timing from datasheet

#### 2. 主驱动类 (`jd9165ba.py`)

```python
from rgb_display_framework import RGBDisplayDriver
from . import _jd9165ba_init as init_cmds

class JD9165BA(RGBDisplayDriver):
    def __init__(self, data_bus, display_width, display_height, ...):
        super().__init__(
            data_bus=data_bus,
            display_width=display_width,
            display_height=display_height,
            # ... 其他参数 / Other parameters
        )

    def init(self):
        # 获取初始化命令 / Get init commands
        cmds = init_cmds.JD9165BAInitCmd.get_init_cmds()
        # 执行初始化 / Execute initialization
        RGBDisplayDriver.init(self, cmds)
```

**关键点 / Key Points:**
- 继承 `RGBDisplayDriver` (MIPI-DSI 使用 DPI 模式)
- 在 `init()` 中发送初始化命令序列
- 支持旋转、颜色顺序等配置

---

## <a name="driver-structure"></a>🏗️ 驱动结构 / Driver Structure

### 目录布局 / Directory Layout

```
lvgl_micropython/
├── api_drivers/
│   └── common_api_drivers/
│       └── display/
│           ├── jd9165ba/           # 你的自定义驱动 / Your custom driver
│           │   ├── __init__.py
│           │   ├── jd9165ba.py
│           │   └── _jd9165ba_init.py
│           └── [other_drivers]/
├── ext_mod/
│   └── lcd_bus/
│       └── esp32_src/
│           └── dsi_bus.c           # MIPI-DSI 总线 (C 层) / DSI bus (C layer)
└── ESP32P4/
    ├── jd9165ba_example.py         # 使用示例 / Usage example
    └── CUSTOM_DRIVER_GUIDE.md      # 本文档 / This document
```

### 依赖关系 / Dependencies

```
JD9165BA 驱动 / JD9165BA Driver
    ↓
RGBDisplayDriver (Python 框架 / Python framework)
    ↓
DSIBus (C 实现 / C implementation)
    ↓
ESP-IDF MIPI-DSI API
    ↓
ESP32-P4 硬件 / Hardware
```

---

## <a name="building-firmware"></a>🔨 构建固件 / Building Firmware

### 方法 1：使用 DISPLAY=all 构建 / Build with DISPLAY=all

```bash
# 包含所有显示驱动（包括 JD9165BA）/ Include all display drivers (including JD9165BA)
python3 make.py esp32 BOARD=ESP32_GENERIC_P4 --flash-size=16 DISPLAY=all INDEV=all EXPANDER=all
```

**输出 / Output:** `build/ESP32_GENERIC_P4-16.bin`

### 方法 2：仅包含特定驱动 / Build with Specific Driver Only

```bash
# 仅包含 JD9165BA 驱动 / Include only JD9165BA driver
python3 make.py esp32 BOARD=ESP32_GENERIC_P4 --flash-size=16 DISPLAY=jd9165ba INDEV=gt911
```

**优点 / Advantages:**
- ✅ 固件体积更小 / Smaller firmware size
- ✅ 启动更快 / Faster boot
- ✅ 避免驱动冲突 / Avoid driver conflicts

### 方法 3：从 GitHub Actions 下载 / Download from GitHub Actions

一旦驱动提交到仓库，GitHub Actions 会自动构建：
Once the driver is committed to the repository, GitHub Actions will automatically build:

1. 访问 / Visit: https://github.com/your-repo/lvgl_micropython/actions
2. 选择最新的构建 / Select latest build
3. 下载 `lvgl_micropy_ESP32` 文件 / Download artifact
4. 解压并刷写 `.bin` 文件 / Extract and flash `.bin` file

---

## <a name="using-the-driver"></a>🚀 使用驱动 / Using the Driver

### 完整示例 / Complete Example

```python
import lvgl as lv
from machine import Pin
from lcd_bus import DSIBus
from jd9165ba import JD9165BA

# 初始化 LVGL / Initialize LVGL
lv.init()

# 创建 MIPI-DSI 总线 / Create MIPI-DSI bus
dsi_bus = DSIBus(
    bus_id=0,
    data_lanes=2,              # 2 数据通道 / 2 data lanes
    freq=1000,                 # 1000 Mbps 每通道 / per lane
    virtual_channel=0,
    hsync_front_porch=40,
    hsync_back_porch=140,
    hsync_pulse_width=40,
    vsync_front_porch=16,
    vsync_back_porch=16,
    vsync_pulse_width=4
)

# 创建显示驱动 / Create display driver
display = JD9165BA(
    data_bus=dsi_bus,
    display_width=800,         # 物理宽度 / Physical width
    display_height=1280,       # 物理高度 / Physical height
    backlight_pin=Pin(23, Pin.OUT),
    color_space=lv.COLOR_FORMAT.RGB565,
    rotation=90                # 旋转到横屏 / Rotate to landscape
)

# 初始化显示 / Initialize display
display.init()

# 设置背光 / Set backlight
display.set_backlight(100)  # 100% 亮度 / brightness

# 创建 UI / Create UI
scr = lv.obj()
label = lv.label(scr)
label.set_text("Hello JD9165BA!")
label.center()
lv.scr_load(scr)

# 主循环 / Main loop
while True:
    lv.task_handler()
    lv.delay_ms(5)
```

### 参数说明 / Parameter Description

| 参数 / Parameter | 说明 / Description | 示例 / Example |
|-----------------|-------------------|---------------|
| `bus_id` | MIPI-DSI 总线 ID / Bus ID | `0` |
| `data_lanes` | 数据通道数 / Number of data lanes | `2` or `4` |
| `freq` | 每通道速率 (Mbps) / Bitrate per lane | `1000` (1 Gbps) |
| `virtual_channel` | 虚拟通道 / Virtual channel | `0` |
| `hsync_*` | 水平时序 / Horizontal timing | 参考数据手册 / See datasheet |
| `vsync_*` | 垂直时序 / Vertical timing | 参考数据手册 / See datasheet |
| `display_width` | 物理宽度 (像素) / Physical width | `800` |
| `display_height` | 物理高度 (像素) / Physical height | `1280` |
| `rotation` | 旋转角度 / Rotation angle | `0`, `90`, `180`, `270` |
| `color_space` | 颜色格式 / Color format | `RGB565`, `RGB888` |

---

## <a name="debugging-tips"></a>🐛 调试技巧 / Debugging Tips

### 1. 检查硬件连接 / Check Hardware Connections

```python
# 测试背光 / Test backlight
from machine import Pin
bl = Pin(23, Pin.OUT)
bl.value(1)  # 应该看到背光点亮 / Should see backlight on
```

### 2. 检查 I2C 触摸控制器 / Check I2C Touch Controller

```python
from machine import I2C, Pin
i2c = I2C(0, scl=Pin(8), sda=Pin(7), freq=400000)
devices = i2c.scan()
print(f"I2C devices: {[hex(d) for d in devices]}")
# 应该看到: ['0x5d'] (GT911 触摸控制器) / Should see GT911 at 0x5D
```

### 3. 启用调试日志 / Enable Debug Logging

```python
# 在驱动初始化前添加 / Add before driver initialization
import sys
sys.path.append('/flash')  # 如果驱动在 flash 存储 / If driver in flash storage

# 打印 DSI 总线配置 / Print DSI bus config
print(f"DSI bus: {dsi_bus}")
print(f"Display: {display}")
```

### 4. 常见问题 / Common Issues

| 问题 / Issue | 原因 / Cause | 解决方案 / Solution |
|-------------|--------------|-------------------|
| 黑屏 / Black screen | 背光未开启 / Backlight not on | 检查 GPIO 23 / Check GPIO 23 |
| 启动崩溃 / Boot crash | 初始化命令错误 / Wrong init commands | 检查命令序列 / Verify command sequence |
| 颜色错误 / Wrong colors | RGB/BGR 顺序 / RGB/BGR order | 设置 `color_byte_order=BYTE_ORDER_BGR` |
| 旋转问题 / Rotation issue | MADCTL 配置 / MADCTL config | 调整 `rotation` 参数 / Adjust rotation parameter |

### 5. 串口调试 / Serial Debugging

```python
# 添加调试输出 / Add debug output
print("Step 1: Creating DSI bus...")
dsi_bus = DSIBus(...)
print("Step 2: Creating display...")
display = JD9165BA(...)
print("Step 3: Initializing...")
display.init()
print("Step 4: Display ready!")
```

---

## 🎓 添加其他显示控制器 / Adding Other Display Controllers

### 步骤 / Steps

1. **创建驱动目录 / Create driver directory:**
   ```bash
   mkdir -p api_drivers/common_api_drivers/display/your_controller
   ```

2. **编写初始化命令 / Write initialization commands:**
   - 参考控制器数据手册 / Reference controller datasheet
   - 复制 `_jd9165ba_init.py` 作为模板 / Copy as template

3. **编写驱动类 / Write driver class:**
   - 复制 `jd9165ba.py` 作为模板 / Copy as template
   - 继承 `RGBDisplayDriver`

4. **创建 `__init__.py` / Create module init:**
   ```python
   from .your_controller import YourController
   __all__ = ['YourController']
   ```

5. **测试驱动 / Test driver:**
   - 创建测试脚本 / Create test script
   - 验证显示输出 / Verify display output

6. **提交到仓库 / Commit to repository:**
   ```bash
   git add api_drivers/common_api_drivers/display/your_controller
   git commit -m "Add your_controller MIPI-DSI driver"
   git push
   ```

---

## 📚 参考资源 / References

### ESP-IDF 文档 / ESP-IDF Documentation
- [MIPI-DSI LCD API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32p4/api-reference/peripherals/lcd.html#mipi-dsi-interfaced-lcd)
- [ESP32-P4 LCD 示例](https://github.com/espressif/esp-idf/tree/master/examples/peripherals/lcd/mipi_dsi)

### 硬件参考 / Hardware References
- [ESP32-P4 数据手册](https://www.espressif.com/sites/default/files/documentation/esp32-p4_datasheet_en.pdf)
- [ESP32-P4 Function EV Board 用户指南](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/user_guide.html)

### LVGL 文档 / LVGL Documentation
- [LVGL 官方文档](https://docs.lvgl.io/)
- [LVGL MicroPython 绑定](https://github.com/lvgl/lv_binding_micropython)

---

## ✅ 检查清单 / Checklist

完成自定义驱动后检查 / Check after completing custom driver:

- [ ] 初始化命令序列正确 / Init command sequence correct
- [ ] 旋转和颜色顺序正常 / Rotation and color order work
- [ ] 背光控制正常 / Backlight control works
- [ ] 帧缓冲分配正确 / Frame buffer allocation correct
- [ ] 睡眠/唤醒功能正常 / Sleep/wake functions work
- [ ] 示例代码可运行 / Example code runs
- [ ] 文档完整 / Documentation complete
- [ ] 提交到 Git / Committed to Git

---

**祝你成功！/ Good luck!** 🎉

如有问题，请参考：
For questions, refer to:
- ESP32P4/README.md
- ESP32P4/jd9165ba_example.py
- https://github.com/lvgl-micropython/lvgl_micropython/issues
