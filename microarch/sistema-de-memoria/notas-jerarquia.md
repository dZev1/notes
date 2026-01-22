# Notas Jerarquía de Memoria
## Importancia de la memoria

- Para entender un poco que es lo que sucede con la memoria, y por qué es tan importante, podemos ir al caso de las prospectos dados por la empresa DEC con respecto a la performance de los procesadores.

- En él, se mencionaba que íbamos a crecer exponencialmente en el número de cores de un procesador, la cantidad de instrucciones que se ejecutasen en un determinado tiempo y también que aumentaríamos considerablemente la frecuencia del clock.

- Obviamente, esto no fue así. Si bien se había aumentado la frecuencia del clock y la cantidad de instrucciones por ciclo de clock, dejando atrás el aumento de cores, la performance entre un modelo más viejo y la novedad de aquel entonces era casi indistinguible. ¿Qué ocurría? LA MEMORIA. El acceso a memoria es lento.

- Entonces la memoria resultaba un cap con respecto a la performance del procesador, pues por más rápido que vaya el procesador, si acceder a memoria es lento y todas las operaciones que se hacen en su mayoría son a memoria, produce que tengamos un techo... O no.

## ¿Por qué tratamos a la memoria como un sistema?

- Hablamos de sistema de memoria porque no está formada por una única tecnología que maneje todo, sino que tenemos que combinar varias, pues no podemos solucionar con una sola tecnología los requerimientos que queremos solventar:
	- costo bajo
	- gran velocidad de acceso
	- bajo consumo
	- gran capacidad de almacenamiento
	- no pérdida de datos tras el apagado

- Entonces se emplean varias tecnologías, y se las organiza en un esquema jerárquico de tipo piramidal, donde en la cúspide de la pirámide se encuentran aquellas que son más rápidas, pero tienen menor capacidad y mayor costo por bit, véase DRAM y SRAM (Cache L1 y L2), registros, etcétera; y en la base, aquellas que son más lentas en cuanto a acceso, pero tienen una mayor capacidad de almacenamiento y un menor costo por bit, como por ejemplo un disco de estado sólido.

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

- Es un principio el cual nos dice que si un programa sigue las buenas prácticas de la programación, entonces es muy probable que, en un lapso de tiempo acotado, usemos las mismas direcciones de memoria y sus direcciones vecinas.

- Dos tipos de localidad:
	- **Localidad Temporal**: Si accedimos a una dirección de memoria, es muy probable que en el tiempo cercano volvamos a acceder a ella.
	- Un ejemplo son las variables en un programa.
	- **Localidad Espacial**: Si accedimos a una dirección de memoria, es muy probable que accedamos a sus direcciones vecinas.
	- Por ejemplo, acceder a los elementos de un array.
