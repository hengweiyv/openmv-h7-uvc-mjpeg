# Source reading map

All extracts come from OpenMV v4.7.0 at `2206dcb31c2a854c79e83cd62d6b55939f6c351a` after applying [`openmv-v4.7.0-uvc-mjpeg.patch`](../patches/openmv-v4.7.0-uvc-mjpeg.patch). They contain selected complete functions or definitions, plus file headers and reading context. They depend on declarations, state and supporting implementation in OpenMV and cannot form an independent firmware build.

| Reading file | Original upstream path | Selected implementation |
| --- | --- | --- |
| [`src/uvc_mjpeg.c`](../src/uvc_mjpeg.c) | `ports/stm32/uvc/src/main.c` | `process_frame()`: JPEG buffer allocation, compression, UVC packetization, EOF/FID flags and cleanup. |
| [`src/uvc_mjpeg_descriptors.c`](../src/uvc_mjpeg_descriptors.c) | `ports/stm32/uvc/src/usbd_uvc.c` | The active six-frame MJPEG descriptor initializer. The disabled legacy uncompressed initializer is omitted. |
| [`src/uvc_transport.c`](../src/uvc_transport.c) | `ports/stm32/uvc/src/usbd_uvc_if.c` | `UVC_Transmit_FS()`: submits the payload and waits for completion before the caller reuses its buffer. |
| [`include/uvc_mjpeg_descriptors.h`](../include/uvc_mjpeg_descriptors.h) | `ports/stm32/uvc/include/usbd_uvc.h` | MJPEG format and frame descriptor initializer macros. |
| [`include/uvc_mjpeg_types.h`](../include/uvc_mjpeg_types.h) | `ports/stm32/uvc/include/uvc.h` | Packed MJPEG format/frame structures and descriptor sizes. |

The patch is also needed to see the integration changes: sensor initialization and resolution selection; horizontal/vertical orientation; one or two CSI buffers according to resolution; Probe/Commit validation; stream start/stop; removal of thermal-camera extension units; additional CSI frame-size IDs; DMA IRQ enablement; USB FIFO sizing; and the Makefile target. See the [architecture patch map](ARCHITECTURE.md) for all eleven affected upstream files.

## Updating the extracts

1. Start with the exact upstream commit and apply the formal patch in that separate source tree.
2. Copy the selected complete functions/definitions into the files above. When a symbol occurs more than once, select the **active MJPEG** implementation, not an initializer inside `#if 0`.
3. Keep the original copyright and license notices and the authoritative-patch notice in each extract.
4. Compare the function/definition bodies against the patched upstream files. Only reading headers, includes, blank lines and trailing whitespace may differ.
5. Validate the patch with `git apply --check` on a clean upstream tree, check local Markdown links and run `git diff --check` before committing.

No code in `src/` or `include/` is part of the OpenMV build. Change the authoritative patch first when changing firmware behavior, then refresh these reading extracts.
