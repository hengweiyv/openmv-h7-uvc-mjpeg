/*
 * This file mirrors modifications applied to OpenMV v4.7.0.
 * The authoritative integration method remains
 * patches/openmv-v4.7.0-uvc-mjpeg.patch.
 * It is a focused reading extract, not a standalone build unit.
 * Original location: ports/stm32/uvc/include/uvc.h
 */

struct uvc_format_mjpeg {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bDescriptorSubType;
	uint8_t bFormatIndex;
	uint8_t bNumFrameDescriptors;
	uint8_t bmFlags;
	uint8_t bDefaultFrameIndex;
	uint8_t bAspectRatioX;
	uint8_t bAspectRatioY;
	uint8_t bmInterlaceFlags;
	uint8_t bCopyProtect;
} __attribute__((packed));

#define UVC_DT_FORMAT_MJPEG_SIZE 11
#define UVC_DT_FRAME_MJPEG_SIZE(n) (26 + 4 * (n))

#define UVC_FRAME_MJPEG(n) uvc_frame_mjpeg_##n
#define DECLARE_UVC_FRAME_MJPEG(n) \
struct UVC_FRAME_MJPEG(n) { \
	uint8_t bLength; \
	uint8_t bDescriptorType; \
	uint8_t bDescriptorSubType; \
	uint8_t bFrameIndex; \
	uint8_t bmCapabilities; \
	uint16_t wWidth; \
	uint16_t wHeight; \
	uint32_t dwMinBitRate; \
	uint32_t dwMaxBitRate; \
	uint32_t dwMaxVideoFrameBufferSize; \
	uint32_t dwDefaultFrameInterval; \
	uint8_t bFrameIntervalType; \
	uint32_t dwFrameInterval[n]; \
} __attribute__((packed))


#define UVC_FRAMES_FORMAT_MJPEG(n) uvc_vs_mjpeg_desc_##n
#define DECLARE_UVC_FRAMES_FORMAT_MJPEG(n) \
struct UVC_FRAMES_FORMAT_MJPEG(n) { \
	struct uvc_format_mjpeg uvc_vs_format; \
	struct UVC_FRAME_MJPEG(1) uvc_vs_frame[n]; \
	struct uvc_color_matching_descriptor uvc_vs_color; \
} __attribute__((packed))
