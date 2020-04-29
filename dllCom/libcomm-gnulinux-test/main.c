
#define _XOPEN_SOURCE 600 // pour avoir accès à ptsname (nom de l'esclave associé à un maître ptmx)
#include <stdlib.h>

#include <protocomm-gnulinux.h>

#ifdef __STDC_NO_THREADS__
#error "This code needs C11 <threads.h> to compile"
#endif
#include <threads.h>
#include <stdio.h>
#include <assert.h>

//========= CODE ESCLAVE ============
typedef struct DeviceInfo {
	proto_Device device;
	void* iodata;
} DeviceInfo;

// Le slave reçoit une demande GET [R] et retourne R*3.
// Les demandes SET échouent toujours.
// userdata doit être un pointeur vers HowToReply
void slave_receive(void* userdata, proto_Command command, uint8_t const* args_in) {
	uint8_t args_out[proto_MAX_ARGS];
	DeviceInfo const* devInfo = userdata;
	switch(command) {
	case proto_GET:
		args_out[0] = args_in[0] * 3;
		proto_writeFrame(proto_REPLY, args_out, devInfo->device, devInfo->iodata);
		break;
	case proto_SET:
		args_out[0] = proto_INVALID_REGISTER;
		proto_writeFrame(proto_ERROR, args_out, devInfo->device, devInfo->iodata);
		break;
	case proto_NOTIF_BAD_CRC:
		args_out[0] = proto_INVALID_CRC;
		proto_writeFrame(proto_ERROR, args_out, devInfo->device, devInfo->iodata);
		break;
	default:
		break;
	}
}

mtx_t slaveStopMtx;
int slaveStop = 0;

int slaveThreadFunc(void* data) {
	DeviceInfo* devInfo = data;
	proto_State state = { 0 };
	proto_setReceiver(&state, slave_receive, devInfo);
	
	while(1) { // tant que slaveStop != 0
		mtx_lock(&slaveStopMtx);
		int shouldStop = slaveStop;
		mtx_unlock(&slaveStopMtx);
		if (shouldStop)
			break;
		
		proto_readBlob(&state, devInfo->device, devInfo->iodata);
		thrd_yield(); // l'OS peut nous faire attendre un peu
	}
	return 0;
}


//============ CODE MASTER ============
// Le master reçoit une réponse et l'écrit dans *userdata.
// userdata doit être un pointeur vers un uint8_t.
uint8_t master_lastError = -1;
void master_receive(void* userdata, proto_Command command, uint8_t const* args) {
	switch (command) {
	case proto_REPLY:
		*(uint8_t*)userdata = args[0];
		break;
	case proto_NOTIF_BAD_CRC:
		printf("Mauvais CRC... reçu=%d, calculé=%d\n", args[0], args[1]);
		break;
	case proto_ERROR:
		master_lastError = args[0];
		break;
	}
}



int main() {
	// Initialisation des deux devices (un pour le maître et un pour l'esclave)
	proto_Data_GnuLinux master, slave;
	
	int errMaster = proto_initData_GnuLinux(
			&master, "/dev/ptmx", 0); // le maître n'attend pas
	if (errMaster) {
		printf("Erreur en ouvrant le maître : %d\n", errMaster);
		return EXIT_FAILURE;
	}
	if (grantpt(master.fileDescriptor) < 0 || unlockpt(master.fileDescriptor) < 0) {
		perror("Erreur en donnant des privilèges au pseudo-terminal");
		proto_finalizeData_GnuLinux(&master);
		return EXIT_FAILURE;
	}
	proto_Device deviceMaster = proto_getDevice_GnuLinux();
	
	int errSlave = proto_initData_GnuLinux(
			&slave, ptsname(master.fileDescriptor), 3); // l'esclave attend au moins 0.3 seconde
	if (errSlave) {
		printf("Erreur en ouvrant l'esclave : %d\n", errSlave);
		proto_finalizeData_GnuLinux(&master);
		return EXIT_FAILURE;
	}
	DeviceInfo slaveInfo = { .device = proto_getDevice_GnuLinux(),
		                     .iodata = &slave };               
	
	// Initialisation du contexte multi-threadé
	if (mtx_init(&slaveStopMtx, mtx_plain) == thrd_error) {
		perror("Erreur en créant un mutex");
		proto_finalizeData_GnuLinux(&master);
		proto_finalizeData_GnuLinux(&slave);
		return EXIT_FAILURE;
	}
	// Lancement du thread de l'esclave
	thrd_t slaveThreadId;
	if (thrd_create(&slaveThreadId, slaveThreadFunc, &slaveInfo)) {
		perror("Erreur en créant le thread");
		mtx_destroy(&slaveStopMtx);
		proto_finalizeData_GnuLinux(&master);
		proto_finalizeData_GnuLinux(&slave);
		return EXIT_FAILURE;
	}
	
	// Initialisation du maître
	proto_State state = { 0 };
	uint8_t output = 0;
	proto_setReceiver(&state, master_receive, &output);
	
	uint8_t args[proto_MAX_ARGS];
	
	// Tout a été correctement initialisé
	args[0] = 0; args[1] = 0; // on veut écrire 0 dans le registre 0
	proto_writeFrame(proto_SET, args, deviceMaster, &master);
	while (!proto_readBlob(&state, deviceMaster, &master));
	// On a normalement reçu une erreur : l'esclave utilisé dans ce
	// programme n'accepte pas les commandes SET !
	assert(master_lastError == proto_INVALID_REGISTER);
	
	master_lastError = -1; // réinitialisation
	
	args[0] = 28; // on veut lire le "registre" 28
	proto_writeFrame(proto_GET, args, deviceMaster, &master);
	while (!proto_readBlob(&state, deviceMaster, &master));
	// On a normalement reçu 28*3 (cf. implémentation de l'esclave)
	assert(output == 28*3);
	
	puts("Tout s'est bien passé !");
	
	// On attend que le thread de l'esclave se termine
	mtx_lock(&slaveStopMtx);
	slaveStop = 1;
	mtx_unlock(&slaveStopMtx);
	thrd_join(slaveThreadId, NULL);
	
	mtx_destroy(&slaveStopMtx);
	proto_finalizeData_GnuLinux(&master);
	proto_finalizeData_GnuLinux(&slave);
	return EXIT_SUCCESS;
}

