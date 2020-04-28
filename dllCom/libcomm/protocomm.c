#include "protocomm.h"
#include <string.h>
#include <assert.h>


static uint8_t getCRC(uint8_t const* data, uint8_t size) {
	//TODO: À implémenter
	return 0;
}

uint8_t proto_getArgsSize(proto_Command command) {
    static uint8_t lut[proto_LAST] = {
        2, // proto_SET     : [REGISTRE] [VALEUR]
        1, // proto_GET     : [REGISTRE]
        1, // proto_REPLY   : [VALEUR]
        1, // proto_ERROR   : [NUM ERREUR]
        2  // proto_NOTIF_BAD_CRC : [RECU] [CALCULE]
    };
    uint8_t nbArgs = command < proto_LAST ? lut[command] : 0;
    assert(nbArgs <= proto_MAX_ARGS);
    return nbArgs;
}

void proto_setReceiver(proto_State* state, proto_OnReception callback, void* userdata) {
	assert(state != NULL);
	state->priv_callback = callback;
	state->priv_userdata = userdata;
}

/// Retourne 1 si une trame a été finie de lire, 0 sinon
static int proto_interpretByte(proto_State* state, uint8_t byte) {
	if (state->priv_nbBytes == 0 && byte != proto_HEADER)
		return 0; // on ignore les octets avant le header de la trame
	uint8_t* frameAsBytes = (void*) &state->priv_frame;
	
	frameAsBytes[state->priv_nbBytes++] = byte;
	
	if (state->priv_nbBytes >= proto_ARGS_OFFSET) {
		// si on a reçu l'octet de commande
		uint8_t nbArgs = proto_getArgsSize(state->priv_frame.command);
		if (state->priv_nbBytes == proto_ARGS_OFFSET + nbArgs) {
			// si on a reçu tous les octets d'arguments
			
			// on calcule le CRC sans compter le CRC reçu ni le Header
			uint8_t crcCalc = getCRC(
				frameAsBytes + proto_COMMAND_OFFSET, 1 + nbArgs);
			if (crcCalc != state->priv_frame.crc) {
				// si le CRC ne correspond pas, on remplace la trame 
				// actuelle par une notification de type BAD_CRC
				uint8_t args[2] = { state->priv_frame.crc, crcCalc };
				state->priv_nbBytes = proto_makeFrame(&state->priv_frame, proto_NOTIF_BAD_CRC, args);
			}
			return 1; // une trame a été reçue en entier
		}
	}
	return 0; // la trame n'a pas été reçue en entier
}

int proto_interpretBlob(proto_State* state, uint8_t const* blob, uint8_t size) {
	assert(state != NULL);
	int nbFrameEnd = 0;
	for (uint8_t i = 0; i < size; ++i) {
		int frameEnd = proto_interpretByte(state, blob[i]);
		if (frameEnd) {
			state->priv_callback(
			    state->priv_userdata,
			    state->priv_frame.command,
			    state->priv_frame.args);
			++nbFrameEnd;
			state->priv_nbBytes = 0;
		}
	}
	return nbFrameEnd;
}

uint8_t proto_makeFrame(proto_Frame* frame, proto_Command command, uint8_t const* args) {
	assert(frame != NULL);
	assert(args != NULL);
	uint8_t nbArgs = proto_getArgsSize(command);
	
	frame->header = proto_HEADER;
	frame->command = command;
	memcpy(frame->args, args, nbArgs); // on copie les arguments
	memset(frame->args + nbArgs, 0, proto_MAX_ARGS - nbArgs); // et on met à zéro les autres octets
	frame->crc = getCRC((uint8_t*)frame + proto_COMMAND_OFFSET, 1 + nbArgs);
	return proto_ARGS_OFFSET + nbArgs;
}

int proto_readBlob(proto_State* state,
                   proto_IfaceIODevice const* iodevice, void* iodata) {
	static uint8_t buffer[20];
	uint8_t nbRead = iodevice->read(iodata, buffer, 20);
	return proto_interpretBlob(state, buffer, nbRead);
}

void proto_writeFrame(proto_Command command, uint8_t const* args,
                      proto_IfaceIODevice const* iodevice, void* iodata) {
    static proto_Frame frame;
    uint8_t nbBytes = proto_makeFrame(&frame, command, args);
    iodevice->write(iodata, (void*)&frame, nbBytes);
}

