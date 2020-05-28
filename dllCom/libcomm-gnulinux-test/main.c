
#define _XOPEN_SOURCE 600 // pour avoir accès à ptsname (nom de l'esclave associé à un maître ptmx)
#include <stdlib.h>

#include <protocomm_master.h>
#include <protocomm_slave.h>
#include <device_serial.h>

#ifdef __STDC_NO_THREADS__
#error "This code needs C11 <threads.h> to compile"
#endif
#include <threads.h>
#include <stdio.h>
#include <assert.h>

//========= CODE ESCLAVE ============

// Le slave reçoit une demande GET [R] et retourne R*3.
// Les demandes SET échouent toujours.
int slave_receive(void* userdata, proto_Command_t command, proto_frame_data_t* args_inout) {
    UNUSED(userdata);

	switch(command) {
    case proto_CMD_GET:
        // les arguments de la trame de retour sont indiqués en écrivant dans args_inout
        args_inout->reg_value = args_inout->req.reg * 3;
        return 0; // 0 = OK, tout va bien
	default:
        return -1; // -1 = erreur, on n'attendait pas cette commande
	}
}

mtx_t slaveStopMtx;
int slaveStop = 0;

int slaveThreadFunc(void* data) {
    proto_hdle_t* hdle = data;
	
	while(1) { // tant que slaveStop != 0
		mtx_lock(&slaveStopMtx);
		int shouldStop = slaveStop;
		mtx_unlock(&slaveStopMtx);
		if (shouldStop)
			break;
        proto_slave_main(hdle);

		thrd_yield(); // l'OS peut nous faire attendre un peu
	}
	return 0;
}

///======= INITIALISATION ================

typedef struct Handles {
    proto_hdle_t* masterHdle;
    proto_hdle_t* slaveHdle;
} Handles_t;

int initialize(Handles_t* handles);
void destroy(Handles_t* handles);


int main() {
    Handles_t handles = {0};

    if (!initialize(&handles)) {
        perror("On n'a pas pu ouvrir les connexions.");
        return EXIT_FAILURE;
    }
	
	// Initialisation du contexte multi-threadé
	if (mtx_init(&slaveStopMtx, mtx_plain) == thrd_error) {
        perror("Erreur en créant un mutex");
        destroy(&handles);
		return EXIT_FAILURE;
	}

	// Lancement du thread de l'esclave
	thrd_t slaveThreadId;
    if (thrd_create(&slaveThreadId, slaveThreadFunc, handles.slaveHdle)) {
		perror("Erreur en créant le thread");
        mtx_destroy(&slaveStopMtx);
        destroy(&handles);
		return EXIT_FAILURE;
	}
	
	//============ CODE MASTER ============
	
	// Initialisation du maître
	uint8_t output = 0;

    // on veut écrire 10 dans le registre 5
    proto_Status_t status = proto_master_set(handles.masterHdle, 5, 10);

	// On a normalement reçu une erreur : l'esclave utilisé dans ce
	// programme n'accepte pas les commandes SET !
    assert(status == proto_INVALID_ARG);
	
	// on veut "lire le registre" 28
    status = proto_master_get(handles.masterHdle, 28, &output);
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

    destroy(&handles);

	return EXIT_SUCCESS;
}



typedef struct Devices {
    proto_Device_t masterDev, slaveDev;
} Devices_t;

/// \return 1 si OK, 0 si erreur
int initializeDevices(Devices_t* devices) {
    proto_Device_t masterDev = devserial_create();
    proto_Device_t slaveDev = devserial_create();

    if (masterDev->open(masterDev, "/dev/ptmx") == 0) {
        int masterFD = devserial_getFD(masterDev);
        if (grantpt(masterFD) >= 0 && unlockpt(masterFD) >= 0) {
            if (slaveDev->open(slaveDev, ptsname(masterFD)) == 0) {
                // les deux devices ont été ouverts
                devices->masterDev = masterDev;
                devices->slaveDev = slaveDev;
                return 1;
            }
        }
        masterDev->destroy(masterDev);
    }
    return 0;
}

/// fonction d'initialisation (pour encapsuler la gestion d'erreur)
int initialize(Handles_t* handles) {
    Devices_t devices = {0};
    if (!initializeDevices(&devices))
        return 0;

    proto_hdle_t* masterHdle = NULL;
    proto_hdle_t* slaveHdle = NULL;

    if ((masterHdle = proto_master_create(devices.masterDev))) {
        if ((slaveHdle = proto_slave_create(devices.slaveDev, &slave_receive, NULL))) {

            handles->masterHdle = masterHdle;
            handles->slaveHdle = slaveHdle;
            return 1;

        }
        proto_master_destroy(masterHdle);
    } else {
        // si proto_master_destroy a été appelé, l'objet est déjà détruit !
        devices.masterDev->destroy(devices.masterDev);
    }
    devices.slaveDev->destroy(devices.slaveDev);
    return 0;
}

void destroy(Handles_t* handles) {
    proto_master_destroy(handles->masterHdle);
    proto_slave_destroy(handles->slaveHdle);
}

