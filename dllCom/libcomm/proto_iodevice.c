#include <stdint.h>
#include <stddef.h>

#include "proto_iodevice.h"
#include "devices/device_emulslave.h"
#include "devices/device_serial.h"
#include "devices/device_libusb.h"
#include "devices/device_proxy.h"
#include "devices/device_usbdev.h"

#if defined(STM32F429xx)
#include "devices/device_stm32.h"
#endif

proto_Device_t device_create(proto_iodev_devices_t type)
{
	proto_Device_t dev = NULL;
	switch (type)
	{
	case PROTO_DEV_EMULSLAVE:
		dev = devemulslave_create();
		break;
	case PROTO_DEV_SERIAL:
		dev = devserial_create();
		break;
	case PROTO_DEV_USBDEV:
		dev = devusbdev_create();
		break;
	case PROTO_DEV_LIBUSB:
		dev = devlibusb_create();
		break;
	case PROTO_DEV_PROXY:
		dev = devproxy_create();
		break;
	case PROTO_DEV_STM32:
#if defined(STM32F429xx)
		dev = devstm32_create();
#endif
		break;
	default:
		break;
	}
	return dev;
}


