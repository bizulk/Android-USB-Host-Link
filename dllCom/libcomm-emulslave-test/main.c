#include <protocomm-emulslave.h>
// SLI 
// Expliquer ce que fait ce test : 
// une fonction par test cela serait plus clair
// comment dans un contexte monothread valide -t-on le fonctionnement du slave ? (je le sais mais je fais une critique sur le manque de commentaire

#include <assert.h>
#include <string.h>
#include <stdio.h>

// API bas-niveau pour tester les comportements bas-niveau (ici, le CRC)

typedef struct master_Data_t {
	uint8_t lastCommand;
	uint8_t lastArgs[proto_MAX_ARGS];
	uint8_t nbReceived;
} master_Data_t;

void master_callback(void* userdata, proto_Command_t command, uint8_t const* args) {
	master_Data_t* data = userdata;
	++data->nbReceived;
	data->lastCommand = command;
	memcpy(data->lastArgs, args, proto_getArgsSize(command));
}
proto_State_t state = { 0 };

// définition des tests

proto_Data_EmulSlave_t devicedata;
proto_Device_t device;
master_Data_t masterdata = { 0 };
uint8_t args[proto_MAX_ARGS] = { 0 };


void test_set_NO_ERROR() {
	devicedata.registers[5] = 0; // Au début, on a le registre 5 avec la valeur 0
	// On fait une demande d'écriture
	proto_Status_t status = proto_cio_master_set(5, 167, /*timeout_ms=*/0, device, &devicedata);
	// PS : pas besoin de timeout car c'est instantané avec l'émulation
	assert(status == proto_NO_ERROR);
	assert(devicedata.registers[5] == 167);
}

void test_set_INVALID_REGISTER() {
	// L'émulation ne dispose que de 20 registres
	// Ecrire dans un registre au-delà est une erreur !
	proto_Status_t status = proto_cio_master_set(25, 0, 0, device, &devicedata);
	assert(status == proto_INVALID_REGISTER);
}

void test_get_NO_ERROR() {
	uint8_t valeur = 0;
	devicedata.registers[18] = 234;
	proto_Status_t status = proto_cio_master_get(18, &valeur, 0, device, &devicedata);
	assert(status == proto_NO_ERROR);
	assert(valeur == 234); 
}

void test_get_INVALID_REGISTER() {
	// L'émulation ne dispose que de 20 registres
	// Lire un registre au-delà est une erreur !
	uint8_t valeur = 23;
	proto_Status_t status = proto_cio_master_get(45, &valeur, 0, device, &devicedata);
	assert(status == proto_INVALID_REGISTER);
	assert(valeur == 23); // en cas d'erreur, la sortie n'est pas modifiée
}

void test_crc() {
	// On crée une frame nous-même, avec un CRC pertinemment faux
	proto_Frame_t frame = { 0 };
	frame.startOfFrame = proto_START_OF_FRAME;
	frame.crc8 = 0; // le CRC est pertinemment faux !
	frame.command = proto_STATUS;
	frame.args[0] = proto_NO_ERROR;
	
	// On essaye de lire la frame
	proto_interpretBlob(&state, (void*)&frame, 4);
	// Une erreur de CRC est normalement arrivée !
	assert(masterdata.lastCommand == proto_NOTIF_BAD_CRC);
}


int main() {
	
	// Initialisation du device d'émulation
	proto_initData_EmulSlave(&devicedata);
	device = proto_getDevice_EmulSlave();
	
	// Initialisation du maître
	proto_setReceiver(&state, master_callback, &masterdata);
	
	
	test_set_NO_ERROR();
	
	test_set_INVALID_REGISTER();
	
	test_get_NO_ERROR();
	
	test_get_INVALID_REGISTER();
	
	test_crc();
	
	puts("Tous les tests ont réussi !");
	
	
	proto_closeDevice(device, &devicedata);
	
	return 0;
}
