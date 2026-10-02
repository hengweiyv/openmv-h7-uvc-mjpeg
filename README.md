# openmv-h7-uvc-mjpeg

**OpenMV Cam H7 / STM32 UVC MJPEG firmware project.** It turns an OpenMV Cam H7 (`OPENMV4`) into a native USB UVC MJPEG webcam for systems with UVC camera support, including Windows, Linux and macOS. Physical flashing and the capture results below were tested on Windows; Linux and macOS have not been separately tested.

The project targets [OpenMV v4.7.0](https://github.com/openmv/openmv/tree/v4.7.0), commit `2206dcb31c2a854c79e83cd62d6b55939f6c351a`. It does not copy the complete OpenMV tree. **[`patches/openmv-v4.7.0-uvc-mjpeg.patch`](patches/openmv-v4.7.0-uvc-mjpeg.patch) is the authoritative integration.** [`src/`](src/) and [`include/`](include/) contain focused C extracts for reading, IDE navigation and GitHub language statistics. They cannot be compiled alone and are not build inputs.

## Features and observed results

- MJPEG over USB UVC **Full Speed**, using OpenMV's image sensor, framebuffer and hardware JPEG encoder.
- Six advertised MJPEG sizes: 160×120, 240×160, 320×240, 352×288, 400×300 and 480×320. The UVC descriptor advertises one 30 fps interval for each.
- Windows enumerated the tested firmware as **OpenMV UVC in FS Mode** through its built-in `usbvideo.sys` driver. Camera and OBS can select it.
- Current firmware applies vertical flip and horizontal mirror. The v1.1.0 binary was physically flashed and capture-tested.

Download [v1.1.0 firmware](firmware/openmv-h7-uvc-mjpeg-vflip-hmirror.bin), or the retained [v1.0.0 firmware](firmware/openmv-h7-uvc-mjpeg-vflip.bin). See [firmware versions and SHA-256 checksums](firmware/README.md) before flashing.

| Size | Observed host capture | Result at requested 30 fps |
| --- | --- | --- |
| 320×240 | v1.1.0: 600 frames / 13.946 s = 43.02 fps | Met or exceeded 30 fps in this test |
| 352×288 | Earlier stable build: 600 frames / 26.895 s = 22.31 fps | Below 30 fps |
| 400×300 | Experimental build: 240 frames / 11.519 s = 20.83 fps | Below 30 fps |
| 480×320 | Experimental build: 240 frames / 11.423 s = 21.01 fps | Below 30 fps |

These are host-side `frames / elapsed time` observations, not guaranteed USB delivery rates. **43.02 fps differs from the advertised 30 fps interval**; the cause has not been isolated. For tested ≥30 fps performance, choose **320×240**. See the [development report](DEVELOPMENT_REPORT.md) for experiments and limitations.

## Repository layout

| Path | Purpose |
| --- | --- |
| [`src/`](src/) and [`include/`](include/) | Focused functions and definitions mirrored from the applied patch; reading only. |
| [`patches/`](patches/) | Formal integration into the exact OpenMV v4.7.0 tree. |
| [`firmware/`](firmware/) | Prebuilt firmware; the older v1.0.0 binary remains for comparison. |
| [`tools/test-uvc-fps.ps1`](tools/test-uvc-fps.ps1) | Windows FFmpeg / DirectShow capture test. |
| [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) | Capture, JPEG, USB and host pipeline. |
| [`docs/SOURCE_MAP.md`](docs/SOURCE_MAP.md) | Where each reading extract comes from and how to keep it synchronized. |
| [`DEVELOPMENT_REPORT.md`](DEVELOPMENT_REPORT.md) | Development details and measured results. |

This repository has no GitHub Actions workflow or separate build script. The patch modifies **OpenMV's top-level `Makefile`**; that Makefile stays in the upstream tree.

## Build from source

Use a Linux or WSL build environment with OpenMV v4.7.0's prerequisites: Arm GNU tools (`arm-none-eabi-*`), Python 3, Make, the LLVM toolchain used by upstream, and its submodules. The v4.7.0 Makefile defaults `LLVM_PATH` to `/opt/LLVM-ET-Arm-18.1.3-Linux-x86_64/bin/`; set it to your installation when needed. The Windows capture test requires PowerShell 7 and FFmpeg with DirectShow support. Run the build commands from a parent directory containing this repository and check the exact upstream commit before applying the patch.

```bash
git clone --branch v4.7.0 --recursive https://github.com/openmv/openmv.git
cd openmv
git rev-parse HEAD  # expected: 2206dcb31c2a854c79e83cd62d6b55939f6c351a
git apply --check ../openmv-h7-uvc-mjpeg/patches/openmv-v4.7.0-uvc-mjpeg.patch
git apply ../openmv-h7-uvc-mjpeg/patches/openmv-v4.7.0-uvc-mjpeg.patch
make TARGET=OPENMV4 submodules
make TARGET=OPENMV4
make TARGET=OPENMV4 uvc
```

The patched `uvc` target produces `build/bin/uvc.bin` plus ELF/DFU artifacts in `build/bin/`. The regular `make TARGET=OPENMV4` prepares OpenMV objects used by that target. Apply and build **inside OpenMV**, never from `src/` or `include/` here. The patch applies cleanly to the specified upstream commit; a complete build still needs its toolchain and submodules.

## Flash and test

Confirm the board is **OpenMV Cam H7 / `OPENMV4`**, back up firmware or data you need, and confirm its DFU identity. A filename or `OPENMV4` directory is not sufficient proof of compatibility. The tested OpenMV DFU device is `37c5:9204`, alt setting 2. From this repository's root:

```bash
dfu-util -w -d ,37c5:9204 -a 2 -D firmware/openmv-h7-uvc-mjpeg-vflip-hmirror.bin -R
```

On Windows, reconnect the USB cable if DFU is waiting for re-enumeration. Check for **OpenMV UVC in FS Mode** in Camera, OBS or a DirectShow device list. With a local FFmpeg installation, run:

```powershell
./tools/test-uvc-fps.ps1 -Frames 600
```

The script requests MJPEG at 30 fps and emits one compact JSON result per tested size. Linux/macOS users can try a UVC-compatible camera app; this firmware has no reported capture test on those hosts.

## Known limits and roadmap

- USB is **Full Speed**, not High Speed. The tested 352×288 to 480×320 modes did not reach 30 fps.
- Only MJPEG is exposed. JPEG quality is fixed at 45 and frames have a 64 KiB maximum.
- This is a standard color webcam. It does **not** provide an IR stream or the face-authentication profile needed for Windows Hello.
- Next work: measure capture/JPEG/USB timing, reconcile host fps with the descriptor interval, improve larger-mode throughput, and test Linux/macOS hosts.

## License

The patch changes OpenMV source, so original upstream copyright notices and license terms remain relevant. This repository currently has **no top-level `LICENSE` file** and does not claim an independent license for project-owned additions or binaries. Consult [OpenMV's licensing information](https://github.com/openmv/openmv) before reuse; choose a repository-level license before accepting third-party contributions.
