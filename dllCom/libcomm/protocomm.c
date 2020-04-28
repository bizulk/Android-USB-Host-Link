#include "protocomm.h"
#include <assert.h>
#include <string.h>


static uint8_t getCRC(uint8_t const* data, uint8_t size) {
	//TODO: À implémenter
	return 0;
}


void proto_setReceiver(proto_State* state, proto_OnReception callback, void* userdata) {
	assert(state != NULL);
	state->priv_callback = callback;
	state->priv_userdata = userdata;
}

/// Retourne 1 si une trame a été finie de lire, 0 sinon
static int proto_readByte(proto_State* state, uint8_t byte) {
	if (state->priv_frameSize == 0 && byte != proto_HEADER)
		return; // on ignore les octets avant le header de la trame
	
	state->priv_frameBuffer[state->priv_frameSize++] = byte;
	
	if (state->priv_frameSize >= proto_ARGS_OFFSET) {
		// si on a reçu l'octet de commande
		uint8_t command = state->priv_frameBuffer[proto_COMMAND_OFFSET];
		uint8_t nbArgs = proto_getArgsSize(command);
		
		if (state->priv_frameSize == proto_ARGS_OFFSET + nbArgs) {
			// si on a reçu tous les octets d'arguments
			// on calcule le CRC sans compter le CRC reçu ni le Header
			uint8_t crcCalc = getCRC(
				state->priv_frameBuffer + proto_COMMAND_OFFSET, 1 + nbArgs);
			uint8_t crcRecu = state->priv_frameBuffer[1];
			if (crcCalc != crcRecu) {
				// si le CRC ne correspond pas, on remplace la trame 
				// actuelle par une notification de type BAD_CRC
				uint8_t args[2] = { crcRecu, crcCalc };
				state->priv_frameSize = proto_writeFrame(
					state->priv_frameBuffer, proto_BAD_CRC, args);
			}
			return 1; // une trame a été reçue en entier
		}
	}
	return 0; // la trame n'a pas été reçue en entier
}

int proto_readBlob(proto_State* state, uint8_t const* blob, uint8_t size) {
	assert(state != NULL);
	int nbFrameEnd = 0;
	for (uint8_t i = 0; i < size; ++i) {
		int frameEnd = proto_readByte(state, blob[i]);
		if (frameEnd) {
			state->priv_callback(
			    state->priv_userdata,
			    state->priv_frameBuffer[proto_COMMAND_OFFSET],
			    state->priv_frameBuffer + proto_ARGS_OFFSET);
			++nbFrameEnd;
		}
	}
	return nbFrameEnd;
}

int proto_makeFrame(uint8_t* buffer, proto_Command command, uint8_t const* args) {
	assert(buffer != NULL);
	assert(args != NULL);
	uint8_t nbArgs = proto_getArgsSize(command);
	
	buffer[0] = proto_HEADER;
	buffer[proto_COMMAND_OFFSET] = command;
	memcpy(buffer + proto_ARGS_OFFSET, args, nbArgs);
	buffer[1] = getCRC(buffer + proto_COMMAND_OFFSET, 1 + nbArgs);
	
	return proto_ARGS_OFFSET + nbArgs;
}
