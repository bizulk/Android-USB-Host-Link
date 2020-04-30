
#define _XOPEN_SOURCE 600 // pour avoir accès à ptsname (nom de l'esclave associé à un maître ptmx)
#include <stdlib.h>

#include <protocomm-gnulinux.h>
#include <protocomm-details.h>

#ifdef __STDC_NO_THREADS__
#error "This code needs C11 <threads.h> to compile"
#endif
#include <threads.h>
#include <stdio.h>
#include <assert.h>

//========= CODE ESCLAVE ============
typedef struct DeviceInfo_t {
	proto_Device_t device;
	void* iodata;
} DeviceInfo_t;

// Le slave reçoit une demande GET [R] et retourne R*3.
// Les demandes SET échouent toujours.
// userdata doit être un pointeur vers DeviceInfo_t
void slave_receive(void* userdata, proto_Command_t command, uint8_t const* args_in) {
	uint8_t args_out[proto_MAX_ARGS];
	DeviceInfo_t const* devInfo = userdata;
	switch(command) {
	case proto_GET:
		args_out[0] = args_in[0] * 3;
		proto_writeFrame(proto_REPLY, args_out, devInfo->device, devInfo->iodata);
		break;
	case proto_SET:
		args_out[0] = proto_INVALID_REGISTER;
		proto_writeFrame(proto_STATUS, args_out, devInfo->device, devInfo->iodata);
		break;
	case proto_NOTIF_BAD_CRC:
		args_out[0] = proto_INVALID_CRC;
		proto_writeFrame(proto_STATUS, args_out, devInfo->device, devInfo->iodata);
		break;
	default:
		break;
	}
}

mtx_t slaveStopMtx;
int slaveStop = 0;

int slaveThreadFunc(void* data) {
	DeviceInfo_t* devInfo = data;
	proto_State_t state = { 0 };
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



int main() {
	// Initialisation des deux devices (un pour le maître et un pour l'esclave)
	proto_Data_GnuLinux_t master, slave;
	proto_Device_t device = proto_getDevice_GnuLinux(); // même device pour l'esclave et le maître : GNU/Linux
	
	int errMaster = proto_initData_GnuLinux(
			&master, "/dev/ptmx", 0); // le maître n'attend pas
	if (errMaster) {
		printf("Erreur en ouvrant le maître : %d\n", errMaster);
		return EXIT_FAILURE;
	}
	if (grantpt(master.fileDescriptor) < 0 || unlockpt(master.fileDescriptor) < 0) {
		perror("Erreur en donnant des privilèges au pseudo-terminal");
		proto_closeDevice(device, &master);
		return EXIT_FAILURE;
	}
	
	int errSlave = proto_initData_GnuLinux(
			&slave, ptsname(master.fileDescriptor), 3); // l'esclave attend au moins 0.3 seconde
	if (errSlave) {
		printf("Erreur en ouvrant l'esclave : %d\n", errSlave);
		proto_closeDevice(device, &master);
		return EXIT_FAILURE;
	}
	DeviceInfo_t slaveInfo = { .device = device,
		                       .iodata = &slave };               
	
	// Initialisation du contexte multi-threadé
	if (mtx_init(&slaveStopMtx, mtx_plain) == thrd_error) {
		perror("Erreur en créant un mutex");
		proto_closeDevice(device, &slave);
		proto_closeDevice(device, &master);
		return EXIT_FAILURE;
	}
	// Lancement du thread de l'esclave
	thrd_t slaveThreadId;
	if (thrd_create(&slaveThreadId, slaveThreadFunc, &slaveInfo)) {
		perror("Erreur en créant le thread");
		mtx_destroy(&slaveStopMtx);
		proto_closeDevice(device, &slave);
		proto_closeDevice(device, &master);
		return EXIT_FAILURE;
	}
	
	//============ CODE MASTER ============
	
	// Initialisation du maître
	uint8_t output = 0;
	
	// on veut écrire 0 dans le registre 0
	proto_Status_t status = proto_cio_master_set(0, 0, /*timeout_ms=*/ 100, device, &master);
	// On a normalement reçu une erreur : l'esclave utilisé dans ce
	// programme n'accepte pas les commandes SET !
	assert(status == proto_INVALID_REGISTER); 
	
	// on veut "lire le registre" 28
	status = proto_cio_master_get(28, &output, 100, device, &master);
	assert(status == proto_NO_ERROR);
	// On a normalement reçu 28*3 (cf. implémentation de l'esclave)
	assert(output == 28*3);
	
	puts("Tout s'est bien passé !");
	
	// On attend que le thread de l'esclave se termine
	mtx_lock(&slaveStopMtx);
	slaveStop = 1;
	mtx_unlock(&slaveStopMtx);
	thrd_join(slaveThreadId, NULL);
	
	mtx_destroy(&slaveStopMtx);
	proto_closeDevice(device, &slave);
	proto_closeDevice(device, &master);
	return EXIT_SUCCESS;
}

