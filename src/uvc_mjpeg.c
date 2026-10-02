/*
 * This file mirrors modifications applied to OpenMV v4.7.0.
 * The authoritative integration method remains
 * patches/openmv-v4.7.0-uvc-mjpeg.patch.
 * It is a focused reading extract, not a standalone build unit.
 * Original location: ports/stm32/uvc/src/main.c
 */

/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * main function.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "fb_alloc.h"
#include "imlib.h"
#include "usbd_uvc.h"
#include "usbd_uvc_if.h"

/* packet, uvc_header and stream state are owned by upstream main.c. */

bool process_frame(image_t *image)
{
    if (image == NULL || image->pixels == NULL || image->w <= 0 || image->h <= 0) {
        return false;
    }

    // Keep two CSI frame buffers and leave enough framebuffer RAM for the
    // JPEG bitstream plus the hardware encoder's MCU row scratch buffer.
    if (fb_avail() < (MJPEG_MAX_FRAME_SIZE + 32768U)) {
        return false;
    }

    fb_alloc_mark();
    image_t jpeg = {
        .w = image->w,
        .h = image->h,
        .pixfmt = PIXFORMAT_JPEG,
        .size = MJPEG_MAX_FRAME_SIZE,
        .pixels = fb_alloc(MJPEG_MAX_FRAME_SIZE, FB_ALLOC_CACHE_ALIGN),
    };
    bool overflow = jpeg_compress(image, &jpeg, 45, false, JPEG_SUBSAMPLING_422);
    if (overflow || jpeg.size < 4 || jpeg.size > MJPEG_MAX_FRAME_SIZE) {
        fb_alloc_free_till_mark();
        return false;
    }

    for (uint32_t offset = 0; offset < jpeg.size; ) {
        uint32_t chunk = IM_MIN(packet_size, jpeg.size - offset);
        packet[0] = uvc_header[0];
        packet[1] = uvc_header[1];
        memcpy(packet + 2, jpeg.pixels + offset, chunk);
        offset += chunk;
        if (offset == jpeg.size) {
            packet[1] |= 0x2;    // Flag end of frame
            uvc_header[1] ^= 1;  // Toggle bit 0 for next new frame
        }
        while (UVC_Transmit_FS(packet, chunk + 2) != USBD_OK) {
            if (g_uvc_stream_status != 2) {
                fb_alloc_free_till_mark();
                return false;
            }
            __WFI();
        }
    }

    fb_alloc_free_till_mark();

    if (g_uvc_stream_status != 2 ||
            frame_index != videoCommitControl.bFrameIndex ||
            format_index != videoCommitControl.bFormatIndex) {
        return false;
    }

    return true;
}
