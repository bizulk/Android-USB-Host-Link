#include "device_stm32.h"

// On teste juste le device ici
#include <assert.h>
#include <string.h>
#include <stdio.h>

unsigned char BYTES[] = "Hello World!";
int main() {
    char buffer[100];
    int nbRead;

    proto_Device_t device = devstm32_create();

    device->open(device, "");

    nbRead = device->read(device, buffer, sizeof(buffer), /* timeout_ms = */ 0);
    assert(nbRead == 0); // on n'a du rien lire vu qu'aucune donnée n'était disponible

    devstm32_pushBytes(device, BYTES, 3); // "Hel"
    devstm32_pushBytes(device, BYTES+3, 5); // "lo Wo"
    devstm32_pushBytes(device, BYTES+8, 4); // "rld!"
    // on a donc push 12 octets dans le device

    nbRead = device->read(device, buffer, 5, 0); // on lit au plus 5 caractères
    assert(nbRead == 5);

    assert(memcmp(buffer, "Hello", 5) == 0);

    // on a lu 5 caractères, il reste donc 7 caractères dans le device
    nbRead = device->read(device, buffer, sizeof(buffer), 0); // on lit au plus 100 caractères
    assert(nbRead == 7);
    assert(memcmp(buffer, " World!", 7) == 0);

    // si on essaye de lire, on obtient rien
    nbRead = device->read(device, buffer, sizeof(buffer), /* timeout_ms = */ 0);
    assert(nbRead == 0); // on n'a du rien lire vu qu'aucune donnée n'était disponible

    // on a fini de se servir du device, on le détruit
    device->destroy(device);

    puts("Les tests ont réussi !");
}
