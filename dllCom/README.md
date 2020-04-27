Ici se trouvent les codes source sur la partie dllCom en C.

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
Bibliothèque implémentant le protocole de communication, complètement
découplé des appels système qui accèdent à la liaison série physique.
