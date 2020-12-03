### libcomm
Bibliothèque implémentant le protocole de communication, ainsi que
des implémentations de IO Device (par exemple émulation d'un esclave
distant sans vraie liaison série, ou implémentation sous GNU/Linux
avec les File Descriptor).

Library implementing a simple communication protocol : 
- slave / master modèle
- operation are : read and write registers
- Registed are accessed using somewhat that could be considered an address.

The library embbeded the protocol implementation and device specific interface.
In the Qt project we targer a shared lib to embbed with some PC x86/64 Projects

### libcomm-emulslave-test

Unit-test project for testing protocol with emulslave device.
That device fake a real device, so that we can work without connecting the device.

### libcomm-gnulinux-test

Unit-test project for testing protocol using ptmx.

### test-ptmx

just an exemple of how to use ptmx

How to try it :

```
gcc test-ptmx/*.c -o out
./out
Fd ouverts
Read: 'Hello, World!'
```

### PCClient 

Qt GUI used to test the protocol

### strm32

The Atollic STM32 Project that embbed the libcomm as a static library

### xamarin

Xamarin binding the libcomm.

There are to sub-project : 
- the swig project, swig is a opensource binding project that wrap C code to C#
- the libcomm project is a Visual Studio project that embbed the libcomm library as a shared object for Android.
