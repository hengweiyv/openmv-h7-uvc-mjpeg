/*
 * This file mirrors modifications applied to OpenMV v4.7.0.
 * The authoritative integration method remains
 * patches/openmv-v4.7.0-uvc-mjpeg.patch.
 * It is a focused reading extract, not a standalone build unit.
 * Original location: ports/stm32/uvc/src/usbd_uvc.c
 */

/**
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "usbd_uvc.h"
#include "uvc_desc.h"

static struct uvc_vs_frames_formats_descriptor uvc_vs_frames_formats_desc = {
    .uvc_vs_frames_format_1 = {
        .uvc_vs_format = UVC_FORMAT_MJPEG_DESCRIPTOR(6),
        .uvc_vs_frame = {
            UVC_FRAME_MJPEG_FORMAT(VS_FRAME_INDEX_1, 160, 120),
            UVC_FRAME_MJPEG_FORMAT(VS_FRAME_INDEX_2, 240, 160),
            UVC_FRAME_MJPEG_FORMAT(VS_FRAME_INDEX_3, 320, 240),
            UVC_FRAME_MJPEG_FORMAT(VS_FRAME_INDEX_4, 352, 288),
            UVC_FRAME_MJPEG_FORMAT(VS_FRAME_INDEX_5, 400, 300),
            UVC_FRAME_MJPEG_FORMAT(VS_FRAME_INDEX_6, 480, 320),
        },
        .uvc_vs_color = UVC_COLOR_MATCHING_DESCRIPTOR(),
    },
};
