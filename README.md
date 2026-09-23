# OpenMV Cam H7 原生 USB 摄像头（MJPEG）

这是我把 OpenMV Cam H7（固件目标 `OPENMV4`）改造成 Windows 原生 UVC 摄像头的开发记录。设备通过 Windows 自带的 `usbvideo.sys` 驱动识别，无需 OBS 虚拟摄像头或电脑端转发程序。

- [完整开发报告](DEVELOPMENT_REPORT.md)：需求、我的工作、调试经过、故障与解决、实测数据和局限。
- [最终固件](firmware/openmv-h7-uvc-mjpeg-vflip.bin)：已在实机刷入验证，MJPEG、垂直翻转已启用。
- [源码补丁](patches/openmv-v4.7.0-uvc-mjpeg.patch)：基于 [OpenMV v4.7.0](https://github.com/openmv/openmv/tree/v4.7.0)，基础提交 `2206dcb31c2a854c79e83cd62d6b55939f6c351a`。
- [帧率测试脚本](tools/test-uvc-fps.ps1)：调用本机 FFmpeg/DirectShow，逐档输出 JSON 测试结果。

## 已验证结果

| MJPEG 分辨率 | 30 fps 请求下的连续采集结果 | 结论 |
| --- | ---: | --- |
| 320×240 | 翻转修正版：600 帧 / 13.952 秒，43.00 fps | 达到至少 30 fps |
| 352×288 | 实验前稳定版：600 帧 / 26.895 秒，22.31 fps | 未达到 30 fps |
| 400×300 | 实验版：240 帧 / 11.519 秒，20.83 fps | 未达到 30 fps |
| 480×320 | 实验版：240 帧 / 11.423 秒，21.01 fps | 未达到 30 fps |

这些是采集程序实际收到的平均帧率，不是 USB 描述符中的标称值；不同场景、主机负载和采集软件会影响结果。当前已实测达到至少 30 fps 的最高原生分辨率为 **320×240**，不是声称的硬件绝对极限。高分辨率档仍可出画面，但不能当成 30 fps 使用。

## 构建与刷写

补丁针对指定的 OpenMV v4.7.0 提交。先在原版源码中应用补丁，再按 OpenMV 官方的 `OPENMV4` 构建环境准备交叉编译器和依赖对象；补丁增加了独立的 `uvc` 目标。构建结果为 `build/bin/uvc.bin`。本仓库不打包 OpenMV 整个上游源码、交叉编译器或 STM32 库。

```bash
git clone --branch v4.7.0 https://github.com/openmv/openmv.git
cd openmv
git apply ../openmv-h7-uvc-mjpeg/patches/openmv-v4.7.0-uvc-mjpeg.patch
make TARGET=OPENMV4
make TARGET=OPENMV4 uvc
```

上面是构建路径示意；OpenMV 的完整构建依赖及工具链设置以对应版本说明为准。`uvc` 目标依赖普通构建产生的部分目标文件，不能只在空目录直接运行 `make uvc`。最终固件的 SHA-256 见开发报告。

刷写仅适用于确认过的 **OpenMV Cam H7 / OPENMV4** 设备。先备份原固件并确认设备可以进入 OpenMV DFU；刷写错误型号或分区可能导致设备无法正常启动。实测使用 OpenMV DFU 设备 `37c5:9204` 的分区 `-a 2`，刷写工具等待期间需要重新插拔 USB：

```text
dfu-util -w -d ,37c5:9204 -a 2 -D openmv-h7-uvc-mjpeg-vflip.bin -R
```

正常运行后，Windows 摄像头名称为 `OpenMV UVC in FS Mode`。早期的第三方/旧版 UVC 固件曾导致设备不能正常枚举；请不要仅凭文件名中的 `OPENMV4` 就认为任何旧固件都兼容。

## 上游与许可

补丁基于 [OpenMV 项目](https://github.com/openmv/openmv) v4.7.0 的现有 UVC、传感器与图像处理代码，不代表 OpenMV 官方发布。上游文件中的版权与许可声明仍以原项目为准。固件是该代码及其构建依赖的派生二进制，请遵守上游各组件的许可证。
