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

### libcomm-emulslave-test

Compilation puis exécution (sous Linux) :
```
gcc libcomm/*.c -Ilibcomm libcomm-emulslave-test/main.c -o out
./out
```
Résultat attendu :
```
Tous les tests ont réussi !
```