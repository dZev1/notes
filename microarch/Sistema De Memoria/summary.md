# Summary
## Importancia de la memoria

Para entender un poco que es lo que sucede con la memoria, y por qué es tan importante, podemos ir al caso de las prospectos dados por la empresa DEC con respecto a la performance de los procesadores.

En él, se mencionaba que íbamos a crecer exponencialmente en el número de cores de un procesador, la cantidad de instrucciones que se ejecutasen en un determinado tiempo y también que aumentaríamos considerablemente la frecuencia del clock.

Obviamente, esto no fue así. Si bien se había aumentado la frecuencia del clock y la cantidad de instrucciones por ciclo de clock, dejando atrás el aumento de cores, la performance entre un modelo más viejo y la novedad de aquel entonces era casi indistinguible. ¿Qué ocurría? LA MEMORIA. El acceso a memoria es lento.

Entonces la memoria resultaba un cap con respecto a la performance del procesador, pues por más rápido que vaya el procesador, si acceder a memoria es lento y todas las operaciones que se hacen en su mayoría son a memoria, produce que tengamos un techo... O no.

## ¿Por qué tratamos a la memoria como un sistema?

Hablamos de sistema de memoria porque no está formada por una única tecnología que maneje todo, sino que tenemos que combinar varias, pues no podemos solucionar con una sola tecnología los requerimientos que queremos solventar:

- costo bajo
- gran velocidad de acceso
- bajo consumo
- gran capacidad de almacenamiento
- no pérdida de datos tras el apagado

Entonces se emplean varias tecnologías, y se las organiza en un esquema jerárquico de tipo piramidal, donde en la cúspide de la pirámide se encuentran aquellas que son más rápidas, pero tienen menor capacidad y mayor costo por bit, véase DRAM y SRAM (Cache L1 y L2), registros, etcétera; y en la base, aquellas que son más lentas en cuanto a acceso, pero tienen una mayor capacidad de almacenamiento y un menor costo por bit, como por ejemplo un disco de estado sólido.

## ¿Qué tecnologías tenemos?

- **Memorias no volátiles**
	- En su momento eran Read-Only, de allí ROM.
	- Son de acceso lento.
	- Mantienen su información luego del apagado del sistema.
	- Hoy en día se pueden reescribir *on the fly*.
	- En un computador, es la que posee el código de arranque del sistema.
- **Memoria volátiles**
	- Son de acceso rápido.
	- Pierde su información almacenada una vez se desconecte la alimentación del sistema.
	- Una vez encendido el sistema, se copia el código de arranque a esta memoria, pues es más rápida que la ROM.
	- Hay dos tipos de memorias volátiles:
		- **DRAM**
			- Es menos costosa económicamente hablando
			- Consume menos energía
			- Acceso más lento
				- Necesita refrescar los datos
				- Una vez leído su contenido se tiene que volver a escribir, pues se pierde la carga del capacitor que la compone.
		- **SRAM**
			- Acceso más rápido
			- Consume más energía
				- Está compuesta por seis transistores, de los cuales tres están saturados
				- Es un Latch
			- Es más costosa

## ¿Qué es el principio de localidad?

Es un principio el cual nos dice que si un programa sigue las buenas prácticas de la programación, entonces es muy probable que, en un lapso de tiempo acotado, usemos las mismas direcciones de memoria y sus direcciones vecinas.

Esto nos habla de dos tipos de localidad:

- **Localidad Temporal**: Si accedimos a una dirección de memoria, es muy probable que en el tiempo cercano volvamos a acceder a ella.
	- Un ejemplo son las variables en un programa.
	
- **Localidad Espacial**: Si accedimos a una dirección de memoria, es muy probable que accedamos a sus direcciones vecinas.
	- Por ejemplo, acceder a los elementos de un array.

## Organización Cache

- La memoria cache está compuesta por un bloque de memoria SRAM.
- Este bloque tiene que ser lo suficientemente grande como para guardar una buena cantidad de líneas y retenerlas por una cantidad considerable de tiempo, pero también tiene que ser suficientemente pequeña como para que no aumente considerablemente el costo del procesador.
- Es utilizada para copiar datos de la memoria principal y sus direcciones de memoria para que el procesador tenga un rápido acceso a los mismos, reduciendo el retardo que produce ir hasta la memoria principal a fetchear el dato.
- Se basa en el principio de localidad.
- Se agrega esta estructura entre el buffer que comunica al bus y la CPU.
- También se agrega un árbitro que se comunica con el bus y con la memoria cache, en caso de que la cache no posea un dato, lo busca en la memoria principal. Este árbitro es el **Controlador Cache**
- Recibe de la CPU la línea de Address y de control. La línea de Address también va al bus del sistema, pero la de control no. Es el Controlador el que decide si llevarla al bus o no.
- Además se agrega un **Cache Directory**, que recuerda mucho al esquema de paginación usado en el TP.
	- En él, cada entrada tiene un TAG y un bit de validez. Esto permite conocer si un dato está presente o no en la memoria cache rápidamente.
- Las **líneas** son la unidad mínima que maneja una memoria cache. Son un conjunto de bytes de la memoria principal.
- La memoria cache interpreta a la memoria principal como una cierta cantidad de páginas de una cierta cantidad de líneas.
- Hay que definir cuatro funciones fundamentales para tener un cache:
	- **Line Placement**: se utiliza un mapping por sets. Si son dos sets, se llama 2-way-set-associative. Hay dos casos particulares, el 1-way-set-associative es un set para cada línea, esto es lo que llamamos Fully Associative y, en caso de que haya un set por línea en el cache, tenemos lo que se llama Direct Mapping. Para obtener en qué set colocamos la línea, usamos el resto de dividir al address físico de una línea por la cantidad de sets en la cache. Es decir, ($Physical Address \ mod \  \#CacheSets$ ). Luego se utilizan los bits intermedios del Cache Directory con el tag correspondiente para determinar en qué línea del set ubicar la línea (es un índice). Por último, los últimos bits nos dan el offset del dato buscado.
	- **Line Identification**: Una vez que nos llega un address, vemos si el tag del address está en el directorio, si está, vamos a llevar el tag a la memoria cache. Allí, comparamos en paralelo con los bancos de memoria (índice del set), para ver cuál es el tag correcto. Cabe aclarar que dentro del banco, también se tiene un bit de validez de tag, y la comparación se hace solo si el tag es válido en el banco. Si el tag está, obtenemos su dato.
	- **Line Replacement**: 