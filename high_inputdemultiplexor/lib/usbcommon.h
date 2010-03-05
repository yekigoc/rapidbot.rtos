#ifndef __USBCOMMON_H__
#define __USBCOMMON_H__

#define CTRL_IN			(0x01 | LIBUSB_ENDPOINT_IN)
#define CTRL_OUT		(0x01 | LIBUSB_ENDPOINT_OUT)
#define USB_RQ_STAT			0x0
#include <libusb.h>

#define PER_PWM                 0x1
#define PER_ADC                 0x2
#define PER_GETADC              0x3
#define PER_TELLADC             0x4

#endif// __USBCOMMON_H__
