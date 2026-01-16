# Sistema de Memoria

## Rol de la memoria

Richard Sites escribe un paper titulado _It's the Memory, Stupid!_, donde hacen una prospección que hizo DEC al lanzar el procesador _Alfa 21064_. 

Ahí estiman que a veinticinco años 1992, es decir 2017, la performance del procesador va a aumentar 1000 veces. También que la frecuencia del clock, la cantidad de instrucciones enviadas a ejecución en un ciclo de clock y el número de cores crecería 10 veces.

Cuatro años más tarde, en 1996, checkearon cómo venía la prospección. Se esperaba que las tres magnitudes enumeradas arriba deberían haberse aumentado en x1.45. En la realidad, aumentaron en un 2.5 la frecuencia de clock y el número de cinstrucciones envíadas, solo la cantidad de cores quedó atrás, pues seguían teniendo uno solo.

Sin embargo, aún teniendo todo esto, la performance era similar a su predecesor. Los procesadores estaban tres ciclos de clock esperando a la memoria.

## Jerarquía de memoria

La memoria no es un componente, es un subsistema. Lo ideal sería tener un sistema de memoria de una sola tecnología y sin jerarquía. Con un diseño simple y costo moderado.

En la realidad, esto solo es posible en sistemas triviales basados en microcontroladores para actividades de control rudimentarias. En un sistema de cómputo un poco más sofisticado, si o si debemos mirar a la memoria como un subsistema, llegando a un punto tal en que es más importante el diseño del subsistema de memoria que el de la propia CPU.

Así fueron aflorando problemas en el diseño del subsistema de memoria que no eran relevantes, entre ellos la integridad de señal entre los distintos tipos y niveles jerárquicos de memoria y CPU.

### **Requerimientos**

- Gran capacidad de almacenamiento.
- Tiempo de acceso mínimo.
- Capacidad de mantener los datos cuando se apaga el equipo.
- BAJO COSTO

Esto es imposible con una sola tecnología y sin organización jerárquica.

### Construcción de la jerarquía

Queremos dar más memoria que la RAM instalada en el sistema. Para ello lo que no entra en la ram, lo ponemos en el storage y luego ir intercambiando entre ellos, según se necesite.

El acceso a la memoria es predecible, no aleatorio.

### Principio de localidad

Si tomamos un programa que respete las buenas prácticas de programación, los accesos a memoria quedan restringidos, durante un intervalo de tiempo significativo, a un mismo intervalo de direcciones de memoria contiguas y acotadas. Es decir, *los programas tieneden a reutilizar datos e instrucciones que han utilizado recientemente*.

#### Localidad Temporal

Se refiere a un patrón de acceso a las mismas direcciones de memoria en un intervalo temporal **finito** y **acotado**.

La probabilidad que la CPU solicite, en los próximos ciclos de acceso a memoria, las mismas direcciones de memoria que está utilizando actualmente, es muy alta.

En cambio, es baja la probabilidad de iniciar un ciclo de memoria hacia direcciones que no están siendo utilizadas.

#### Localidad Espacial

Se refiere al espacio de direcciones que utiliiza una CPU en un intervalo temporal finito y acotado.

La probabilidad que la CPU use direcciones vecinas a la del objeto que está direccionando actualmente es muy alta.

En cambio, es muy poco probable que utilice direcciones de memoria lejanas a las actuales.

### Jerarquía

La CPU posee una jerarquía piramidal, donde la capa más alta es la que posee una mayor velocidad y **costo por bit** (pues tenemos mayor cantidad de transistores, entonces mayor consumo, etc), pero tiene menor capacidad, y la más baja es la que posee más capacidad pero es más lenta y tiene un menor costo por bit.

Se organiza de la siguiente manera:

- Registros
- Cache L1
- Cache L2
- DRAM
- Storage
- Backup Offline

![[Pasted image 20260106140409.png]]

Mientras más lejos nos movamos del procesador, la memoria en el nivel siguiente se vuelve más lenta pero más grande.
