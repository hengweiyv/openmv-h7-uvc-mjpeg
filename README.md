# OpenMV Cam H7 原生 USB 摄像头（MJPEG）

这是我把 OpenMV Cam H7（固件目标 `OPENMV4`）改造成 Windows 原生 UVC 摄像头的开发记录。设备通过 Windows 自带的 `usbvideo.sys` 驱动识别，无需 OBS 虚拟摄像头或电脑端转发程序。

- [完整开发报告](DEVELOPMENT_REPORT.md)：需求、我的工作、调试经过、故障与解决、实测数据和局限。
- [最新固件 v1.1.0](firmware/openmv-h7-uvc-mjpeg-vflip-hmirror.bin)：已在实机刷入验证，MJPEG、上下翻转与左右镜像已启用；[v1.0.0 旧版](firmware/openmv-h7-uvc-mjpeg-vflip.bin)仍保留供回退。
- [源码补丁](patches/openmv-v4.7.0-uvc-mjpeg.patch)：基于 [OpenMV v4.7.0](https://github.com/openmv/openmv/tree/v4.7.0)，基础提交 `2206dcb31c2a854c79e83cd62d6b55939f6c351a`。
- [帧率测试脚本](tools/test-uvc-fps.ps1)：调用本机 FFmpeg/DirectShow，逐档输出 JSON 测试结果。

## 已验证结果

| MJPEG 分辨率 | 30 fps 请求下的连续采集结果 | 结论 |
| --- | ---: | --- |
| 320×240 | v1.1.0：600 帧 / 13.946 秒，43.02 fps | 达到至少 30 fps |
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
dfu-util -w -d ,37c5:9204 -a 2 -D openmv-h7-uvc-mjpeg-vflip-hmirror.bin -R
```

正常运行后，Windows 摄像头名称为 `OpenMV UVC in FS Mode`。早期的第三方/旧版 UVC 固件曾导致设备不能正常枚举；请不要仅凭文件名中的 `OPENMV4` 就认为任何旧固件都兼容。

## Windows Hello 人脸登录

当前固件是普通彩色 MJPEG UVC 摄像头，**不支持 Windows Hello 人脸验证**。OpenMV H7 有 850 nm 红外补光 LED，但红外补光灯不等于独立、符合要求的红外视频流。微软的 [Windows Hello 人脸认证说明](https://learn.microsoft.com/en-us/windows-hardware/design/device-experiences/windows-hello-face-authentication)要求专门配置的近红外成像；[UVC 实现指南](https://learn.microsoft.com/en-us/windows-hardware/drivers/stream/uvc-camera-implementation-guide)还说明了 RGB/IR 流、Face Auth Profile V2 和分辨率/帧率门槛。当前设备只输出一条彩色流，最大已发布高度为 320 像素，不能仅靠修改 USB 名称或标志安全地充当 Hello 设备。理论上需要额外的红外成像与相应 UVC/认证开发，本项目没有实现或验证这些条件。

## 上游与许可

补丁基于 [OpenMV 项目](https://github.com/openmv/openmv) v4.7.0 的现有 UVC、传感器与图像处理代码，不代表 OpenMV 官方发布。上游文件中的版权与许可声明仍以原项目为准。固件是该代码及其构建依赖的派生二进制，请遵守上游各组件的许可证。
