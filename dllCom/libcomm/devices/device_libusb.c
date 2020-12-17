/// Implement a libusb_fs device.
/// We're using the linux API to call bulk transfer
/// Incompatible file for iOS 

#include <stdlib.h>
#include <assert.h>
#include "proto_iodevice.h"
#include "device_libusb.h"
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "libusb.h"

#include "log.h"

/******************************************************************************
 * TYPES & VARIABLES
******************************************************************************/

/* User data */
typedef struct
{
	libusb_context *ctx; /// libusbd context
	char * szNodePath; /// path node : /dev/bus/usb/XXXX/YYYY
	libusb_device * dev; /// libusb device 
	libusb_device_handle * devh; /// libusb device handle
    int fd; ///  file descriptor from android hardware usb api
	int ep_in; //!< endpoint address for device to host communication
	int ep_out;  //!< endpoint address for host to device communication
	int max_pkt_size; /// TODO needed ?
} proto_dev_libusb_t ;

#define LIBUSB_BULK_XFER_TOUT_MS 200

#define DEVLIBUSB_FD_INVALID -1

/******************************************************************************
 * LOCAL PROTO
******************************************************************************/

///
/// \brief devlibusb_init : make the allocated structure initialization
/// \param _this Instance
/// \return operation result : 0 if ok otherwise error
static int devlibusb_init(proto_Device_t _this);

/// \brief display device info (bulk endpoints) using libusb
/// Used for device debug
/// TODO if this work, don't get anymore these info from the android lib
static void devlibusb_log_devInfo(libusb_device * dev);

/******************************************************************************
 * FUNCTION
******************************************************************************/

void devlibusb_destroy(proto_Device_t _this)
{
    assert(_this && _this->user);
	proto_dev_libusb_t* infos = _this->user;
	/// system ressource is managed outside dll com.
    /// Liberation des structures
	LOG("destroying ressources");

	// The contexte is one step befor open/close operation, it is kept alive until we free the device.
	if (infos->ctx)
	{
		libusb_exit(infos->ctx);
	}
    _this->close(_this);
    free(_this->user);
    free(_this);
}

static int devlibusb_close(struct proto_IfaceIODevice* _this)
{
    assert(_this);
	int ret = 0;

    proto_dev_libusb_t* infos = _this->user;
	
	// Just invalidate descriptor file, we don't own the ressources
	if (infos->fd >=0)
	{
		infos->fd = DEVLIBUSB_FD_INVALID;
	}

	return ret;
}

static int devlibusb_open(struct proto_IfaceIODevice* _this, const char * szPath)
{
    assert(_this && szPath);
	proto_dev_libusb_t* infos = _this->user;
	int ret = 0;

	// context must have been initialize at create.
	assert(infos->ctx);
	// We call the specific function for that wrap the system fd.
	// http://libusb.sourceforge.net/api-1.0/group__libusb__dev.html#ga98f783e115ceff4eaf88a60e6439563c
	ret = libusb_wrap_sys_device(infos->ctx, (intptr_t)infos->fd, &infos->devh);
	if (ret != LIBUSB_SUCCESS)
	{
		LOG("error : libusb_wrap_sys_device failed : %s", libusb_strerror(ret));
	}
	if (infos->devh == NULL)
	{
		LOG("error : libusb could not create device handle");
		return -1;
	}

	// I'm curious to see if I can retreive the internally created device
	infos->dev = libusb_get_device(infos->devh);
	if (infos->dev == NULL)
	{
		LOG("warning : libusb could not return device");
	}

	return 0;
}

static int devlibusb_read(struct proto_IfaceIODevice* _this, void* buffer, uint8_t len, int16_t tout_ms)
{
	assert(_this && buffer);
	proto_dev_libusb_t* infos = _this->user;
	int res = 0;
	int transferred = 0;


	if ((infos->fd < 0) || (len <= 0))
	{
		LOG("Error : arg invalid");
		return -1;
	}

	// we'll send all until an error occurs
	// FIXME - we'll always have a timeout error.
	res = libusb_bulk_transfer(infos->devh, infos->ep_in, buffer, len, &transferred, LIBUSB_BULK_XFER_TOUT_MS);
	switch (res)
	{
	case LIBUSB_SUCCESS:
		break;
	case LIBUSB_ERROR_TIMEOUT:
		// According to our protocol specification, we must return any data received successfully
		// We shall return an error if actually no data was returned;
		if (transferred == 0)
		{
			LOG("error : %s (fd=%d, endpoint=%d)", libusb_strerror(res), infos->fd, infos->ep_in);
			return -1;
		}
		break;
	default:
		LOG("error : %s (fd=%d, endpoint=%d)", libusb_strerror(res), infos->fd, infos->ep_in);
		return -1;
		break;
	}

	LOG("info - read end, expected %d, received : %d", len, transferred);
	// Display data because it seems not validated
	if (transferred > 0)
	{
		int i = 0, cursor=0;
		char szData[3*8 +1] = { 0 };	
		for (i=0, cursor=0; (i< transferred) && (i<8); i++)
		{
			snprintf(&szData[cursor], 4, "%02X ", ((uint8_t*)buffer)[i]);
			cursor += 3;
		}
		LOG("info data (8 first bytes) :%s", szData);
		LOG("strlen(szData)=%d", (int)strlen(szData));
	}
	
	return transferred;
}


static int devlibusb_write(struct proto_IfaceIODevice* _this, const void * buffer, uint8_t len)
{
	assert(_this && buffer);
	proto_dev_libusb_t* infos = _this->user;
	int res = 0;
	int actual_length = 0;
	uint8_t sent = 0;
	int nbretry = 2;
	if ((infos->fd < 0) || (len <= 0))
	{
		LOG("Error : arg invalid");
		return -1;
	}		

	// we'll send all until an error occurs
	do
	{
		res = libusb_bulk_transfer(infos->devh, infos->ep_out, &((uint8_t*)buffer)[sent], len - sent, &actual_length, LIBUSB_BULK_XFER_TOUT_MS);
		switch (res)
		{
		case LIBUSB_SUCCESS:
			sent += actual_length;
			break;
		case LIBUSB_ERROR_TIMEOUT:
			nbretry--;
			if (nbretry == 0)
			{
				LOG("error : %s (fd=%d, endpoint=%d)", libusb_strerror(res), infos->fd, infos->ep_out);
				return -1;
			}
			res = libusb_clear_halt(infos->devh, infos->ep_in);
			if (res != LIBUSB_SUCCESS)
			{
				LOG("error : %s (fd=%d, endpoint=%d)", libusb_strerror(res), infos->fd, infos->ep_in);
			}
			break;
		default:
			LOG("error : %s (fd=%d, endpoint=%d)", libusb_strerror(res), infos->fd, infos->ep_out);
			return -1;
			break;
		}		
	} while(sent < len);

	LOG("info - write end sent : %d", sent);
	return 0;
}


int devlibusb_setFD(proto_Device_t _this, int fd, int ep_in, int ep_out, int max_pkt_size)
{
    assert(_this);
	assert(infos->ctx || infos->devh);
	proto_dev_libusb_t* infos = _this->user;
	if(fd < 0)
		return -1;

	// if a  device already exist refuse to assign because caller must first open the newly device node.
	if( (infos->fd != DEVLIBUSB_FD_INVALID) || (infos->devh) )
	{
		LOG("error : you must first close the currently opened device");
		return -1;
	}
	// We just copy the parameters, the device handle shall be created in open().
	infos->fd = fd;
	infos->ep_in = ep_in;
	infos->ep_out = ep_out;
	infos->max_pkt_size = max_pkt_size;
 
    return 0;
}

int devlibusb_getFD(proto_Device_t _this)  
{
    assert(_this);
	proto_dev_libusb_t* infos = _this->user;
	
    return infos->fd;

}


proto_Device_t devlibusb_create(void)
{
    /// allocation des structures et initialisation du pointeur
    proto_Device_t _this  = (proto_Device_t)malloc( sizeof(proto_IfaceIODevice_t) );
    _this->user = (proto_dev_libusb_t*)malloc(sizeof(proto_dev_libusb_t));
    devlibusb_init(_this);

    return _this;
}


int devlibusb_init(proto_Device_t _this)
{
    assert(_this);
	DEVIO_INIT(devlibusb, _this); // Init interface
    proto_dev_libusb_t* infos = _this->user;
	int ret;

	// Initialize 'user' field to invalid state
    infos->fd		= -1;

	// Do specific libusb initialization
	// this option must be set before libusb_init
	// http://libusb.sourceforge.net/api-1.0/group__libusb__lib.html#ga07d4ec54cf575d672ba94c72b3c0de7c
	ret = libusb_set_option(infos->ctx, LIBUSB_OPTION_WEAK_AUTHORITY, NULL);
	if (ret != LIBUSB_SUCCESS) 
	{
		LOG(" error - libusb_init failed: %s\n", libusb_strerror(ret));
		return -1;
	}

	ret = libusb_init(&infos->ctx);
	if (ret != LIBUSB_SUCCESS)
	{
		infos->ctx = NULL; // be sure that the context is reseted
		LOG("error : %s", libusb_strerror(ret));
		// The device close function shall be called to free any allocated resources
		return -1;
	}

	// try the libusb debug to see if it displays more info
	libusb_set_option(infos->ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);

	return 0;
}

void devlibusb_log_devInfo(libusb_device * dev)
{
	int i,j,k;
	int ret;
	struct libusb_config_descriptor *config;
	// config index : default 0 but why
	ret = libusb_get_config_descriptor(dev, 0, &config);
	if (ret != LIBUSB_SUCCESS)
	{
		LOG("error : %s", libusb_strerror(ret));
		return;
	}
#define STR_INTERFACE_DESC "int[%d] sett[%d] intno=%d"
#define STR_ENDPOINT_DESC "ep[%d] type=%d addr=%d attr=%d"

	const struct libusb_interface *inter;
	const struct libusb_interface_descriptor *interdesc;
	const struct libusb_endpoint_descriptor *epdesc;
	for ( i = 0; i < (int)config->bNumInterfaces; i++) 
	{
		inter = &config->interface[i];
		for (j = 0; j < inter->num_altsetting; j++) 
		{
			interdesc = &inter->altsetting[j];
			LOG(STR_INTERFACE_DESC, i, j, interdesc->bInterfaceNumber);
			for (k = 0; k < (int)interdesc->bNumEndpoints; k++)
			{
				epdesc = &interdesc->endpoint[k];
				LOG(STR_ENDPOINT_DESC, k, epdesc->bDescriptorType, epdesc->bEndpointAddress, epdesc->bmAttributes);
			}
		}
	}
	libusb_free_config_descriptor(config);
}