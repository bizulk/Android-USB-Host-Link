 /* example.i */
 %module protocomm
 %{
	#include "protocomm_master.h"
 %}

 /* utiliser les fichiers d'interface pour les standard sinon on a des types opaques
https://stackoverflow.com/questions/10476483/how-to-generate-a-cross-platform-interface-with-swig
*/
%include "stdint.i"

/* Le fichier d'entete intermédiaire sert de substitution à l'interface, SWIG étant capable de parser un fichier
	comme les CFFI/Python, avec les mêmes problèmes d'ailleurs, donc ce fichier est le même pour les deux outils.
*/
%include "protocomm_ll.h"
%include "libcomm_global.h"
