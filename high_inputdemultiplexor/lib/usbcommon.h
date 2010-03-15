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
#define PER_TELLID              0x5
#define PER_SETLOCK             0x6
#define PER_SETUNLOCK           0x7
#define PER_TELLLOCKED          0x8
#define PER_TELLREADY           0x9
#define PER_GETTIME             0x10
#define PER_TELLTIME            0x11
#define PER_TIME                0x12
#define PER_GETBATCHADC         0x13
#define PER_TELLBATCHADC        0x14

#endif// __USBCOMMON_H__
