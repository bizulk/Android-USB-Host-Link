#include <protocomm-emulslave.h>
// SLI 
// Expliquer ce que fait ce test : 
// une fonction par test cela serait plus clair
// comment dans un contexte monothread valide -t-on le fonctionnement du slave ? (je le sais mais je fais une critique sur le manque de commentaire

#include <assert.h>
#include <string.h>
#include <stdio.h>

typedef struct master_Data {
	uint8_t lastCommand;
	uint8_t lastArgs[proto_MAX_ARGS];
	uint8_t nbReceived;
} master_Data;

void master_callback(void* userdata, proto_Command command, uint8_t const* args) {
	master_Data* data = userdata;
	++data->nbReceived;
	data->lastCommand = command;
	memcpy(data->lastArgs, args, proto_getArgsSize(command));
}

int main() {
	
	// Initialisation du device (code à changer en fonction du device)
	proto_Data_EmulSlave devicedata;
	proto_initData_EmulSlave(&devicedata);
	proto_Device device = proto_getDevice_EmulSlave();
	// Maintenant qu'on a initialisé le device, le code suivant est
	// indépendant du device sous-jacent
	
	// Initialisation du maître
	master_Data masterdata = {0};
	uint8_t args[proto_MAX_ARGS] = {0};
	proto_State state = {0};
	proto_setReceiver(&state, master_callback, &masterdata);
	
	// On fait une demande d'écriture
	args[0] = 5; // registre à écrire
	args[1] = 175; // nombre à écrire
	proto_writeFrame(proto_SET, args, device, &devicedata);
	
	/* SLI
		Comment l'esclave a-t-il pu prendre en compte la demande sans que proto_readBlob soit exécuté ?
	*/
	// L'esclave a bien pris en compte la demande
	assert(devicedata.registers[5] == 175);
	
	/* SLI deuxième test ICI ok
	*/
	devicedata.registers[13] = 218; // on écrit directement sur l'esclave
	// On fait une demande de lecture
	args[0] = 13; // registre à lire
	proto_writeFrame(proto_GET, args, device, &devicedata);
	/* SLI oui mais la ondevrait tester la valeur recu non ?? il manque un assert pour moi et aussi le readblob */

	/* !! Je ne comprends pas ce test là ??
	*/
	int nbFrameCompleted = proto_readBlob(&state, device, &devicedata);
	// Cette commande a lu 2 trames ! Une trame de STATUS (retourné
	// par SET) et une trame REPLY (retourné par GET)
	assert(nbFrameCompleted == 2); // on a du recevoir une frame
	assert(masterdata.nbReceived == 2); // le master aussi a vu une trame passer
	
	assert(masterdata.lastCommand == proto_REPLY); // on a bien reçu une trame de réponse
	assert(masterdata.lastArgs[0] == 218); // on a bien reçu la valeur attendue
	
	
	// on réinitialise masterdata
	masterdata.nbReceived = 0;
	
	// On fait une demande de lecture
	args[0] = 25; // erreur ! l'esclave (tel qu'implémenté par l'émulation) n'a que 20 registres
	proto_writeFrame(proto_GET, args, device, &devicedata);
	
	nbFrameCompleted = proto_readBlob(&state, device, &devicedata);
	assert(nbFrameCompleted == 1); // on a du recevoir une frame
	assert(masterdata.nbReceived == 1); // le master aussi a vu une trame passer
	
	assert(masterdata.lastCommand == proto_STATUS); // on a reçu une erreur de la part de l'esclave
	assert(masterdata.lastArgs[0] == proto_INVALID_REGISTER);
	
	/* Que teste-t-on ici ??*/
	proto_Frame frame = { 0 };
	frame.header = proto_HEADER;
	frame.crc = 0; // le CRC est pertinemment faux !
	frame.command = proto_STATUS;
	frame.args[0] = proto_NO_ERROR;
	
	proto_interpretBlob(&state, (void*)&frame, 4);
	// Une erreur de CRC est normalement arrivée !
	assert(masterdata.lastCommand == proto_NOTIF_BAD_CRC);
	
	
	puts("Tous les tests ont réussi !");
	
	// En fonction du device, il faudra peut-être le détruire.
	// Ici, le device est complètement alloué sur la pile, il sera donc
	// détruit à la fin de la fonction. Donc on ne fait rien.
	
	return 0;
}
