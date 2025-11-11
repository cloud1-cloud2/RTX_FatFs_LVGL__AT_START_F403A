/**
  **************************************************************************
  * @file     msc_desc.c
  * @brief    usb msc device descriptor
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
#include "stdio.h"
#include "usb_std.h"
#include "usbd_sdr.h"
#include "usbd_core.h"
#include "msc_custom_hid_desc.h"

/** @addtogroup AT32F403A_407_middlewares_usbd_class
  * @{
  */

/** @defgroup USB_msc_desc
  * @brief usb device msc descriptor
  * @{
  */

/** @defgroup USB_msc_desc_private_functions
  * @{
  */

static usbd_desc_t *get_device_descriptor(void);
static usbd_desc_t *get_device_qualifier(void);
static usbd_desc_t *get_device_configuration(void);
static usbd_desc_t *get_device_other_speed(void);
static usbd_desc_t *get_device_lang_id(void);
static usbd_desc_t *get_device_manufacturer_string(void);
static usbd_desc_t *get_device_product_string(void);
static usbd_desc_t *get_device_serial_string(void);
static usbd_desc_t *get_device_interface_string(void);
static usbd_desc_t *get_device_config_string(void);

static uint16_t usbd_unicode_convert(uint8_t *string, uint8_t *unicode_buf);
static void usbd_int_to_unicode (uint32_t value , uint8_t *pbuf , uint8_t len);
static void get_serial_num(void);

#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_usbd_desc_buffer[256] ALIGNED_TAIL;
static usbd_desc_t vp_desc;

/**
  * @brief device descriptor handler structure
  */
usbd_desc_handler msc_custom_hid_desc_handler =
{
  get_device_descriptor,
  get_device_qualifier,
  get_device_configuration,
  get_device_other_speed,
  get_device_lang_id,
  get_device_manufacturer_string,
  get_device_product_string,
  get_device_serial_string,
  get_device_interface_string,
  get_device_config_string,
};

/**
  * @brief usb device standard descriptor
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_usbd_descriptor[USB_DEVICE_DESC_LEN] ALIGNED_TAIL =
{
  USB_DEVICE_DESC_LEN,                   /* bLength */
  USB_DESCIPTOR_TYPE_DEVICE,             /* bDescriptorType */
  0x00,                                  /* bcdUSB */
  0x02,
  0xEF,                                  /* bDeviceClass */
  0x02,                                  /* bDeviceSubClass */
  0x01,                                  /* bDeviceProtocol */
  USB_MAX_EP0_SIZE,                      /* bMaxPacketSize */
  LBYTE(USBD_MSC_Custom_HID_VENDOR_ID),             /* idVendor */
  HBYTE(USBD_MSC_Custom_HID_VENDOR_ID),             /* idVendor */
  LBYTE(USBD_MSC_Custom_HID_PRODUCT_ID),            /* idProduct */
  HBYTE(USBD_MSC_Custom_HID_PRODUCT_ID),            /* idProduct */
  0x00,                                  /* bcdDevice rel. 1.00 */
  0x01,
  USB_MFC_STRING,                        /* Index of manufacturer string */
  USB_PRODUCT_STRING,                    /* Index of product string */
  USB_SERIAL_STRING,                     /* Index of serial number string */
  1                                      /* bNumConfigurations */
};

/**
  * @brief usb configuration standard descriptor
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_usbd_configuration[USBD_MSC_CUSTON_HID_CONFIG_DESC_SIZE] ALIGNED_TAIL =
{
  USB_DEVICE_CFG_DESC_LEN,               /* bLength: configuration descriptor size */
  USB_DESCIPTOR_TYPE_CONFIGURATION,      /* bDescriptorType: configuration */
  LBYTE(USBD_MSC_CUSTON_HID_CONFIG_DESC_SIZE),      /* wTotalLength: bytes returned */
  HBYTE(USBD_MSC_CUSTON_HID_CONFIG_DESC_SIZE),      /* wTotalLength: bytes returned */
  0x02,                                  /* bNumInterfaces: 2 interface */
  0x01,                                  /* bConfigurationValue: configuration value */
  0x04,                                  /* iConfiguration: index of string descriptor describing
                                            the configuration */
  0xC0,                                  /* bmAttributes: self powered */
  0x32,                                  /* MaxPower 100 mA: this current is used for detecting vbus */

  // 接口关联描述符(IAD) - 用于复合设备
  0x08,       // bLength: 8字节
  0x0B,       // bDescriptorType: 接口关联描述符
  0x00,       // bFirstInterface: 接口号0
  0x01,       // bInterfaceCount: 关联的接口数量
  0x08,       // bFunctionClass: MSC类
  0x06,       // bFunctionSubClass: SCSI透明命令集
  0x50,       // bFunctionProtocol: 批量传输协议
  0x00,       // iFunction: 功能字符串索引

  USB_DEVICE_IF_DESC_LEN,                /* bLength: interface descriptor size */
  USB_DESCIPTOR_TYPE_INTERFACE,          /* bDescriptorType: interface descriptor type */
  0x00,                                  /* bInterfaceNumber: number of interface */
  0x00,                                  /* bAlternateSetting: alternate set */
  0x02,                                  /* bNumEndpoints: number of endpoints */
  USB_CLASS_CODE_MSC,                    /* bInterfaceClass: msc class code */
  0x06,                                  /* bInterfaceSubClass: subclass code scsi */
  0x50,                                  /* bInterfaceProtocol: protocol code BBB */
  0x05,                                  /* iInterface: index of string descriptor */

  USB_DEVICE_EPT_LEN,                    /* bLength: size of endpoint descriptor in bytes */
  USB_DESCIPTOR_TYPE_ENDPOINT,           /* bDescriptorType: endpoint descriptor type */
  USBD_MSC_BULK_IN_EPT,                  /* bEndpointAddress: the address of endpoint on usb device described by this descriptor */
  USB_EPT_DESC_BULK,                     /* bmAttributes: endpoint attributes */
  LBYTE(USBD_MSC_IN_MAXPACKET_SIZE),
  HBYTE(USBD_MSC_IN_MAXPACKET_SIZE),         /* wMaxPacketSize: maximum packe size this endpoint */
  0x00,                                  /* bInterval: interval for polling endpoint for data transfers */

  USB_DEVICE_EPT_LEN,                    /* bLength: size of endpoint descriptor in bytes */
  USB_DESCIPTOR_TYPE_ENDPOINT,           /* bDescriptorType: endpoint descriptor type */
  USBD_MSC_BULK_OUT_EPT,                 /* bEndpointAddress: the address of endpoint on usb device described by this descriptor */
  USB_EPT_DESC_BULK,                     /* bmAttributes: endpoint attributes */
  LBYTE(USBD_MSC_OUT_MAXPACKET_SIZE),
  HBYTE(USBD_MSC_OUT_MAXPACKET_SIZE),        /* wMaxPacketSize: maximum packe size this endpoint */
  0x00,                                  /* bInterval: interval for polling endpoint for data transfers */

  // HID的IAD
  0x08,       // bLength: 8字节
  0x0B,       // bDescriptorType: 接口关联描述符
  0x01,       // bFirstInterface: 接口号1
  0x01,       // bInterfaceCount: 关联的接口数量(1)
  0x03,       // bFunctionClass: HID类
  0x00,       // bFunctionSubClass: 无引导
  0x00,       // bFunctionProtocol: 无
  0x00,       // iFunction: HID功能字符串索引

  // HID接口描述符
  USB_DEVICE_IF_DESC_LEN,                /* bLength: interface descriptor size */
  USB_DESCIPTOR_TYPE_INTERFACE,          /* bDescriptorType: interface descriptor type */
  0x01,                                  /* bInterfaceNumber: number of interface */
  0x00,                                  /* bAlternateSetting: alternate set */
  0x02,                                  /* bNumEndpoints: number of endpoints */
  USB_CLASS_CODE_HID,                    /* bInterfaceClass: class code hid */
  0x00,                                  /* bInterfaceSubClass: subclass code */
  0x00,                                  /* bInterfaceProtocol: protocol code */
  0x00,                                  /* iInterface: index of string descriptor */
  
  // HID描述符
  0x09,                                  /* bLength: size of HID descriptor in bytes */
  HID_CLASS_DESC_HID,                    /* bDescriptorType: HID descriptor type */
  LBYTE(CUSHID_BCD_NUM),
  HBYTE(CUSHID_BCD_NUM),                 /* bcdHID: HID class specification release number */
  0x00,                                  /* bCountryCode: hardware target conutry */
  0x01,                                  /* bNumDescriptors: number of HID class descriptor to follow */
  HID_CLASS_DESC_REPORT,                 /* bDescriptorType: report descriptor type */
  LBYTE(sizeof(g_usbd_custom_hid_report)),
  HBYTE(sizeof(g_usbd_custom_hid_report)), /* wDescriptorLength: total length of reprot descriptor */

  // HID端点描述符 - 中断输入端点
  USB_DEVICE_EPT_LEN,                    /* bLength: size of endpoint descriptor in bytes */
  USB_DESCIPTOR_TYPE_ENDPOINT,           /* bDescriptorType: endpoint descriptor type */
  USBD_CUSTOM_HID_IN_EPT,                /* bEndpointAddress: the address of endpoint on usb device described by this descriptor */
  USB_EPT_DESC_INTERRUPT,                /* bmAttributes: endpoint attributes */
  LBYTE(USBD_CUSTOM_IN_MAXPACKET_SIZE),
  HBYTE(USBD_CUSTOM_IN_MAXPACKET_SIZE),  /* wMaxPacketSize: maximum packe size this endpoint */
  CUSHID_BINTERVAL_TIME,                                  /* bInterval: interval for polling endpoint for data transfers */

  // HID端点描述符 - 中断输出端点
  USB_DEVICE_EPT_LEN,                    /* bLength: size of endpoint descriptor in bytes */
  USB_DESCIPTOR_TYPE_ENDPOINT,           /* bDescriptorType: endpoint descriptor type */
  USBD_CUSTOM_HID_OUT_EPT,               /* bEndpointAddress: the address of endpoint on usb device described by this descriptor */
  USB_EPT_DESC_INTERRUPT,                /* bmAttributes: endpoint attributes */
  LBYTE(USBD_CUSTOM_OUT_MAXPACKET_SIZE),
  HBYTE(USBD_CUSTOM_OUT_MAXPACKET_SIZE),  /* wMaxPacketSize: maximum packe size this endpoint */
  CUSHID_BINTERVAL_TIME,                    /* bInterval: interval for polling endpoint for data transfers */
};



/**
  * @brief usb hid report descriptor
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD uint8_t g_usbd_custom_hid_report[USBD_CUSHID_SIZ_REPORT_DESC] ALIGNED_TAIL =
{
		0x05, 0x01,       /* Usage Page (Generic Desktop) */ 

  0x09, 0x05,       /* Usage (Gamepad) */

  0xA1, 0x01,       /* Collection (Application) */ 

  0x85, 0x01,       /*  Report ID (1 / keystroke) */ 

 

   /* This HID report is used for Windows and non-Windows OS.*/ 

  /* Left Thumbstick */ 

  0x09, 0x01,       /* Usage (Pointer) */ 

  0xA1, 0x00,       /* Collection (Physical) */ 

  0x09, 0x30,       /* Usage (X) */ 

  0x09, 0x31,       /* Usage (Y) */ 

  0x15, 0x00,       /* Logical Min (0) */ 

  0x27, 0xFF, 0xFF, 0x00, 0x00, /* Logical Max (0xFFFF) */ 

  0x95, 0x02,       /* Report Count (2) */ 

  0x75, 0x10,       /* Report Size (16) */ 

  0x81, 0x02,       /* Input (Data,Var,Abs) */ 

  0xC0,                   /* End Collection (Thumbstick) */ 

 

  /* Right Thumbstick */  

  0x09, 0x01,       /* Usage (Pointer) */ 

  0xA1, 0x00,       /* Collection (Physical) */ 

  0x09, 0x32,       /* Usage (Z)  X and Y for Right thumbstick (16-bit) */ 

  0x09, 0x35,       /* Usage (Rz) */ 

  0x15, 0x00,       /* Logical Min (0) */ 

  0x27, 0xFF, 0xFF, 0x00, 0x00,  /* Logical Max (0xFFFF) */

  0x95, 0x02,       /* Report Count (2) */ 

  0x75, 0x10,       /* Report Size (16) */

  0x81, 0x02,       /* Input (Data,Var,Abs) */ 

  0xC0,                   /* End Collection (Thumbstick) */ 

   

  /* Left Trigger */  

  0x05, 0x02,       /* Usage Page (Simulation Controls) */ 

  0x09, 0xC5,       /* Usage (Brake) */ 

  0x15, 0x00,       /* Logical Min (0) */ 

  0x26, 0xFF, 0x03, /* Logical Max (0x3FF) */ 

  0x95, 0x01,       /* Report Count (1) */ 

  0x75, 0x0A,       /* Report Size (10) */ 

  0x81, 0x02,       /* Input (Data,Var,Abs) */ 

  /* Padding 6 bits */

  0x15, 0x00,       /* Logical Min (0) */ 

  0x25, 0x00,       /* Logical Max (0) */ 

  0x75, 0x06,       /* Report Size (6) */ 

  0x95, 0x01,       /* Report Count (1) */ 

  0x81, 0x03,       /* Input (Constant) */ 

   

  /* Right Trigger */  

  0x05, 0x02,       /* Usage Page (Simulation Controls) */ 

  0x09, 0xC4,       /* Usage (Accelerator)  */ 

  0x15, 0x00,       /* Logical Min (0) */ 

  0x26, 0xFF, 0x03, /* Logical Max (0x03FF) */ 

  0x95, 0x01,       /* Report Count (1) */ 

  0x75, 0x0A,       /* Report Size (10) */ 

  0x81, 0x02,       /* Input (Data,Var,Abs) */ 

  /* Padding 6 bits */ 

  0x15, 0x00,       /* Logical Min (0) */ 

  0x25, 0x00,       /* Logical Max (0) */ 

  0x75, 0x06,       /* Report Size (6) */ 

  0x95, 0x01,       /* Report Count (1) */ 

  0x81, 0x03,       /* Input (Constant) */ 

   

  /* Hat Switch(DPAD)  */ 

  0x05, 0x01,       /* Usage Page (Generic Desktop) */ 

  0x09, 0x39,       /* Usage (Hat Switch) DPAD  */ 

  0x15, 0x00,       /* Logical Min (1)   Windows XInput requires 0 for center position. */ 

  0x25, 0x07,       /* Logical Max (8)   4 bits for 8 pressed position in order of "Right  Left  Down  Up" */ 

                                 /* Logical Min (0)   Some partners use 8 for center based on Angular units.  */ 

                                 /* Logical Max (7)   0 - 7 for 8 pressed position based on angular 45 degree for each step.  */ 

  0x35, 0x00,       /* Physical Min (0)  Logical values from 0 to 10    ===  bit0   bit2  bit1  bit0   */ 

  0x46, 0x3B, 0x01, /* Physical Max (315) */ 

  0x66, 0x14, 0x00, /* Unit (English Rotation: Angular Position [degrees]) */ 

  0x75, 0x04,       /* Report Size (4)  --- 4 buttons */ 

  0x95, 0x01,       /* Report Count (1) */ 

  0x81, 0x42,       /* Input (Variable, Null) (DPAD) */ 

  /* Padding 4 bits  */ 

  0x75, 0x04,       /* Report Size (4) */ 

  0x95, 0x01,       /* Report Count (1) */ 

  0x15, 0x00,       /* Logical Min (0) */ 

  0x25, 0x00,       /* Logical Max (0) */ 

  0x35, 0x00,       /* Undefine Physical Min/Max (0) */ 

  0x45, 0x00,       /* Undefine Physical Min/Max (0) */ 

  0x65, 0x00,       /* Undefine Unit (None) */ 

  0x81, 0x03,       /* Input (Constant) */ 

   

  /* 15 Digital Buttons: A, B, X, Y, LShd(LeftShoulder)/LB(Left bumper), RShd/RB, */ 

  /*  View/BACK, Menu/START, LSB(Left stick button)/(Thumbstick Pressed), RSB, LThStk,  RThStk*/ 

  0x05, 0x09,       /* Usage Page (Button) */ 

  0x19, 0x01,       /* Usage Min (Button 1)  */ 

  0x29, 0x0F,       /* Usage Max (Button 15) */ 

  0x15, 0x00,       /* Logical Min (0) */ 

  0x25, 0x01,       /* Logical Max (1) */ 

  0x75, 0x01,       /* Report Size (1) */ 

  0x95, 0x0F,       /* Report Count (15) */ 

  0x81, 0x02,       /* Input (Data,Var,Abs) */ 

  /* Padding 1 bits for D-Pad, Leftshoulder, RightShoulder */ 

  0x15, 0x00,       /* Logical Min (0) */ 

  0x25, 0x00,       /* Logical Max (0) */ 

  0x75, 0x01,       /* Report Size (1) */ 

  0x95, 0x01,       /* Report Count (1) */ 

  0x81, 0x03,       /* Input (constant) */ 

  /* Share button */ 

  0x05, 0x0C,       /* Usage Page(Consumer) */ 

  0x0A, 0xB2, 0x00, /* Usage(Record) -- Share button */ 

  0x15, 0x00,       /* Logical Min (0) */ 

  0x25, 0x01,       /* Logical Max (1) */ 

  0x95, 0x01,       /* Report Count (1) */ 

  0x75, 0x01,       /* Report Size (1) */ 

  0x81, 0x02,       /* Input (Data,Var,Abs) */ 

  /* Padding 7 bits */ 

  0x15, 0x00,       /* Logical Min (0) */ 

  0x25, 0x00,       /* Logical Max (0) */ 

  0x75, 0x07,       /* Report Size (7) */ 

  0x95, 0x01,       /* Report Count (1) */ 

  0x81, 0x03,       /* Input (Constant) */ 
	
	/*********** Report#2 (Output): Xbox One Gamepad Rumble Motor ************/   

  0x05, 0x0F,  /* Usage Page (Physical Interface) */  

  0x09, 0x21,  /* Usage (Set Effect Report) */  

  0x85, 0x02,    /*Report ID (2) */ 

 

  0xA1, 0x02,       /* Collection (Logical) */  

  0x09, 0x97,       /* Usage (DC enable actuators) */  

  0x15, 0x00,       /* Logical Min (0) */  

  0x25, 0x01,       /* Logical Max (1) */  

  0x75, 0x04,       /* Report Size (4) */  

  0x95, 0x01,       /* Report Count(1) */  

  0x91, 0x02,       /* Output (Data,Var,Abs) */  

  /* Padding 4 bits */  

  0x15, 0x00,       /* Logical Min (0) */  

  0x25, 0x00,       /* Logical Max (0) */  

  0x75, 0x04,       /* Report Size (4) */  

  0x95, 0x01,       /* Report Count (1) */  

  0x91, 0x03,       /* Output (Constant) */  

   

  0x09, 0x70,       /* Usage (Magnitude in percentage) // Left Impulse, Right Impulse, Left Rumble, Right Rumble */  

  0x15, 0x00,       /* Logical Min (0) */  

  0x25, 0x64,       /* Logical Max (100) */  

  0x75, 0x08,       /* Report Size (8) */  

  0x95, 0x04,       /* Report Count (4) */  

  0x91, 0x02,       /* Output (Data,Var,Abs) */  

   

  0x09, 0x50,       /* Usage (Duration) On time duration, in steps of 10ms. */  

  0x66, 0x01, 0x10, /* Unit time seconds */  

  0x55, 0x0E,       /* Unit Exponent -2 for 10mS */  

  0x15, 0x00,       /* Logical Min (0) */  

  0x26, 0xFF, 0x00, /* Logical Max (255)  */  

  0x75, 0x08,       /* Report Size (8) */  

  0x95, 0x01,       /* Report Count (1)  */  

  0x91, 0x02,       /* Output (Data,Var,Abs) */  

   

  0x09, 0xA7,       /* Usage (Start Delay) On time duration, in steps of 10ms. */  

  0x15, 0x00,       /* Logical Min (0) */  

  0x26, 0xFF, 0x00, /* Logical Max (255)   */  

  0x75, 0x08,       /* Report Size (8) */  

  0x95, 0x01,       /* Report Count (1)  */  

  0x91, 0x02,       /* Output (Data,Var,Abs)  */  

  0x65, 0x00,       /* Undefine Unit (None) */  

  0x55, 0x00,       /* Undefine Exponent (None) */  

   

  0x09, 0x7C,       /* Usage (Loop Count)  Number of times to repeat.  0 = run once. */  

  0x15, 0x00,       /* Logical Min (0) */  

  0x26, 0xFF, 0x00, /* Logical Max (255)  */  

  0x75, 0x08,       /* Report Size (8) */  

  0x95, 0x01,       /* Report Count (1) */  

  0x91, 0x02,       /* Output (Data,Var,Abs)  */  

  0xC0, /* End Collection (Rumble)  */ 

  0xC0, /* End Collection (HID)  */ 
};


/**
  * @brief usb hid descriptor
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD uint8_t g_custom_hid_usb_desc[9] ALIGNED_TAIL =
{
  0x09,                                  /* bLength: size of HID descriptor in bytes */
  HID_CLASS_DESC_HID,                    /* bDescriptorType: HID descriptor type */
  LBYTE(CUSHID_BCD_NUM),
  HBYTE(CUSHID_BCD_NUM),                    /* bcdHID: HID class specification release number */
  0x00,                                  /* bCountryCode: hardware target conutry */
  0x01,                                  /* bNumDescriptors: number of HID class descriptor to follow */
  HID_CLASS_DESC_REPORT,                 /* bDescriptorType: report descriptor type */
  LBYTE(sizeof(g_usbd_custom_hid_report)),
  HBYTE(sizeof(g_usbd_custom_hid_report)),      /* wDescriptorLength: total length of reprot descriptor */
};



/**
  * @brief usb string lang id
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_string_lang_id[USBD_MSC_CUSTOM_HID_SIZ_STRING_LANGID] ALIGNED_TAIL =
{
  USBD_MSC_CUSTOM_HID_SIZ_STRING_LANGID,
  USB_DESCIPTOR_TYPE_STRING,
  0x09,
  0x04,
};

/**
  * @brief usb string serial
  */
#if defined ( __ICCARM__ ) /* iar compiler */
  #pragma data_alignment=4
#endif
ALIGNED_HEAD static uint8_t g_string_serial[USBD_MSC_CUSTOM_HID_SIZ_STRING_SERIAL] ALIGNED_TAIL =
{
  USBD_MSC_CUSTOM_HID_SIZ_STRING_SERIAL,
  USB_DESCIPTOR_TYPE_STRING,
};


/* device descriptor */
static usbd_desc_t device_descriptor =
{
  USB_DEVICE_DESC_LEN,
  g_usbd_descriptor
};

/* config descriptor */
static usbd_desc_t config_descriptor =
{
  USBD_MSC_CUSTON_HID_CONFIG_DESC_SIZE,
  g_usbd_configuration
};

/* langid descriptor */
static usbd_desc_t langid_descriptor =
{
  USBD_MSC_CUSTOM_HID_SIZ_STRING_LANGID,
  g_string_lang_id
};

/* serial descriptor */
static usbd_desc_t serial_descriptor =
{
  USBD_MSC_CUSTOM_HID_SIZ_STRING_SERIAL,
  g_string_serial
};


/**
  * @brief  standard usb unicode convert
  * @param  string: source string
  * @param  unicode_buf: unicode buffer
  * @retval length
  */
static uint16_t usbd_unicode_convert(uint8_t *string, uint8_t *unicode_buf)
{
  uint16_t str_len = 0, id_pos = 2;
  uint8_t *tmp_str = string;

  while(*tmp_str != '\0')
  {
    str_len ++;
    unicode_buf[id_pos ++] = *tmp_str ++;
    unicode_buf[id_pos ++] = 0x00;
  }

  str_len = str_len * 2 + 2;
  unicode_buf[0] = (uint8_t)str_len;
  unicode_buf[1] = USB_DESCIPTOR_TYPE_STRING;

  return str_len;
}

/**
  * @brief  usb int convert to unicode
  * @param  value: int value
  * @param  pbus: unicode buffer
  * @param  len: length
  * @retval none
  */
static void usbd_int_to_unicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;

  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2 * idx] = (value >> 28) + '0';
  }
  else
  {
      pbuf[2 * idx] = (value >> 28) + 'A' - 10;
    }

    value = value << 4;

    pbuf[2 * idx + 1] = 0;
  }
}

/**
  * @brief  usb get serial number
  * @param  none
  * @retval none
  */
static void get_serial_num(void)
{
  uint32_t serial0, serial1, serial2;

  serial0 = *(uint32_t*)MCU_ID1;
  serial1 = *(uint32_t*)MCU_ID2;
  serial2 = *(uint32_t*)MCU_ID3;

  serial0 += serial2;

  if (serial0 != 0)
  {
    usbd_int_to_unicode (serial0, &g_string_serial[2] ,8);
    usbd_int_to_unicode (serial1, &g_string_serial[18] ,4);
  }
}

/**
  * @brief  get device descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_descriptor(void)
{
  return &device_descriptor;
}

/**
  * @brief  get device qualifier
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t * get_device_qualifier(void)
{
  return NULL;
}

/**
  * @brief  get config descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_configuration(void)
{
  return &config_descriptor;
}

/**
  * @brief  get other speed descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_other_speed(void)
{
  return NULL;
}

/**
  * @brief  get lang id descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_lang_id(void)
{
  return &langid_descriptor;
}


/**
  * @brief  get manufacturer descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_manufacturer_string(void)
{
  vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_MSC_CUSTOM_HID_DESC_MANUFACTURER_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}

/**
  * @brief  get product descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_product_string(void)
{
  vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_MSC_CUSTOM_HID_DESC_PRODUCT_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}

/**
  * @brief  get serial descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_serial_string(void)
{
  get_serial_num();
  return &serial_descriptor;
}

/**
  * @brief  get interface descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_interface_string(void)
{
  vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_MSC_CUSTOM_HID_DESC_INTERFACE_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}

/**
  * @brief  get device config descriptor
  * @param  none
  * @retval usbd_desc
  */
static usbd_desc_t *get_device_config_string(void)
{
  vp_desc.length = usbd_unicode_convert((uint8_t *)USBD_MSC_CUSTOM_HID_DESC_CONFIGURATION_STRING, g_usbd_desc_buffer);
  vp_desc.descriptor = g_usbd_desc_buffer;
  return &vp_desc;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
