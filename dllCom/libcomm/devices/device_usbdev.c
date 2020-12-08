/// Implement a usbdev_fs device.
/// We're using the linux API to call bulk transfer
/// Incompatible file for iOS 

#include <stdlib.h>
#include <assert.h>
#include "proto_iodevice.h"
#include "device_usbdev.h"
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/usbdevice_fs.h>

#include "log.h"

/******************************************************************************
 * TYPES & VARIABLES
******************************************************************************/

/* User data */
typedef struct
{
    int fd; //!<  file descriptor
	int ep_in; //!< endpoint number for device to host communication
	int ep_out;  //!< endpoint number for host to device communication
	int max_pkt_size;
} proto_dev_usbdev_t ;

#define DEVUSB_BULK_XFER_TOUT_MS 1000

/******************************************************************************
 * LOCAL PROTO
******************************************************************************/

///
/// \brief devusbdev_init : même chose que create, mais avec une instance alloué sur la pile
/// \warning Ne pas appeler "destroy" pour ce cas
/// \param _this Instance pre-alloué
///
static void devusbdev_init(proto_Device_t _this);


/******************************************************************************
 * FUNCTION
******************************************************************************/

void devusbdev_destroy(proto_Device_t _this)
{
    assert(_this && _this->user);

	/// system ressource is managed outside dll com.
    /// Liberation des structures
	LOG("info");
    _this->close(_this);
    free(_this->user);
    free(_this);
}

static int devusbdev_close(struct proto_IfaceIODevice* _this)
{
    assert(_this);
	int ret = 0;

    proto_dev_usbdev_t* infos = _this->user;
	
	// Just invalidate descriptor file, we don't own the ressources
	if (infos->fd >=0)
	{
		infos->fd = -1;
	}

	return ret;
}

static int devusbdev_open(struct proto_IfaceIODevice* _this, const char * szPath)
{
    assert(_this && szPath);

    //proto_dev_usbdev_t* infos = __this->user;
	
	// Don't do anything : we wait for the device info
	
	return 0;
}


static int devusbdev_read(struct proto_IfaceIODevice* _this, void* buffer, uint8_t len, int16_t tout_ms)
{
	assert(_this && buffer);
	proto_dev_usbdev_t* infos = _this->user;
	if (infos->fd < 0)
		return -1;

	LOG("read %d bytes from fd/%d", len, infos->fd);
	// Do a bulk transfert for data
	// TODO : split data write if overriding packet size
	struct usbdevfs_bulktransfer frame = { 0 };
	frame.ep = infos->ep_in;
	frame.len = len;
	frame.timeout = DEVUSB_BULK_XFER_TOUT_MS;
	frame.data = (void*)buffer;
	int res = ioctl(infos->fd, _IOWR('U', 0, struct usbdevfs_ctrltransfer), &frame);

	if (res < 0)
	{
		LOG("error : %s (fd=%d)", strerror(errno), infos->fd);
		return -1;
	}
	return 0;
}


static int devusbdev_write(struct proto_IfaceIODevice* _this, const void * buffer, uint8_t size)
{
	assert(_this && buffer);
	proto_dev_usbdev_t* infos = _this->user;
	if (infos->fd < 0)
		return -1;

	LOG("writing %d bytes to fd.%d ep.%d", size, infos->fd, infos->ep_out);
	// Do a bulk transfert for data
	// TODO : split data write if overriding packet size
	struct usbdevfs_bulktransfer frame = { 0 };
	frame.ep = infos->ep_out;
	frame.len = size;
	frame.timeout = DEVUSB_BULK_XFER_TOUT_MS;
	frame.data = (void*)buffer;
	int res = ioctl(infos->fd, _IOWR('U', 0, struct usbdevfs_ctrltransfer), &frame);

	if (res < 0)
	{
		LOG("error : %s (fd=%d)", strerror(errno), infos->fd);
		return -1;
	}
	return 0;
}


int devusbdev_setDev(proto_Device_t _this, int fd, int ep_in, int ep_out, int max_pkt_size)
{
    assert(_this);
	proto_dev_usbdev_t* infos = _this->user;
	if( (fd < 0) || (ep_in < 0) || (ep_out < 0) )
		return -1;

	// On ferme les ressources existantes
	devusbdev_close(_this);
    infos->fd = fd;
	infos->ep_in = fd;
	infos->ep_out = ep_out;
	infos->max_pkt_size = max_pkt_size;

    return 0;
}

int devusbdev_getFD(proto_Device_t _this) 
{
    assert(_this);
	proto_dev_usbdev_t* infos = _this->user;
	
    return infos->fd;

}


proto_Device_t devusbdev_create(void)
{
    /// allocation des structures et initialisation du pointeur
    proto_Device_t _this  = (proto_Device_t)malloc( sizeof(proto_IfaceIODevice_t) );
    _this->user = (proto_dev_usbdev_t*)malloc(sizeof(proto_dev_usbdev_t));
    devusbdev_init(_this);

    return _this;
}


void devusbdev_init(proto_Device_t _this)
{
    assert(_this);
	DEVIO_INIT(devusbdev, _this);
    proto_dev_usbdev_t* infos = _this->user;
	// Initialize 'user' field to invalid state
    infos->fd		= -1;
	infos->ep_in	= -1;
	infos->ep_out	= -1;
}
