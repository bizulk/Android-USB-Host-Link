Ici se trouvent les codes source sur la partie dllComm en C.

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
