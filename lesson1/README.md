Clase 1 de revisión de código
=============================

##Plataforma
----------
Microcontrolador CC1110 (core con la arquitectura 8051) de Texas Instrument:
- 32 KB de Flash
- 4 KB de RAM
- 13-26 MHz
- Interfaz radio...


##Aplicación
----------
En este ejemplo se pretende entender el buen uso de las directivas de precompilador
`#ifdef-#ifndef-#endif`. Se aplicará a un código que se usa ahora mismo para dos placas
con HW distinto; en este momento se compila todo el código independientemente de la
placa en al que vaya a ser cargado.

Además del fichero adjunto, hay otros que también usan partes iguales de código, como 
leer valor de batería, leen de los ADC  o checkear una transmisión (y otros). Se pretende 
hacer un fichero común con dichos códigos convertidos en funciones que serán llamadas.

Cualquier otro error, recomendación o comentario al margen también es objetivo de estas
"clases de C".

