#include "protocomm-details.h"

#include <assert.h>
#include <string.h>

static uint8_t getCRC(uint8_t const* data, uint8_t size) {
    // plus d'info : https://en.wikipedia.org/wiki/Cyclic_redundancy_check#Computation
    // implémentation de base : https://stackoverflow.com/a/51777726
    unsigned crc = 0; // l'accumulateur pour les divisions
    while (size--) {
        crc ^= *data++; // on prend un nouvel octet de la séquence
        for (unsigned k = 0; k < 8; k++) // pour chacun des 8 bits, dans l'ordre:
            crc = crc & 0x80 ? // si le bit "actuel" est 1
                    (crc << 1) ^ 0x31 : // on applique la division polynomiale
                    crc << 1; // sinon on passe au bit suivant
    }
    return crc & 0xff; // on ne garde que les 8 bits de poids faibles
}

static uint8_t getFrameCRC(uint8_t const* frame) {
	return getCRC(frame + proto_COMMAND_OFFSET,
	              1 + proto_getArgsSize(frame[proto_COMMAND_OFFSET]));
}

uint8_t proto_getArgsSize(proto_Command_t command) {
    static uint8_t lut[proto_LAST] = {
        2, // proto_SET     : [REGISTRE] [VALEUR]
        1, // proto_GET     : [REGISTRE]
        1, // proto_REPLY   : [VALEUR]
        1, // proto_STATUS  : [proto_Status]
        2  // proto_NOTIF_BAD_CRC : [RECU] [CALCULE]
    };
    uint8_t nbArgs = command < proto_LAST ? lut[command] : 0;
    assert(nbArgs <= proto_MAX_ARGS);
    return nbArgs;
}

void proto_setReceiver(proto_State_t* state, proto_OnReception_t callback, void* userdata) {
	assert(state != NULL);
	state->priv_callback = callback;
	state->priv_userdata = userdata;
}

/// Statut en résultat de proto_interpretByte
typedef enum proto_ResultStatus_t {
	proto_WAITING,   /// La trame n'est pas finie
	proto_COMPLETED, /// On a lu une trame correctement
	proto_REFUSED    /// On a lu une trame mais il y a eu une erreur (exemple : CRC)
} proto_ResultStatus_t;

/// Cette fonction est responsable de créer la trame de notification 
/// d'erreur car c'est elle qui a le plus d'informations sur le CRC.
static proto_ResultStatus_t proto_interpretByte(proto_State_t* state, uint8_t byte) {
	// Synchronisation sur le SOF
	if (state->priv_nbBytes == 0 && byte != proto_START_OF_FRAME)
		return proto_WAITING; 
	uint8_t* frameAsBytes = (void*) &state->priv_frame;
	// Empilement des octets reçu
	frameAsBytes[state->priv_nbBytes++] = byte;
	// La trame n'est pas encore complète, on a juste accès à l'octet
	// de commande, donc on peut connaître le nombre d'arguments attendus.
	if (state->priv_nbBytes >= proto_ARGS_OFFSET) {
		// si on a reçu l'octet de commande
		uint8_t nbArgs = proto_getArgsSize(state->priv_frame.command);
		if (state->priv_nbBytes == proto_ARGS_OFFSET + nbArgs) {
			// si on a reçu tous les octets d'arguments, on passe les
			// test de validation
			
			// on calcule le CRC sans compter le CRC reçu ni le Header
			uint8_t crcCalc = getFrameCRC(frameAsBytes);
			if (crcCalc != state->priv_frame.crc8) {
				// si le CRC ne correspond pas, on remplace la trame 
				// actuelle par une notification de type BAD_CRC
				uint8_t args[2] = { state->priv_frame.crc8, crcCalc };
				state->priv_nbBytes = proto_makeFrame(&state->priv_frame, proto_NOTIF_BAD_CRC, args);
				return proto_REFUSED;
			}
			return proto_COMPLETED; // une trame a été reçue en entier
		}
	}
	return proto_WAITING; // la trame n'a pas été reçue en entier
}


int proto_interpretBlob(proto_State_t* state, uint8_t const* blob, uint8_t size) {
	assert(state != NULL);
	int nbFrameEnd = 0;
	for (uint8_t i = 0; i < size; ++i) {
		proto_ResultStatus_t status = proto_interpretByte(state, blob[i]);
		if (status != proto_WAITING) {
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

uint8_t proto_makeFrame(proto_Frame_t* frame, proto_Command_t command, uint8_t const* args) {
	assert(frame != NULL);
	assert(args != NULL);
	uint8_t nbArgs = proto_getArgsSize(command);
	
	frame->startOfFrame = proto_START_OF_FRAME;
	frame->command = command;
	memcpy(frame->args, args, nbArgs); // on copie les arguments
	memset(frame->args + nbArgs, 0, proto_MAX_ARGS - nbArgs); // et on met à zéro les autres octets
	frame->crc8 = getFrameCRC((void*)frame);
	return proto_ARGS_OFFSET + nbArgs;
}

int proto_readBlob(proto_State_t* state,
                   proto_Device_t iodevice, void* iodata) {
	uint8_t buffer[20];
	uint8_t nbRead = iodevice->read(iodata, buffer, 20);
	return proto_interpretBlob(state, buffer, nbRead);
}

void proto_writeFrame(proto_Command_t command, uint8_t const* args,
                      proto_Device_t iodevice, void* iodata) {
    proto_Frame_t frame;
    uint8_t nbBytes = proto_makeFrame(&frame, command, args);
    iodevice->write(iodata, (void*)&frame, nbBytes);
}


