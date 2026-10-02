/*
 * This file mirrors modifications applied to OpenMV v4.7.0.
 * The authoritative integration method remains
 * patches/openmv-v4.7.0-uvc-mjpeg.patch.
 * It is a focused reading extract, not a standalone build unit.
 * Original location: ports/stm32/uvc/include/usbd_uvc.h
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



#define UVC_FORMAT_MJPEG_DESCRIPTOR(NUM_FRAME_DESCS) { \
  .bLength = UVC_DT_FORMAT_MJPEG_SIZE, \
  .bDescriptorType = UVC_CS_INTERFACE, \
  .bDescriptorSubType = UVC_VS_FORMAT_MJPEG, \
  .bFormatIndex = VS_FMT_INDEX(MJPEG), \
  .bNumFrameDescriptors = NUM_FRAME_DESCS, \
  .bmFlags = 0x00, /* JPEG size varies with scene content. */ \
  .bDefaultFrameIndex = VS_FRAME_INDEX_1, \
  .bAspectRatioX = 0, \
  .bAspectRatioY = 0, \
  .bmInterlaceFlags = 0, \
  .bCopyProtect = 0, \
}

#define UVC_FRAME_MJPEG_FORMAT(FRAME_INDEX, WIDTH, HEIGHT) { \
  .bLength = UVC_DT_FRAME_MJPEG_SIZE(1), \
  .bDescriptorType = UVC_CS_INTERFACE, \
  .bDescriptorSubType = UVC_VS_FRAME_MJPEG, \
  .bFrameIndex = FRAME_INDEX, \
  .bmCapabilities = 0x02, \
  .wWidth = WIDTH, \
  .wHeight = HEIGHT, \
  .dwMinBitRate = 8U * 1024U * CAM_FPS, \
  .dwMaxBitRate = 8U * MJPEG_MAX_FRAME_SIZE * CAM_FPS, \
  .dwMaxVideoFrameBufferSize = MJPEG_MAX_FRAME_SIZE, \
  .dwDefaultFrameInterval = INTERVAL, \
  .bFrameIntervalType = 1, \
  .dwFrameInterval = { INTERVAL }, \
}
