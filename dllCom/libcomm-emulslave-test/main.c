#include "protocomm_master.h"
#include "device_emulslave.h"

// Test du protocole master/slave avec le device emulslave

#include <assert.h>
#include <string.h>
#include <stdio.h>

// Structure de test pour ce qui se passe côté master
typedef struct master_Data_t {
    proto_hdle_t * proto; ///< Instance du protocole
	uint8_t lastCommand;
	uint8_t lastArgs[proto_MAX_ARGS];
	uint8_t nbReceived;
    uint8_t * slaveRegs; /// Acces au registre du slave (pour contrôle et initialisation)
} master_Data_t;

void master_callback(void* userdata, proto_Command_t command, uint8_t const* args) {
	master_Data_t* data = userdata;
	++data->nbReceived;
	data->lastCommand = command;
	memcpy(data->lastArgs, args, proto_getArgsSize(command));
}

proto_Device_t device;
master_Data_t _masterdata = { 0 };
uint8_t args[proto_MAX_ARGS] = { 0 };

///
/// \brief test_set_NO_ERROR test nominal des écritures sur le slave
///
void test_set_NO_ERROR() {
    int regno = 5;
    uint8_t regvalue = 167;

    /* TODO On va iterer sur tous les registres que l'on a, a chaque fois :
     * on verifier la mise à jour du registre
     * on vérifie que les autres registres ne sont pas impactés
    */

    memset(_masterdata.slaveRegs, 0, EMULSLAVE_NB_REGS * sizeof(*_masterdata.slaveRegs) );
	// On fait une demande d'écriture
    proto_Status_t status = proto_master_set(_masterdata.proto, regno, regvalue);
	// PS : pas besoin de timeout car c'est instantané avec l'émulation
	assert(status == proto_NO_ERROR);
    assert(_masterdata.slaveRegs[regno] == regvalue);
}

void test_set_INVALID_REGISTER() {

    int regno = EMULSLAVE_NB_REGS;
    uint8_t regvalue = 167;

    // On va écrire sur un registre inexistant
    proto_Status_t status = proto_master_set(_masterdata.proto, regno, regvalue);
    assert(status == proto_INVALID_ARG);
}

void test_get_NO_ERROR() {

    int regno = 18;
    uint8_t regvalue = 0;

    // Positionner le registre à une valeur non nulle
    _masterdata.slaveRegs[regno] = 234;
    proto_Status_t status = proto_master_get(_masterdata.proto, regno, &regvalue);
	assert(status == proto_NO_ERROR);
    assert(regvalue == _masterdata.slaveRegs[regno]);
}

void test_get_INVALID_REGISTER() {

    int regno = EMULSLAVE_NB_REGS;
    uint8_t regvalue = 0;

    proto_Status_t status = proto_master_get(_masterdata.proto, regno, &regvalue);
    assert(status == proto_INVALID_ARG);
    assert(regvalue == 0); // en cas d'erreur, la sortie n'est pas modifiée
}

void test_crc() {
    int regno = 18;
    uint8_t regvalue = 0;
    devemulslave_setFlags(_masterdata.proto->priv_iodevice, EMULSLAVE_FLAG_MASTER_BADCRC);
    proto_Status_t status = proto_master_set(_masterdata.proto, regno, regvalue);
    assert(status == proto_ERR_CRC);
}


int main() {
	
    // Initialisation du protocole avec le device emulslave
    proto_Device_t devslave = devemulslave_create();
    _masterdata.proto = proto_master_create(devslave);
    assert(_masterdata.proto);
    _masterdata.slaveRegs = devemulslave_getRegisters(devslave);

	test_set_NO_ERROR();
	
	test_set_INVALID_REGISTER();
	
	test_get_NO_ERROR();
	
	test_get_INVALID_REGISTER();
	
	test_crc();
	
	puts("Tous les tests ont réussi !");
	
	
    proto_close(_masterdata.proto);
    proto_destroy(_masterdata.proto);
	
	return 0;
}
