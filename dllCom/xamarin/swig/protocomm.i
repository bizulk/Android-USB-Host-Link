 /* example.i */
 %module protocomm
 %{
	#include "protocomm_master.h"
	#include "log.h"
	#include "devices/device_emulslave.h"
	#include "devices/device_serial.h"
 %}

 /* utiliser les fichiers d'interface pour les standard sinon on a des types opaques
https://stackoverflow.com/questions/10476483/how-to-generate-a-cross-platform-interface-with-swig
*/
%include "stdint.i"

/* Utiliser cpointer pour manipuler les pointeurs, lorsque qu'ils sont a manipuler directement 
*/
%include "cpointer.i"
%pointer_functions(uint8_t, uint8_t_p);

/* Le fichier d'entete intermédiaire sert de substitution à l'interface, SWIG étant capable de parser un fichier
	comme les CFFI/Python, avec les mêmes problèmes d'ailleurs, donc ce fichier est le même pour les deux outils.
*/
%include "libcomm_global.h"
%include "protocomm_master.h"
%include "protocomm_ll.h"
%include "device_emulslave.h"
%include "device_serial.h"
%include "log.h"
