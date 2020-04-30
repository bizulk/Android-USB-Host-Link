Ici se trouvent les codes source sur la partie dllComm en C.

SLI 
	OK je vois bien qu'il y a des instructions de compilation, mais moi je travaille sous windows,
	OK je pourrais tester les WSL pour compiler, Mais cela serait plus satisfaisant d'avoir une gestion de projet : 
		> il faut générer une lib,
		> Les applis de test se link dessus.
		> il faut un profil debug/release, vous ne maîtrisez pas actuellement votre profil de compilation. A la livraison tout sera testé en Release (ie avec les optimisations activées).
		
	Générer un projet de lib sous QtCreator c'est assez rapide, et proposera une abstraction par rapport au toolkit utilisé. Une souplesse par rapport aux fichier à inclure ou pas.
	Prenez le toolkit Qt mingw qui vous convient sur l'installeur Qt (je peux vous passer l'installeur en ligne)
SLI END

	ATTENTION ! Il ne s'agit de faire une lib qui correspond à ce que CIO dispose, même s'il existe de meilleurs implémentation.
	L'implémentation sous-jacente peut présenter un découplage fort comme actuellement fait mais il me faut une implémentation d'une lib côté master qui correspond à ce que j'ai :
	ouverture d'un port : proto_cio_master_open( "/dev/ttyS0" )
	envoie d'une demande avec récupération de la réponse : 
		- proto_cio_master_write( args ) (ret : OK ou error)
		- proto_cio_master_read( args ) (ret : OK, ERROR, dans les deux cas avec la données réponse, il y a eu l'échange de trames)
		- proto_cio_master_close()
	
### test-ptmx
Petit exemple pour tester l'utilisation d'un pseudo-terminal sous Linux.

Compilation puis exécution (sous Linux !) :
```
gcc test-ptmx/*.c -o out
./out
```
Résultat attendu (après une seconde d'attente) :
```
Fd ouverts
Read: 'Hello, World!'
```

### libcomm
Bibliothèque implémentant le protocole de communication, ainsi que
des implémentations de IO Device (par exemple émulation d'un esclave
distant sans vraie liaison série, ou implémentation sous GNU/Linux
avec les File Descriptor).

Le moyen le plus facile d'utiliser la bibliothèque est de copier les
fichiers qui nous intéressent.
- `libcomm.*` pour la bibliothèque sans devices
- `libcomm-emulslave.*` pour avoir un device émulant un esclave
- `libcomm-gnulinux.*` pour avoir un device relié à un fileDescriptor sous GNU/Linux

### libcomm-emulslave-test

Compilation puis exécution (sous Linux) :
```
gcc libcomm/libcomm.c libcomm/libcomm-emulslave.c -Ilibcomm libcomm-emulslave-test/main.c -o out
./out
```
Résultat attendu :
```
Tous les tests ont réussi !
```

### libcomm-gnulinux-test

Ce test ne fonctionne que sous GNU/Linux avec un pseudo-terminal.
Il faut également avoir un compilateur gérant la norme C11 ainsi que la bibliothèque standard des threads `<threads.h>`.

Compilation puis exécution :
```
gcc libcomm/libcomm.c libcomm/libcomm-gnulinux.c -Ilibcomm libcomm-gnulinux-test/main.c -o out -lpthread
./out
```
Résultat attendu :
```
Tout s'est bien passé !
```
