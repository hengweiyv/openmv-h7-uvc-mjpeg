/*
 * This file mirrors modifications applied to OpenMV v4.7.0.
 * The authoritative integration method remains
 * patches/openmv-v4.7.0-uvc-mjpeg.patch.
 * It is a focused reading extract, not a standalone build unit.
 * Original location: ports/stm32/uvc/src/usbd_uvc_if.c
 */

/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @brief          :
  ******************************************************************************
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice,
  * this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/

#include "usbd_uvc.h"
#include "usbd_uvc_if.h"

uint8_t UVC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
  USBD_UVC_SetTxBuffer(hUsbDevice_0, Buf, Len);
  result = USBD_UVC_TransmitPacket(hUsbDevice_0);
  HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
  if (result == USBD_OK) {
    USBD_UVC_HandleTypeDef *state = (USBD_UVC_HandleTypeDef *) hUsbDevice_0->pClassData;
    while (state->TxState != 0 && g_uvc_stream_status == 2) {
      __WFI();
    }
    if (g_uvc_stream_status != 2) {
      return USBD_FAIL;
    }
  }
  return result;
}
