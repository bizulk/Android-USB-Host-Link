/// Implement a proxy device.
/// We're using the linux socket API only.

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <netinet/tcp.h>

#include "proto_iodevice.h"
#include "device_proxy.h"
#include "log.h"

/******************************************************************************
 * TYPES & VARIABLES
******************************************************************************/

/* User data */
typedef struct
{
    int fd; ///  file descriptor from android hardware usb api
} proto_dev_proxy_t ;

#define DEVPROXY_FD_INVALID -1

/******************************************************************************
 * LOCAL PROTO
******************************************************************************/

///
/// \brief devproxy_init : make the allocated structure initialization
/// \param _this Instance
///
static void devproxy_init(proto_Device_t _this);

///
/// \brief wrapper on the sys call send to complete all data
/// \param fd file descriptor
/// \param buffer data to send
/// \param len buffer len
/// \param flags flags to pass to send function
/// \return 0 on success (all data) otherwise error
static int devproxy_send(int fd, const void * buffer, int len, int flags);


static int devproxy_recv(int fd, void * buffer, int len, int flags);

/// \brief Validate function use commonly for read/write function
/// \param pHeader message to validate
/// \param len field datalen value expected
///	\return 0 message if valid, otherwise -1
static int devproxy_headerValidate(devproxy_header_t * pHeader, int len);

/******************************************************************************
 * FUNCTION
******************************************************************************/

void devproxy_destroy(proto_Device_t _this)
{
    assert(_this && _this->user);
	//proto_dev_proxy_t* infos = _this->user;
	/// system ressource is managed outside dll com.
    /// Liberation des structures
	LOG("destroying ressources");

    _this->close(_this);
    free(_this->user);
    free(_this);
}

static int devproxy_close(struct proto_IfaceIODevice* _this)
{
    assert(_this);
	int ret = 0;

    proto_dev_proxy_t* infos = _this->user;
	
	// Just invalidate descriptor file, we don't own the ressources
	if (infos->fd >=0)
	{
		close(infos->fd);
		infos->fd = DEVPROXY_FD_INVALID;
	}

	return ret;
}

static int devproxy_open(struct proto_IfaceIODevice* _this, const char * szPath)
{
    assert(_this && szPath);

	proto_dev_proxy_t* infos = _this->user;
	struct sockaddr_in serv_addr;
	unsigned short usPort = PROXY_DEFAULT_PORT;
	char szFields[25] = { 0 } ; // For IP & port field extraction
	const char * szIp = NULL;
	const char * szPort = NULL;

	if ((infos->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		LOG(" Error : Could not create socket");
		return 1;
	}

	strncpy(szFields, szPath, sizeof(szIp));
	szIp = strtok(szFields, PROXY_URL_SEP); // has modified the ':' to null char
	szPort = strtok(NULL, PROXY_URL_SEP);
	if (szPort != NULL)
	{
		// TODO (lazy conversion) better use strtol
		usPort = atoi(szPort);
		LOG("info - opening port %d", usPort);
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(usPort);

	
	if (inet_pton(AF_INET, szIp, &serv_addr.sin_addr) <= 0)
	{
		LOG("error with IP addr");
		return -1;
	}

	if (connect(infos->fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		LOG("error when connecting");
		return -1;
	}

	if (PROXY_SOCKET_OPT_NODLY)
	{
		int flag = 1;
		int result = setsockopt(infos->fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
		if (result < 0) 
		{
			LOG("could not set tcp_nodelay option");
		}
	}
	LOG("connected");
	return 0;
}

static int devproxy_read(struct proto_IfaceIODevice* _this, void* buffer, uint8_t len, int16_t tout_ms)
{
	assert(_this && buffer);
	proto_dev_proxy_t* infos = _this->user;
	
	if (infos->fd == DEVPROXY_FD_INVALID)
	{
		LOG("cannot write, socket is invalide");
		return -1;
	}
	// Write the header then write the data
	devproxy_header_t header = { 0 };
	header.SOF = DEVPROXY_HEADER_MAGIC;
	header.code = PROXY_CMD_READ;
	header.datalen = len;


	if (devproxy_send(infos->fd, &header, sizeof(header), 0) != 0)
	{
		LOG("send error");
	}

	// Now wait the proxy to reply
	memset(&header, 0, sizeof(header));
	if (devproxy_recv(infos->fd, &header, sizeof(header), 0) != 0)
	{
		LOG("rcv error");
		return -1;
	}

	// We expect the proxy to send us all the data, as himself apply a timeout on device
	if (devproxy_headerValidate(&header, len) != 0)
	{
		return -1;
	}

	if (devproxy_recv(infos->fd, buffer, len, 0) != 0)
	{
		LOG("rcv error");
		return -1;
	}

	return len;
}

static int devproxy_write(struct proto_IfaceIODevice* _this, const void * buffer, uint8_t len)
{
	assert(_this && buffer);
	proto_dev_proxy_t* infos = _this->user;
	

	if (infos->fd == DEVPROXY_FD_INVALID)
	{
		LOG("cannot write, socket is invalide");
		return -1;
	}
	// Write the header then write the data
	devproxy_header_t header = { 0 };
	header.SOF = DEVPROXY_HEADER_MAGIC;
	header.code = PROXY_CMD_WRITE;
	header.datalen = len;
 
	// We notify the kernel we have more data to send
	if (devproxy_send(infos->fd, &header, sizeof(header), MSG_MORE) != 0)
	{
		LOG("send error");
	}
	if (devproxy_send(infos->fd, buffer, len, 0) != 0)
	{
		LOG("send error");
	}

	// Now wait the proxy to reply
	memset(&header, 0, sizeof(header));
	if (devproxy_recv(infos->fd, &header, sizeof(header), 0) != 0)
	{
		LOG("rcv error");
		return -1;
	}
	if (devproxy_headerValidate(&header, 0) != 0)
	{
		return -1;
	}
	return 0;
}

proto_Device_t devproxy_create(void)
{
    /// allocation des structures et initialisation du pointeur
    proto_Device_t _this  = (proto_Device_t)malloc( sizeof(proto_IfaceIODevice_t) );
    _this->user = (proto_dev_proxy_t*)malloc(sizeof(proto_dev_proxy_t));
	//proto_dev_proxy_t* infos = _this->user;
    devproxy_init(_this);

    return _this;
}

void devproxy_init(proto_Device_t _this)
{
    assert(_this);
	DEVIO_INIT(devproxy, _this);
    proto_dev_proxy_t* infos = _this->user;
	// Initialize 'user' field to invalid state
    infos->fd		= DEVPROXY_FD_INVALID;
}

int devproxy_send(int fd, const void * buffer, int len, int flags)
{
	int sent = 0;
	int ret = 0;

	assert(fd && buffer && (len > 0));
	// TODO : use a non blocking call to setup a timeout 
	sent = 0;
	do
	{
		ret = send(fd, buffer, len, flags);
		if (ret >= 0)
		{
			sent += ret;
		}
	} while ((sent < len) && (ret > 0));

	return (sent == len) ? 0 : -1;
}

static int devproxy_recv(int fd, void * buffer, int len, int flags)
{
	assert(fd && buffer && (len > 0));

	int read = 0;
	int ret = 0;

	assert(fd && buffer && (len > 0));
	// TODO : use a non blocking call to setup a timeout 
	read = 0;
	do
	{
		ret = recv(fd, buffer, len, flags);
		if (ret >= 0)
		{
			read += ret;
		}
	} while ((read < len) && (ret > 0));

	return (read == len) ? 0 : -1;
}

static int devproxy_headerValidate(devproxy_header_t * pHeader, int len)
{
	assert(pHeader && (len >= 0));

	// Validate message
	if ((pHeader->SOF != DEVPROXY_HEADER_MAGIC) || (pHeader->datalen != len))
	{
		LOG("error - proxy sent unexpected reply (SOF =%X, len=%lu)", (unsigned int)pHeader->SOF, pHeader->datalen);
		return -1;
	}
	// validate operation
	if (pHeader->code != PROXY_REP_DONE)
	{
		switch (pHeader->code)
		{
		case PROXY_REP_NACK:
			// TODO when it occurs to many time we should close & reopen socket
			LOG("error - proxy refused the request");
			break;
		case PROXY_REP_ERR:
			// We cannot the anything
			LOG("error - proxy could not perform the request");
			break;
		default:
			break;
		}
		return -1;
	}
	return 0;
}
