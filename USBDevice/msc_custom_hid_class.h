/**
  **************************************************************************
  * @file     msc_class.h
  * @brief    usb msc class file
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
#ifndef __MSC_CLASS_H
#define __MSC_CLASS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usb_std.h"
#include "usbd_core.h"

#define USBD_MSC_BULK_IN_EPT             0x81
#define USBD_MSC_BULK_OUT_EPT            0x01

#define USBD_MSC_IN_MAXPACKET_SIZE           0x40
#define USBD_MSC_OUT_MAXPACKET_SIZE          0x40

extern usbd_class_handler msc_custom_hid_class_handler;

///////////////////////////////////////////////////////////////////

#define USBD_CUSTOM_HID_IN_EPT                  0x82
#define USBD_CUSTOM_HID_OUT_EPT                 0x02

#define USBD_CUSTOM_IN_MAXPACKET_SIZE           0x40
#define USBD_CUSTOM_OUT_MAXPACKET_SIZE          0x40

usb_sts_type custom_hid_class_send_report(void *udev, uint8_t *report, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif //#ifndef __MSC_CLASS_H
