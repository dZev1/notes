# Memoria Cache

## Características

### ¿Cuál es el tamaño ideal del banco de memoria?

Tiene que ser suficientemente grande para que el procesador pueda **resolver la mayor cantidad posible de búsquedas de código y datos** en esta memoria garantizando alta performance. Por otro lado, tiene que ser suficientemente pequeña para **no afectar ni el costo ni el consumo del sistema**.

### Definiciones

#### Hit

Acceso a un elemento, ya sea dato o código, que se encuentra en la memoria cache.
#### Miss

Acceso a un elemento que no se encuentra en la memoria cache

#### Hit Rate

$$
HitRate = \frac{\#Accesos \ con \ hit}{\# Accesos \ totales}
$$

Se espera que el hit rate sea lo más alto posible.

#### Latency

Tiempo para traer desde la CPU la primera palabra de la línea

#### Ancho de banda
Determina el tiempo adicional para traer al cache el resto de las palabras que caben en esa línea

## Métricas

### ¿Cuánto nos cuesta un MISS?

Cuando el elemento requerido no se encuentra en el cache, el tiempo de recuperación del MISS no es fácil de determinar, pues depende de las características del sistema.

La cache se administra por bloques o líneas (es lo mismo bloque y línea), basado en el principio de localidad. Al requerirse un elemento, se traen la mayor cantidad de posible de vecinos. Los buses de datos entre CPU y cache son muy grandes, típicamente de 512 bits.

Si está en los niveles inferiores de memoria, el tiempo de recupero del elemento dependerá del **latency** y del **ancho de banda** de la memoria con que se haya implementado ese nivel de la jerarquía.

Si el procesador ejecuta instrucciones *EN ORDEN*, entonces se detiene hasta conseguir el elemento requerido. Si ejecuta *FUERA DE ORDEN*, puede seguir con otras instrucciones dejando en espera la que generó el MISS.

La memoria principal está administrada en páginas de tamaño fijo. Ahora, puede ocurrir que el elemento buscado podría no estar ubicado en una página alojada en la memoria principal, por lo que se detiene el SO junto a la CPU, se obtiene la página que lo contiene de la memoria virtual, se aloja esa página en la memoria principal, y desde allí se obtiene el mismo.

### Performance
#### Tiempo de ejecución de una CPU
$$
CPU_{ExcecTime}=(CPU_{ClockCycles}+Memory_{StallCycles}) \cdot Periodo_{Clock}
$$
Acá $CPU_{ClockCycles}$ incluye los ciclos de búsqueda en el cache si se trata de un HIT, y el procesador se atasca ante un MISS.

El tiempo de Memory Stall depende del número de MISS y el tiempo que cuesta cada MISS (MISS penalty).

$$
Memory_{StallCycles} = \#MISS \cdot MISS_{Penalty}
$$
$$
= \#Instrucciones \cdot \frac{MISS}{Instruccion} \cdot MISS_{Penalty}
$$
$$
= \#Instrucciones \cdot \frac{Memory Accesses}{Instruccion} \cdot MISS_{Rate}  \cdot MISS_{Penalty}
$$
Acá, la cantidad de instrucciones se puede medir, la cantidad de accesos a memoria por instrucción también y $MISS_{Rate} = 1 - HitRate$.

MISS Penalty no se mide, se estima con un promedio en base a experimentar con una cantidad de casos suficientemente grande como para ser representativa.

MISS rate y MISS penalty varían de forma notable para lecturas y escrituras.

## Hardware dedicado es más complejidad
![[Pasted image 20260107150355.png]]

El **Cache Controller** está entre el procesador y el bus del sistema. Las líneas del bus de control indican qué operación va a iniciar el procesador hacia el sistema. el bus de address hacia qué dirección hacerla, y por el bus de datos fluye la información.

El controlador Cache se convierte en el árbitro del bus, tomando el control de la lńea de control de la CPU. Este controlador luego se encarga de comunicarse con la cache, o con el sistema según corresponda. Por eso tenemos el Bus local del Controlador Cache.

El controlador cache tiene una interfaz con el Bus Local del procesador, una con el Procesador y una con el Control del Cache. Por último tenemos el Directorio Cache, que es una memoria de acceso por contenido

![[Pasted image 20260107151115.png]]

Cada vez que llega una dirección al controlador cache, ell directorio es el encargado de decir si hay un MISS o un HIT.

### Operacion de lectura

1. Llega un address al Cache Controller y se busca en el Directorio Cache.
2. Si se produce un HIT:
	- Se habilita la lectura de la memoria cache
	- Se utiliza el elemento en un clock o dos.
3. Si se produce un MISS:
	- Se habilita la lectura de la memoria DRAM
	- Se habilita la escritura en la memoria cache
	- Se actualiza el Directorio Cache
	- Se usa el elemento.