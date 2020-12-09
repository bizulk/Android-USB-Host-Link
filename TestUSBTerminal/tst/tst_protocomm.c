/*
 * tst_protocomm.c
 *
 *  Created on: 24 mai 2020
 *      Author: sli
 *
 *  Implementation of the dllCom / protocomm protocol slave
 *
 *
 */
#include "tst_select.h"


#ifdef TST_PROTOCOMM

#include "usbd_cdc_if.h"
#include "protocomm_slave.h"
#include "device_stm32.h"

// Specific stm32 device that interface the usb
static proto_Device_t _stm32dev;
//! Protocole Handle
static proto_hdle_t * _protoSlave;

#define MY_TABLE_LEN 5

/** We use simple array to map register ids and values */
static uint8_t _myTable[MY_TABLE_LEN] = {0};

/*** This is the protocol slave callback that will handle commands
 *
 */
static int slave_receive(void* userdata, proto_Command_t command, proto_frame_data_t* args_inout);


void tst_init(void)
{
	/* We instanciate our protocol */
	_stm32dev = devstm32_create();
	_protoSlave = proto_slave_create(_stm32dev, slave_receive, NULL);
}

int8_t _CDC_Receive_FS_user(uint8_t* Buf, uint32_t *Len)
{
	// When receiving data from usb, push it the the device FIFO (async process)
	devstm32_pushBytes(_stm32dev, Buf, *Len);
	return USBD_OK;
}

void tst_loop_main( void )
{
	// We'll process all the slave communication here
	proto_slave_main(_protoSlave);
}

static int slave_receive(void* userdata, proto_Command_t command, proto_frame_data_t* args_inout)
{
	int ret = 0;
    UNUSED(userdata);
    switch (command)
    {
    case proto_CMD_SET: // Master ask to change register value
        if (args_inout->req.reg < MY_TABLE_LEN) {
        	_myTable[args_inout->req.reg] = args_inout->req.value;
        } else
            ret = -1;
        break;

    case proto_CMD_GET: // Master ask register value
        if (args_inout->req.reg < MY_TABLE_LEN)
        	args_inout->reg_value = _myTable[args_inout->req.reg];
        else
            ret = -1;
        break;

    default:
        ret = -1;
        break;
    }
    return ret;
}
#endif

