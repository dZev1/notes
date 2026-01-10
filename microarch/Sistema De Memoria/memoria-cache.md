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

## Organización

El Cache esta organizado en **líneas**, la mínima unidad de información que maneja un cache. Estas líneas representan un conjunto de bytes que están en alguna zona de la memoria principal (la dinámica).

Estos bloques de memoria están alineados al tamaño de una línea. Esto es por una cuestión de principio de localidad espacial y de eficiencia, pues se tarda lo mismo en leer un byte que una palabra del ancho del bus de datos y los bytes que se encuentran cercanos tienen alta probabilidad de ser requeridos de inmediato.

Cada vez que se fetchea un código de operación y hay un miss, el cache controller trae toda la línea donde se encuentra la dirección que produjo el miss.

Cada línea tiene un tag que corresponde a la parte alta de la dirección física donde se encuentra la línea. Contiguo al TAG, hay un bit de validez, que dice si una línea está libre u ocupada en el cache.

![[Pasted image 20260107231450.png]]
Esto es lo conocido como Cache Directory. Es muy simil a un Page Directory del esquema de paginación.

Se necesita para organizar el cache:
1. Una política para ubicar una línea de memoria principal en el cache (**Line Placement**).
2. Una vez ubicada la línea, necesita un mecanismo de identificación (**Line Identification**).
3. Un mecanismo de reemplazo de líneas existentes en respuesta a un MISS (**Line Replacement**).
4. Una estrategia para manejar escrituras (**Write Policies**).

### Line Placement: ¿Dónde va una línea en el cache?

#### Fully Associative

Este criterio consiste en que una vez es leída la línea desde la memoria principal, se coloca en cualquiera de las líneas libres del cache. Es lo menos restrictivo posible

- **Ventajas**:
	- Máxima flexibilidad para ubicar la línea.
	- Máximo Hit Rate.
	- Permite el uso de algoritmos de reemplazo variados.

- **Desventajas**
	- Requiere iterar a través de las lóneas hasta encontrar la primera libre (no determinístico) o emplear mucho hardware para buscar en paralelo para encontrar en tiempo de búsqueda de los demás métodos.
	- Aumenta complejidad y consumo

#### Direct Mapped

Este criterio es el más restrictivo. Consiste en que una línea de la memoria principal puede ubicarse en una única línea del cache, cuyo número es determinado por la siguiente expresión:
$(Line \ Frame \ Address) \ mod \ (Cache \# Lines)$

- **Ventajas**
	- Simplifica el placement y la búsqueda.
	- Minimiza el soporte de hardware y el consumo de energía.

- **Desventajas**
	- Peor Hit Rate. Dos líneas frecuentemente utilizada que mapeen sobe la misma línea de cache, generan desalojos constantes (**Conflict Miss**).

#### Set Associative

Un criterio intermedio, en donde hay un grupo de líneas en el cache en las que se puede guardar una nueva línea leída desde la memoria principal, que queda determinado por la siguiente expresión:
$(Line \ Frame \ Address) \ mod \ (Cache \# Sets)$

Si tenemos $n$ sets posibles para almacenar la línea, el sistema es $n$-way set associative.

- **Ventajas**
	- Mejor relación de compromiso entre los criterios para determinar el placement.
	- La búsqueda permite flexibilidad en el uso de distintos algoritmos.

- **Desventajas**
	- No aprovecha óptimamente la capacidad del cache.
	- Puede llegar a haber escenarios de Conflict Miss.
#### Frame address
se obtiene haciendo $\lfloor \frac{Address \ to \ access}{Line \ Size} \rfloor$

### Ejemplo

Un cache de 32 Kb se or ganiza en 1024 líneas de 32 Bytes. El Controlador Cache trata a la memoria principal como un conjunto de páginas del mismo tamaño del cache (32 Kb) organizadas en líneas.

El controlador ve entonces $2^{17}$ páginas de 32 Kb , 17 entonces es él tamaño del tag. Obteniendo los 17 bits más significativos de la dirección.

Ahora, si tomo al cache como 8-way set associative, cada bloque de memoria del sistema es de 4 Kb. Si el tamaño de línea se mantiene, cada vía ahora contiene 128 líneas. El espacio de direccionamiento de 4 Gb es tratado por el Controlador como una sucesión lineal de 2²⁷ líneas de 32 Bytes cada una.

El tamaño de bloque cambia de 1024 a 128 líneas. Entonces si cada bloque contiene menos líneas y el espacio de direccionamiento sigue siendo de 4 Gb, la cantidad de páginas en que se organiza la memoria aumenta, lógicamente, de 2¹⁷ a 2²⁰. Entonces el TAG ahora es de 20 bits. Se debe agregar también bits de control para manejar el Conflict Miss.

## Line Identification

El Controlador Cache lee la dirección física que la CPU pone en su Address Bus. Toma luego los bits más significativos de la dirección, las cuales indican el Tag.

En el Cache Directoy, se guarda un Tag asociado a la línea, el cual corresponde al número de página que contiene la línea presente en alguna de las vías del cache. El Controller entonces tiene que determinar si el tag está asociado a alguna de las líneas que componen el set.

Los bits menos significativos son el offset del dato direccionado por la CPU dentro de la línea. Los bits intermedios indican el índice a la línea dentro del cache (o de la página de memoria principal).

![[Pasted image 20260108133541.png]]

A partir de la dirección física obtenida del procesador, el Cache Controller obtiene el Tag y el Index de $t$ bits e $in$ bits respectivamente.

Veamoslo sobre un sistema 4-way set associative que cumple lo siguiente:
- $t$ = 19 bits
- $in$ = 8 bits
- $b$ = 5 bits
- $Line \ size$ = 32 Bytes
- $Cache \ size$ = 32 Kb
- $Address \ Space$ = 4 Gb

El índice es de 8 bits por lo que hay 256 posibilidades. Esto se debe a que hay 4 vías de conjuntos, y nuestro cache es de 32 Kb, entonces cada vía es de 8 Kb, es decir, tendrá 256 entradas.

Los bits de índice señalan el set. En este caso el set número $i$ se compone de las 4 $i$-ésimas líneas de cada una de las 4 vías. 

Una vez obtenido el set, se comparan en paralelo solo los tags **válidos** del Cache Directory con el obtenido de la dirección física.

## Line Replacement

Queremos ver cuál de las n vías desalojar cuando hay un Conflict Miss. Entonces surgen tres criterios:

### Random

Repartirmos la asignación de manera uniforme, seleccionando de manera aleatoria al candidato a ser desalojado.

### Least Recently Used (LRU)

Teniendo en cuenta que de acuerdo con la localidad, es probable reutilizar líneas recientemente accedidas, entonces la línea que es mejor candidata a desalojarse es la línea menos utilizada recientemente. Pero esto es costoso porque hay que agregar hardware.

### First In First Out (FIFO)

Simplificación de **LRU**. Se limita a marcar la línea ingresada hace más tiempo. También requiere más hardware.

### Sigamos

El criterio Random es fácil de construir en el hardware.

A medida que aumentamos la cantidad de líneas, LRU se vuelve cada vez más costoso y se termina aproximando con un Pseudo LRU. Esto se hace con un campo de bits para cada set del cache, donde cada bit corresponde a una vía. Cuando se accede a un set, se activa el bit correspondiente a la vía que contiene la línea que buscamos.

Como consecuencia de setear un bit, todos los bits de un set quedan activados y se resetean con la excepción del bit activado más recientemente.

Cuando se debe reemplazar una línea, la CPU elige aquella vía que tiene el bit reseteado. Si hay más de una opción, entonce se selecciona random.

La diferencia de cache misses cada 1000 instrucciones entre LRU y Random tiende a bajar cuando se aumenta el tamaño del cache. LRU se impone cuando se utilizan caches más pequeños, entonces se suele usar para Cache L1 y luego para los niveles más bajos, se utiliza Random.

## Write Policies

A la memoria se la accede en su mayoría para leer. Una CPU está haciendo un Fetch de instrucciones permanentemente. El fetch del opcode en si es un acceso de lectura.

De esas instrucciones entre un 25% y un 30% son lecturas de datos de memoria, y un 10%-15% son de escritura de datos de memoria. El caso más frecuente, es decir la lecutra, es relativamente sencillo de realizar en el menor tiempo posible.

En el mismo clock leemos el dato del Cache, verificamos el Tag para saber si es un hit. Si lo es, el dato está disponible para ser procesado.

No se puede escribir sin antes saber si el acceso es un hit. Como no podemos paralelizar la escritura con la comprobación del tag, entonces se agregan demoras.

Las escrituras deben modificar únicamente la parte de la línea que corresponde. La CPU establece de manera precisa a partir de qué dirección escribir y el tamaño del dato a escribir (1 byte a 8). En contraparte, las lecturas se llevan toda la línea a la CPU, sin demorar.

Las políticas de escritura definen el diseño del sistema cache.

Hay dos políticas muy diferentes.

### Write Through

El dato se escribe en el Cache y en el nivel inferior de la jerarquía.

- **Ventajas**
	- Implementación simple
	- El siguiente nivel jerárquico menor siempre tiene una copia coherente del dato. Esto favorece tanto a Multicore como a los subsistemas de I/O.

Se puede encontrar Write Through en el nivel más alto de un cache multinivel, porque es mucho menor el tiempo hasta el Cache L2 que hasta la DRAM.

### Write Back (Copy Back)

El dato se escribe solo en el Cache y se actualiza en el resto de la jerarquía solo cuando es desalojado.

Para minimizar la cantidad de write backs, se utiliza por cada línea un bit que indique si está modificada (**dirty**) o no modificada (**clean**).

Solo se realiza el write back al momento de desalojar líneas dirty, ya que estas son las modificadas respecto de los niveles inferiores.

- **Ventajas**
	- Las escrituras ocurren a la velocidad del cache.
	- Si en una misma línea ocurren múltiples escrituras, solo una escritura se envía al nivel inferior de la jerarquía.
	- Demanda menos ancho de banda del Bus. Mejor para los sistemas Multicore. Donde el bus es un recurso compartido.
	- Minimizar Write Back implica mínimo consumo de energía. Mejor para los embebidos.

### Write Stall

Espera del procesador para que una escritura se replique en los niveles inferiores de memoria. Para mejorar la penalización de performance debido a estos *stalls*, se puede incluir un Write Buffer en el Cache Controller.

Así las escrituras se llevan a cabo en el cache y al mismo tiempo se almacenan en el Write Buffer. Entonces en paralelo se van actualizando las copias en el nivel inferior de la jerarquía, y el procesador ejecuta otras instrucciones y lee datos del Cache.

Si se genera un Read Miss mientras el controlador está actualizando el valor, el Miss Penalty es mucho mayor, pues se debe esperar a que se termine la actualización.

El Write Buffer no elimina los Write Stall, solo reduce la cantidad. Esto es debido a que puede recibir escrituras a mayor frecuencia de las que puede realizarlas.

### El dato a escribir no está en el cache => Write Miss

Dos formas de resolver esto:

#### Write Allocate

Se lee la línea en el cache y luego se escribe. Símil a un Read Miss.

#### No-Write Allocate

La operación no afecta al Cache. El dato se escribe en el nivel inferior inmediato. Recién con una lectura, se trae el dato al cache.

---

## Ejemplos reales

### AMD Opteron

- Procesador con un solo core con Ejecución Fuera de Orden.
- Dos niveles de cache
	- L1: 64 KiB
	- L2: 256 KiB
- Direcciones virtuales de 48 bits, físicas de 40 bits envíadas al address bus.
- L1 2-way set-associative con tamaño de línea de 64 Bytes. Entonces está compuesto por dos vías organizadas en 512 sets de líneas de 64 Bytes.
- La longitud del offset se obtiene de $log_2(LineSize) = log_2(64) = 6$
- La longitud del índice se obtiene de
	 $log_2\left( \frac{CacheSize}{LineSize \ * \ SetAssociative} \right) = log_2\left(\frac{65536}{64 \ * \ 2}\right) = log_2(512) = 9$
- La longitud del tag se obtiene de $AddressSize - Index - Offset = 40 - 9 - 6 = 25$
- L1 tiene un bit LRU por cada línea que indica cual de las dos es la más recientemente accedida. Esto no es muy complejo con dos vías, pero cuando se aumentan, la complejidad y el costo aumentan considerablemente.
- L1 usa Write Back. Cada línea tiene un bit de estado que indica si se modificó la línea en algún momento. Si lo fue, su dirección y dato se escribe en el **Victim Buffer**.
- El Victim Buffer soporta hasta ocho "víctimas". Mientas estas se acutalizan en el nivel inferior, la CPU y el L1 pueden realizar otras actividades.
- Si hay Write Miss, utilizan Write allocate. Entonces, se modifica en el nivel inferior y se almacena en el cache al mismo tiempo.

**CUELLO DE BOTELLA** al intentar resolver un Load o un store mientras se necesita buscar más instrucciones. Se puede usar un D&C. Usamos dos caches separadas, una para instrucciones (ICache) y otra para datos (DCache).

- Opteron tiene 2 L1 Cache.
- Esto es un Split Cache.
- El ICache tiene un miss rate mejor que el DCache. Esta métrica no es una razón para utilizar Split Caches. La performance si lo es, para desatascar el pipeline.

## Conclusiones

Los Sistemas Multicore, y los Subsistemas I/O buscan el mínimo tráfico y consumo de energía transaccional y la menor penalización de performance que proporciona el Write Back. Al mismo tiempo se busca la robustez en coherencia que da el Write Through.