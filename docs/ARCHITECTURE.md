# Architecture

This project targets the **OpenMV Cam H7** (`OPENMV4`) and the STM32 USB **Full Speed** device controller. The authoritative implementation is the patch against OpenMV v4.7.0; [`src/`](../src/) and [`include/`](../include/) contain focused reading extracts from that patch.

```text
OpenMV image sensor
        ↓  YUV422 capture / CSI + DMA
Framebuffer
        ↓  STM32 hardware JPEG encoder, quality 45
MJPEG frame (up to 64 KiB)
        ↓  2-byte UVC payload header + frame ID / end-of-frame flags
USB FS isochronous IN endpoint (EP1, 1022-byte packet)
        ↓
Host UVC driver (Windows: usbvideo.sys)
        ↓
Camera app / OBS / browser / video-call software
```

The patched `Makefile` adds a separate `uvc` target that links OpenMV's existing CSI, framebuffer, image, HAL, sensor and USB sources. It produces `build/bin/uvc.bin`, rather than compiling this repository's reading extracts. The sensor frame is compressed through `jpeg_compress()`, then sent in chunks of at most 1020 JPEG bytes plus the two-byte UVC header. The end-of-frame bit is set on the final packet; the frame ID bit toggles for the next frame. Probe/Commit control and MJPEG descriptors expose six frame sizes.

The descriptor sets `CAM_FPS` to 30 and offers one frame interval. That is the **advertised** interval, not a guarantee of delivered frames. At Full Speed, the nominal payload ceiling from 1020 bytes per 1 ms USB frame is about 1.02 MB/s before other overhead. JPEG size depends on the scene; 30 fps needs about 34 kB or less per frame at that ceiling. Capture, compression, buffer memory and host behavior also affect throughput. Measurements in the README are host observations, including values above the advertised 30 fps, and are not USB timing guarantees.

The current patch hard-codes Full Speed (`USE_USB_FS=1`), MJPEG quality 45, vertical and horizontal mirroring, and one MJPEG format. It does not implement USB High Speed or Windows Hello face authentication.

## Patch map

| Upstream path | Project change |
| --- | --- |
| `Makefile` | Restores a separate native UVC build target. |
| `ports/stm32/uvc/src/main.c` | JPEG compression, payload packetization, sensor size selection, buffer policy, image orientation. |
| `ports/stm32/uvc/include/uvc.h`, `usbd_uvc.h`, `uvc_desc.h` | MJPEG descriptor types, format, frame and packet limits. |
| `ports/stm32/uvc/src/usbd_uvc.c`, `usbd_uvc_if.c` | Six-frame descriptor, stream state, Probe/Commit and transfer completion. |
| `common/omv_csi.c`, `omv_csi.h` | Additional sensor frame-size IDs, including 400×300. |
| `ports/stm32/uvc/src/stm32_it.c`, `usbd_conf.c` | DMA IRQ enablement and USB FIFO size. |

All eleven files already exist upstream. The patch adds no new OpenMV source file. The focused files here retain only directly relevant C definitions/functions and cannot be built in isolation.
