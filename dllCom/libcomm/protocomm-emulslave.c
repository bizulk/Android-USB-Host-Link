#include "protocomm-emulslave.h"
#include <string.h>
#include <assert.h>

static void pushFrame(proto_Data_EmulSlave_t* data, proto_Command_t command, uint8_t const* args) {
	if (data->priv_nbDelayedBytes + proto_FRAME_MAXSIZE < sizeof(data->priv_delayedBytes)) {
		data->priv_nbDelayedBytes += proto_makeFrame(
		    (void*)(data->priv_delayedBytes + data->priv_nbDelayedBytes),
		    command, args);
	}
}

/// SLI data -> appelle ca plutôt this plutôt
/// Cette fonction sert à stacker les réponses qui devront être dépilés comment.
static void pushFrame1arg(proto_Data_EmulSlave_t* data, proto_Command_t command, uint8_t arg1) {
	pushFrame(data, command, &arg1);
}

static void emulslave_callback(void* userdata, proto_Command_t command, uint8_t const* args) {
	proto_Data_EmulSlave_t* data = userdata;
	switch (command) {
	case proto_SET: // quand le MASTER demande de changer une valeur
		if (args[0] < 20) {
			data->registers[args[0]] = args[1];
			pushFrame1arg(data, proto_STATUS, proto_NO_ERROR);
		} else
			pushFrame1arg(data, proto_STATUS, proto_INVALID_REGISTER);
		break;
		
	case proto_GET: // quand le MASTER demande d'accéder à une valeur
		if (args[0] < 20)
			pushFrame1arg(data, proto_REPLY, data->registers[args[0]]);
		else
			pushFrame1arg(data, proto_STATUS, proto_INVALID_REGISTER);
		break;
		
	case proto_NOTIF_BAD_CRC: // quand la bibliothèque a détecté un mauvais CRC
		pushFrame1arg(data, proto_STATUS, proto_INVALID_CRC);
		break;
		
	default:
		// On ne réagit pas aux commandes proto_REPLY et proto_ERROR
		break;
	}
}

static void emulslave_write(void* iodata, uint8_t const* buffer, uint8_t size) {
	assert(iodata != NULL);
	assert(buffer != NULL);
	proto_Data_EmulSlave_t* data = iodata;
	proto_setReceiver(&data->priv_state, emulslave_callback, data); 
	proto_interpretBlob(&data->priv_state, buffer, size);
}

static uint8_t emulslave_read(void* iodata, uint8_t* buffer, uint8_t bufferSize) {
	assert(iodata != NULL);
	assert(buffer != NULL);
	proto_Data_EmulSlave_t* data = iodata;
	
	uint8_t nbDelayedBytes = data->priv_nbDelayedBytes;
	
	// On prend le minimum entre bufferSize et nbDelayedBytes
	int nbRead = bufferSize < nbDelayedBytes ? bufferSize : nbDelayedBytes;
	// On copie les octets dans le buffer donné
	memcpy(buffer, data->priv_delayedBytes, nbRead);
	// On décale les octets (pour enlever ceux qui viennent d'être lus)
	memmove(data->priv_delayedBytes, data->priv_delayedBytes + nbRead, nbDelayedBytes - nbRead);
	data->priv_nbDelayedBytes -= nbRead;
	
	return nbRead;
}

static void emulslave_destroy(void* iodata) { /* ne fait rien */ }

void proto_initData_EmulSlave(proto_Data_EmulSlave_t* iodata) {
	assert(iodata != NULL);
	memset(iodata, 0, sizeof(*iodata));
}

static proto_IfaceIODevice_t emulslaveDevice = {
	.write = emulslave_write, .read = emulslave_read, .destroy = emulslave_destroy
};

proto_Device_t proto_getDevice_EmulSlave(void) {
	return &emulslaveDevice;
}
