# Notas DRAM

## Consideraciones

- Tener en cuenta a la hora de diseñar DRAMS:
	- Pines
		- Las entradas de los circuitos lógicos reflejan inductancia, capacitancia, etc, estos son parámetros que tenemos que considerar, pues sino podemos tener problemas de integridad de señal.
	- Señalización
		- Implementar un mediador entre la DRAM y la CPU. Necesitamos un controlador de memoria dinámica. Normalmente cada core ya tiene implementado uno. Hay entonces un Handshake entre la memoria y el controlador.
	- Integridad de Señal
		- Mantener la integridad de señal con respecto a la frecuencia de trabajo de la CPU.
	- Encapsulado
		- El encapsulado define la manufacturabilidad del dispositivo. Cómo proveo un chip de gran densidad electrónica, con una cantidad considerable de terminales que estén separados para que sea soldable. Qué tipo de montaje tiene, VGA o Superficial. Etcétera.
	- Clock y Sincronización
		- Al estar fuera del CPU, se tienen que definir criterios de montaje en el PCB y adaptaciones al bus, para así mantener la sincronización con el resto del sistema.
	- Timing
		- Recordemos que estas memorias hay que refrescarlas, necesitamos saber cuánto tardamos en leer, cuánto en escribir, cosas que debe saber el controlador de memoria
- Estos chips van a un PCB llamado **DIMM**

## **Controlador de Memoria Dinámica (North Bridge para Intel)**

- Media entre la CPU y los chips DRAM.
- Sugiere demoras de acceso, que se compensa con funciones y valor agregado que incluye el controlador.
- No es el gran responsable de la demora.
- Sugiere complejidad en el sistema.
- Aporta independencia a la CPU de detalles de DRAM.

## **Celdas**

- Bit = Transistor que actúa como capacitor.
	- Cuando leo destruyo la información contenida en esa celda.
	- Cada vez que leo, tengo que volver a escribir.
	- Esto trae más retardo para el inicio del próximo ciclo de lectura.
	- El leakage, hacen que el capacitor pierda carga solo, sin hacer nada.
	- Se deben refrescar periódicamente para no perder la información almacenada.
- Los conductores que transportan la señal, deben estar precargados en un estado **entre** 0 y 1.
	- La carga de la capacidad de entrada del transistor hace que se desbalancee el estado de esa carga en + o en -.
	- Usando amplificadores, podemos empujar la carga, habiendo detectado el desbalance de la celda, a su estado lógico inicial.

## Organización

- Matriz rectangular de $N \times M$
	- Originalmente $N$ y $M$ debían ser iguales o con una unidad de diferencia. Esto porque ambos en un multiplexor, y de allí se seleccionaba en primer lugar la fila y en segundo la columna, y de ahí se referenciaba un bit. No se desbalanceaba el circuito por la proximidad numérica de N y M.
	- La cantidad de terminales se reduce a la mitad.
- Además de los pines de address, había que agregar señales de **handshake**, **RAS** (Row Address Strobe) y **CAS**(Column Address Strobe).
	- RAS selecciona la fila a leer
	- CAS los elementos de esa fila que se copian al buffer de lectura.
- Tenemos dos decodificadores, uno de columna y uno de fila. En la intersección de las líneas decodificadas, está el transistor que almacena el bit que buscamos.
	- A la línea de la columna se la llama **bit line** y la de fila **word line**.
	- Con la decodificación de una determinada bit line y word line, polarizamos el transistor y así, se entrega el valor de la capacidad por la **bit line**.
- Cada chip de DRAM contiene varios arrays de $N \times M$ celdas, para así poder extraer bytes.
- Los arrays pueden trabajar completamente asociados, disociados o en configuraciones intermedias.
	- Si trabajan de manera asociada, todos los arrays reciben o transmiten el bit correspondiente al valor de fila y columna, proveyendo acceso a un número de tantos bit como arrays hay en el chip.
	- Una x4 DRAM entrega 4 bits por cada valor de fila-columna.

## Bancos DRAM

- Set de arrays con algunas restricciones.
	- Se activan, se precargan, se leen, se escriben, ..., al mismo tiempo que otros bancos, ya sea en el mismo chip, o distinto chip.
- Usar bancos independientes aumenta la velocidad de acceso a la DRAM. Si accedo a 2 bancos entrelazados, es el doble de rápido de acceso que acceder a uno simple.
	- Suponiendo por ejemplo, que un banco tiene tiempo de acceso de 10ns, leer entrelazado reduce a 5ns.
		- Si se distribuye equitativamente el tiempo (Round-Robin), con 4 bancos, se puede lograr en 2,5ns.
- Esto se obtiene con el Controlador de Memoria.
## DIMM

- Significa **Dual In Module Memory**.
- La DRAM se monta a ambos lados del PCB.
- Cada DIMM es un banco independiente o, cada dispositivo o grupo de dispositivos DRAM de un DIMM se puede asignar a un banco independiente.
- **Organización**
	- *RANK*: es un conjunto de dispositivos DRAM, ya sean, todos los de un DIMM o una parte de ellos, que operan de manera conjunta.
	- Cada banco tiene un conjunto de arrays de memoria.
	- Estructura jerárquica de memoria. Toda esta complejidad está manejada por el Controlador de Memoria.
	- **Concurrencia = Paralelismo = Ancho de banda**
		- Accesos concurrentes a bancos y rangos con un *request pipeline* aumenta el ancho de banda, pues operan en paralelo múltiples DRAM a nivel de rango y múltiples arrays a nivel de banco.
		- Acceso multi-fase: se divide a la ram dinámica en páginas. Para cada columna aumentar el row address.
	- Dentro del dispositivo, cada capacitor se ubica en la intersección entre la bit line y la word line. Se conecta la a la bit line controlado por la word line.
		- La bit line está precargada, la word line la polariza.
		- La word line que recibe un nivel de tensión alto, satura todos los transistores que tengan su gate conectado.
		- Para que la bit line detecte estos valores, se utiliza un amplificador de sensibilidad conectado a la bit line.
			- El mismo precarga las bit lines con un valor entre `0` y `1`.
			- Cuando se habilita la word line, se produce una alteración muy pequeña en el valor precargado en + o en - depende la carga del capacitor.
			- Esta pequeña alteración, es detectada por el amplificador y lleva la bit line a un `0` o a un `1` según corresponda.
			- Este pull por parte del amplificador, también restituye el valor de carga de los capacitores, cuya carga se había perdido al modificar la tensión precargada en cada bit line.
	- **Acceso**
		- Con la dirección física recibida, el Memory Controller decodifica el rango, el banco dentro del rango, la fila y la columna correspondiente en el banco.
			- Con el banco genera un grupo de bits adicionales a la dirección.
			- Con el rango genera el valor a colocar en el bus de chip select
		- Cuando recibe la información de direcciones, el dispositivo DRAM precarga las bit lines del banco seleccionado.
		- El chip select selecciona los dispositivos DRAM adecuados, de acuerdo a la información del bus, y se activa la fila seleccionada con la **row address**, activando RAS del DRAM device.
		- El DRAM selecciona la fila entera, las cuales atacan su correspondiente amplificador de sensibilidad, el cual detecta el diferencial de carga, trayendo el bit correspondiente y recargando los capacitores de la fila.
		- Luego, se activa CAS, para traer de los amplificadores seleccionados los bits necesarios para escribir en el Row Buffer.
		- El Controlador de Memoria recibe los datos y los transmite al procesador.

## Standards

- **Synchronous DRAM (SDRAM)**
	- Sincronizar permite aplicar CAS y RAS al mismo tiempo.
		- Disponemos de la capacidad de poner latches internos que aseguran la información que se aplica a la matriz
	- Es más predecible que las asincrónicas. Ya no hay accesos independientes del dispositivo.
	- Minimización del tiempo de conmutación.
	- Burst Mode por **BEDO**.
		- Registro especial para programar el número de accesos burst.
		- Evita enviar un CAS por cada lectura, liberando el ancho de banda de bus dedicado a handshakes.
		- Aumenta el porcentaje dedicado a transferir datos.
		- Producto del principio de localidad.
	- Dual Data Rate (**DDR**)
		- Los datos se validan en ambos flancos del clock (por ser sincrónicas).
		- Idénticas a las SDRAM tradicionales, pero con el doble de velocidad de transferencia.
		- Durante las escrituras, no se usa el clock, sino que se utiliza una señal llamada **DQS**. En sus flancos ascendentes y descendentes se escriben los datos.

- **Standard JEDEC**
	- Especificación internacional para las DRAM o DIMMs.
	- Define la interfaz entre el controlador de memoria y los dispositivos RAM.
		- Define que la interacción sea sobre cuatro buses independientes: **data**, **address**, **control**. **chip select**.
			- El bus de address y control se unifica en el **address & commands** bus.
			- **chip select** permite que el controlador de memoria sepa a qué DIMMs habilitar para volcar o buscar los datos.
	- **data**
		- Se define el ancho de banda de 64 bits.
		- En sistemas de alto rendimiento se llega hasta 256 bits. Esto es Standard **HBM** (High Bandwidth Memory).
			- Hasta 4 vías de 256 bits.
			- Este aumento permite leer muchos más datos en la misma velocidad.
	- **address & commands**
		- **address**
			- Envía información de fila y columna  a los DIMMs.
			- Ancho proporcional a la capacidad de DRAM.
		- **commands**
			- Lleva los strobes de fila y columna, con sus señales de control asociadas.
	- **chip select**
		- Individual para cada rango. 2 rangos por DIMM implica dos `chip select` bus por cada DIMM.

## **Controlador de Memoria**

- **Contexto**
	- Un procesador puede conectarse a uno o más controladores de memoria. También varios procesadores pueden usar un único controlador.
	- Las redes de conexión entre el procesador y el Controlador de Memoria, son diferentes a las que conectan el controlador con la DRAM.
	- Así se independiza el procesador de las características físicas de los dispositivos DRAM.
		- Controla varios dispositivos DRAM.
- **¿Por qué?**
	- Maneja la transferencia de datos hacia y desde la memoria, asegurando la compatibilidad con el protocolo.
	- Independiza al procesador de las características eléctricas de los dispositivos DRAM que forman al sistema de memoria.
	- Desentiende al procesador de los requerimientos de timing y de acceso.
	- Arbitra el acceso a un único sistema de memoria por parte de múltiples cores de procesamiento.
- **Arquitectura**
	- Elementos
		- Tiene conexión a cada CPU y a dispositivos de I/O. Los I/O no es necesario que se comuniquen al procesador.
		- El árbitro tiene un buffer de transacciones que encola en una pool los requests provistos del CPU o el IO.
			- Traduce la dirección provista por la CPU/IO y las encola por cada banco.
		- Se hace un scheduling de los comandos, para luego una vez hechos, se devuelven al que lo pidió.
		- Una interfaz de señalización que envía los comandos a los DIMMs.
	- Los costos en acceso a memoria aumentan relativamente los costos computacionales de muchos algoritmos.
	- Arbitrar los requerimientos es crítico para la performance del sistema de memoria.
	- Cuando una transacción gana la arbitración, se traduce la dirección física del bus del procesador a la dirección del bus del controlador de memoria, especificando FILA y COLUMNA.
		- Puede llegar a traducir una transacción en varios comandos a las RAM.
		- Buffering == IMPORTANTE pero aumenta el consumo. Está en permanente análisis de Trade-Ons y Trade-Offs.

## Configuración de DRAM devices

- DRAM devices son circuitos analógicos.
- Almacenar y recuperar datos en forma de carga en un capacitor con amplificadores de sensibilidad agrega retardo (en nano segundos no en ciclos de clock).
- Cada DRAM, según su diseño y fabricación tiene parámetros de timing distintos.
- Por esto la industria encontró una solución proveyendo una interfaz sincrónica.
- Se agregan registros de modo a los DRAM.

- **Registro de Modo**
	- CAS Latency
		- Retardo de un CAS.
		- De acuerdo al valor de este campo de bits, el dispositivo SDRAM retorna el dato dos o tres ciclos de clock luego de activado el CAS.
	- Tipo de Burst
		- Da el orden en el que se retornan los datos
			- Secuencial
			- Entrelazado
	- Longitud de Burst
		- 1, 2, 4 u 8 palabras
	- Los DDR SDRAM y los D-RDRAM tienen más registros de modo donde se pueden configurar más parámetros de control.

- Las SDRAM se clasifican según el número de bits contenidos en el dispositivo, pero se pueden organizar de otras maneras, según tamaño de palabra de datos.
	- Generalmente tienen el mismo número de bancos, filas y según disminuya la cantidad de columnas aumenta el ancho de banda del **data** bus.

- **Burst**
	- Podemos programar el tamaño del Burst como 1, 2, 4 u 8 columnas de datos a transferir como respuesta a un solo CAS.
	- En las SDRAM y DDR SDRAM, se necesita re ordenar los datos de tal forma que la columna requerida esté en primer lugar en la transferencia.
	- **Prebúsqueda de n-bits**
		- Cada vez que se envía un comando READ a un device SDRAM, la lógica de control determina duración y orden de burst de datos.
		- Cada columna se mueve en forma separada a través del bus de datos interno.
		- Controlar de manera separada cada columna limita la tasa de datos de operación.
		- En DDR SDRAM, se fueron moviendo más cantidad de bits en paralelo desde los amplificadores hasta el latch de lectura, y los datos se envían en un pipeline desde un multiplexor al bus externo de datos.
		- Basado en el principio de localidad, busca los primeros N bits y los siguientes, que tienen alta probabilidad de ser requeridos.
		- La velocidad se incrementa pero el chip no puede ser mayor de 4x.

## Integración con Cache

- Hay que evitar un cuello de botella.
- **Interfaz DRAM-LLC**
	- Rango de 8 chips
	- Cada chip tiene 8 matrices de bits (8x8), así se entrega 1 Byte.
	- Entre los 8 chips se compone una qword.
	- Esta qword es pasada al Controlador de Memoria que se comunica con el LLC.
	- Se usa el modo Burst de 8 golpes para llenar toda la línea.
	- Igualmente se hacen 8 lecturas para llenar la línea, pero sin el retardo que se haría si tuviesemos que pasar cada address, copiar el byte, repetir.