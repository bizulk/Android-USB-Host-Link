#ifndef DEVICE_PROXY_H
#define DEVICE_PROXY_H

#ifdef __cplusplus
extern "C" {
#endif


/// We created _this device specifically for android.
/// As it is not possible to find a suitable way to embded native usb transfert, we let the Android Site do them, 
/// We connect to a tcp socket server and send macros.
/// Note that we suppose that the client has the same endianness as the server (this is local exécution)
#include "proto_iodevice.h"
#include "libcomm_global.h"


///
/// \brief Return a interface instance
/// \return Instance de notre device
///
proto_Device_t LIBCOMM_EXPORT devproxy_create(void);

/// Magic number for header packet
#define DEVPROXY_HEADER_MAGIC 0xCAFEBABE

/// Define the max time we wait for readin/writing to proxy
#define PROXY_SOCKET_XFER_TOUT_MS 1000

/// If defined to !=0, client will set the socket to disable Nable Algorithm
#define PROXY_SOCKET_OPT_NODLY 1

/// Define the default server port to connect
/// The port is defined in the URL
#define PROXY_DEFAULT_PORT 5000

/// Define the URL separator 127.0.0.1:4000
#define PROXY_URL_SEP ":"

/// Command and reply with the proxy
typedef enum
{
	PROXY_CMD_WRITE, ///< Request proxy to perform a write
	PROXY_CMD_READ, ///< Request the proxy to perform a read
	PROXY_REP_DONE, ///< The Proxy did perform the operation
	PROXY_REP_NACK, ///< The proxy did not accept the request
	PROXY_REP_ERR, ///< the proxy met an error
	PROXY_OPCODE_LAST,
	PROXY_INT_VALUE=(int)0x7FFFFFFF ///!< force the enum to be coded with int
} devproxy_opcode_t;

/// Define a header for packet data we'll exchange with the proxy.
typedef struct 
{
	unsigned long SOF; ///< magic number for syncing with the proxy.
	devproxy_opcode_t code; ///< request/reply
	unsigned long datalen; ///< datalen that is following the header, or that we expect from the proxy
} devproxy_header_t;


#ifdef __cplusplus
} // extern "C"
#endif

#endif // DEVICE_PROXY_H
