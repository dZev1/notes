# Notas Memoria Cache
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
	- **Line Placement**: se utiliza un mapping por sets. Si son dos sets, se llama 2-way-set-associative. Hay dos casos particulares, el 1-way-set-associative es un set para cada línea, esto es lo que llamamos Fully Associative y, en caso de que haya un set por línea en el cache, tenemos lo que se llama Direct Mapping. Para obtener en qué set colocamos la línea, usamos el resto de dividir al address físico de una línea por la cantidad de sets en la cache. Es decir, ($Physical Address \ mod \  \#CacheSets$ ). Luego se utilizan los bits intermedios del Cache Directory con el tag correspondiente para determinar en qué ubicación del set ubicar la línea (es un índice). Por último, los últimos bits nos dan el offset del dato buscado.
	- **Line Identification**: Una vez que nos llega un address, vemos si el tag del address está en el directorio, si está, vamos a llevar el tag a la memoria cache. Allí, comparamos en paralelo con los bancos de memoria (índice del set), para ver cuál es el tag correcto. Cabe aclarar que dentro del banco, también se tiene un bit de validez de tag, y la comparación se hace solo si el tag es válido en el banco. Si el tag está, obtenemos su dato.
	- **Line Replacement**: ¿De qué manera vamos a reemplazar la línea?
		- Políticas
			- **Random**: Elegimos dentro del set de la línea, una aleatoria (semi aleatoria con aplicaciones de hardware), y reemplazamos la línea por la nueva que leimos/escribimos.
				- *Ventaja:* Fácil implementación.
				- *Desventaja*: Podemos caer en conflict misses muy seguidos, pues podemos sacar una línea que fue recientemente colocada.
			- **LRU**: Siguiendo el principio de localidad, es probable que aquellas líneas recientemente accedidas sean vueltas a acceder.
				- *Ventaja*: Mejor precisión a la hora de elegir qué línea quitar de la memoria cache.
				- *Desventaja*: Costoso a nivel de hardware cuanto más grande sea el cache. Se termina aproximando en muchos casos.
		 - LRU es similar a random en performance cuando crece el tamaño del cache, pero gana en caches de tamaño más pequeño. Es por eso que se suele utilizar LRU para caches L1 y Random para L2, L3.
	- **Write Policies**: ¿De qué manera vamos a escribir los datos?
		- Para escribir, debemos esperar a que se pueda determinar si hay un hit o un miss. Si hay un hit, es trivial escribir en el dato... ¿Pero si hay un miss?
		- Las escrituras alteran datos que se encuentran replicados en los niveles inferiores de la jerarquía, aún cuando no se encuentren en el primer nivel.
		- Si hay un miss, tenemos dos políticas
			- **Write Through**: El dato se escribe en el Cache L1 y luego en el nivel inferior inmediato de la jerarquía (Cache L2).
				- *Ventajas*
					- Implementación simple.
					- Asegura la coherencia.
					- Favorece sistemas multiprocesador y subsistemas de Entrada-Salida.
					- Escribir en el L2 Cache es mucho más rápido que escribir en la DRAM.
			- **Write Back**: Se agrega un bit a la línea que nos dice si la línea fue modificada o no. Se escribe el dato solo en el Cache L1 y solo se actualiza en los niveles inferiores si es desalojado.
				- *Ventajas*
					- Máxima velocidad de escritura.
					- Requerimiento de ancho de banda inferior.
					- Minimiza consumo de energía.
				- *Desventaja*: Genera Write Stalls
					- Para mejorar esta penalización se implementan Write Buffers en el Cache Controller.
						- El write penalty es mayor si hay un read miss de un dato que está en el buffer.
						- El buffer recibe escrituras más rápido de lo que puede escribirla en el nivel inferior. Solamente REDUCE Stalls.
		- Si el dato a escribir no está en el Cache, tenemos dos formas de solucionarlo:
			- **Write Allocate**: Escribo en el L1 la línea que fue leída y luego modificarla.
			- **No-Write Allocate**: Escribo el dato en el nivel inferior, sin afectar la L1. Una vez que sea leído el dato, se trae la línea desde el nivel inferior hasta el L1 Cache.
	- **Conclusiones**
		- Queremos tener Write Back la mayor parte del tiempo y la robustez de un Write Through.

## Sistemas SMP

- Sistemas con acceso a memoria uniforme.
	- Todos los cores tienen el mismo retardo para acceder a memoria, aunque se organice en varios niveles y bancos.
- Si aumenta la cantidad de cores, aumenta el ancho de banda de la memoria DRAM.

## Coherencia

- Queremos que cada copia de una línea que esté en la memoria Cache de uno o más cores, sea idéntica una de la otra.
- Cuando escribimos un dato, necesitamos mantener esta propiedad. Es por eso que se llegó a distintas estrategias para cumplirlo.
	- Si el sistema es mono-core, simplemente tendríamos que replicar el dato a los niveles inferiores, ya sea con un Write-Back o Write-Through.
	- En un sistema SMP multi-core, se agrega el tener que no solo replicar hacia abajo en la jerarquía, sino que también debemos migrar el dato a todas las copias que había en los demás cores o, al menos, invalidar su copia mediante una notificación de escritura.
- **Protocolos**
	- Siempre van a requerir mecanismos de tracking entre cores para líneas compartidas y establecer estados, como Dirty a la línea modificada.
	- Dos clases:
		- **Directory Based**: se almacena en un directorio centralizado todas las líneas y su estado.
			- Es muy costoso de implementar
		- **Snoopy**: Cada core mantiene el estado de sus líneas y puede monitorear lo que hacen el resto de cores con un bus dedicado (**Snoop Bus**).
			- Es la clase más común, pues es más complejo, pero más eficiente en recursos. 
			- Hay dos formas de implementarlos
				- **Write Invalidate**: Cuando un core escribe un dato, se garantiza exclusividad a ese core de esa línea. Toda copia que haya en el resto de cores/niveles es invalidada inmediatamente.
					- Por lejos lo más aplicado, pues es solo agregar un bit de validez y es trivial implementarlo.
				- **Write Update**: Cada vez que se escribe un dato, el valor de la línea se copia a todos los demás cores que tengan a esta línea en su banco. Es muy costoso, pero con menor Miss Rate.
					- Hace crecer increíblemente el ancho de banda del bus.
			- El Snoop bus se suele implementar actualmente como un pequeño bus entre la cache compartida y las caches privadas de los procesadores. Anteriormente, se implementaba con el bus del sistema, llevando a cada controlador cache el R/W del opcode y el address.
			- Los Cache write-back usan el Cache Read Miss como Write Miss.

## Protocolos de Coherencia

### MSI

- Tres estados: MODIFIED, SHARED, INVALID
- El estado Shared es impreciso, pues significa que la línea puede llegar a estar compartida, pero también puede que no, ofc.
- MODIFIED implica EXCLUSIVIDAD.

- Partiendo desde *Invalid*
	- Un CPU Read propaga un Read Miss al bus, poniendo la línea en nuestro banco, pero colocándola en Shared, pues no sabemos con precisión si está o no en el banco de otro core.
		- Si la copia en otro core está shared, no ocurre nada con ella.
		- Si la copia en otro core está modified, debemos realizar el replicado a los niveles inferiores (Write Back), y luego colocarla en estado shared
	- Un CPU Write propaga un Write Miss al bus, colocando a la línea en el banco en estado Modified, pues la acabamos de modificar.
		- Esto garantiza exclusividad, por lo tanto, cuando propagamos el Write Miss, todas las copias de la línea dentro de los demás bancos de los otros cores son invalidadas inmediatamente.
- En estado *Modified*
	- Cualquier Hit, ya sea Read o Write, mantiene el estado, pues estamos modificando la copia exclusiva, entonces no ocurre nada.
	- Si ocurre un Conflict Write Miss, replicamos la línea en los niveles inferiores, y propagamos un Write Miss al bus de la línea nueva, avisando a los demás cores de invalidar la línea nueva, reemplazando a la replicada.
	- Si se produce un Conflict Read Miss, entonces hay una línea que necesitamos leer que no está en la cache, y la línea Modified es la candidata perfecta para ser removida.
		- Luego debemos realizar el Write Back correspondiente y propagar el Read Miss en el Bus y colocamos la nueva línea.
		- Esta nueva línea no tenemos certeza si es exclusiva o no, es decir, colocamos en Shared.
- Desde *Shared*
	- Cualquier Read Hit, se mantiene en estado Shared, solo estamos leyendo la línea, no modificando nada.
	- Un Conflict Read Miss no cambia el estado de la línea, pues estamos cambiando a una nueva línea, sin saber si está compartida o no en el resto de bancos.
	- En caso de realizar un Write, debemos propagar un Invalidate al bus, para que el resto de cores, si tienen la línea, la invaliden. Esto pone a la línea en estado Modified.
	- En caso de haber un Conflict Write Miss, debemos enviar un Write Miss, para que se invaliden las copias, pero de la nueva línea, y luego modificarla. Obviamente, esto conlleva a pasar a Modified

- Tiene como desventaja que, al ser Shared impreciso, se realicen transacciones al bus de más en los casos que la línea sea exclusiva de ese core. Esto lo arregla el siguiente protocolo.

### MESI

- Agrega un estado nuevo, EXCLUSIVE. Repara medianamente la imprecisión del estado Shared del protocolo MSI
	- Si se invalida, no se informa en ningún momento para pasar a Exclusive desde Shared.
- Además se implementa dos señales
	- **RFO**: Read For Ownership. Asegura el acceso a un dato válido cuando no esté coherente a lo largo de toda la jerarquía.
	- **Shared**: Indica si otro core posee la línea que se envía por el Address Bus. En caso de no levantar la señal, estaremos en una línea con estado EXCLUSIVE.

- Desde *Invalid*
	- Si se ejecuta un CPU Read y no recibimos un Shared del bus tras propagar un Read Miss de la línea, quiere decir que la línea no está presente en el resto de bancos cache, luego la agregamos al banco de nuestro core como Exclusive.
	- En caso de que el bus avise que está en otro banco de otro core, se levanta la señal Shared, por lo tanto no tenemos una línea exclusiva, sino que debemos establecerla Shared.
	- Si se ejecuta un Write, simplemente levantamos un Write Miss al bus, para que las líneas que tenían la línea la invaliden, haciendo su write back si estaba modified en otro core, y se coloca la línea en estado Modified en el banco.
- Desde *Exclusive*
	- Cualquier Read Hit no altera los datos guardados, por lo tanto no altera el estado Exclusive de la copia, pues tampoco la propagamos en otros cores.
	- Como mencioné antes, si se escribe, como somos los únicos dueños de la línea, no hace falta propagar un Write Miss, simplemente cambiamos el estado a Modified, pues está sucia con respecto a la memoria principal y escribimos el dato.
	- Si hay un Conflict Read Miss, pero viene un Shared del bus, propagamos el Read Miss y reemplazamos la línea. Provino un Shared del bus, entonces establecemos la línea en Shared.
	- Si no levanta el Shared, solo debemos reemplazar la línea y pasar el Read Miss al bus.
- Desde *Modified*
	- Cualquier hit, ya sea read o write, escribe sobre la línea o lee sobre ella, según corresponda, pero sin alterar su estado, pues sigue modificada con respecto a la memoria principal.
	- Si se ejecuta un Conflict Read Miss pero no vino un Shared del bus, entonces propagamos un Read Miss de la nueva línea, previamente haciendo un Write Back de la anterior y luego reemplazándola por la nueva, poniéndola en estado Exclusive, pues no provino Shared del Bus.
	- En caso de que sí venga el Shared, distamos en poner la línea en estado Shared.
- Desde *Shared*
	- Si hay un Conflict Read Miss, reemplazamos la línea, propagando un Read Miss. Si llega un Shared del bus, no cambiamos el estado de Shared.
		- La invalidación de la predecesora no se informó nunca. Shared sigue siendo impreciso.
	- Si no vino un Shared del bus, solamente se esparce el Read Miss y se hace el replacement, siendo ahora dueños Exclusive de la línea.
	- Si hay un Conflict Write Miss, propagamos el Write Miss, para que el resto invaliden sus líneas, y hacemos el replacement, poniendo estado Modified.

- Modified y Exclusive son precisos.
- Shared o Exclusive pueden ser Invalid en cualquier momento, Modified también pero requiere Write Back previo.
- Optimiza el uso del Bus, habilitando Write Back siempre que sea posible.
- Al invalidar las copias cada vez que se escribe un dato, se recurre a write through. Es aquí donde hay lugar para mejoras, que aporta el protocolo MESIF para Intel y MOESI para el resto de procesadores multicore que lo implementen.

### MESIF

- Hace que en un determinado set de cores con una línea compartida, uno de los cores tenga su línea marcada como Forwarder.
- El Forwarder se encarga de proveer la línea a aquellos cores que quieran acceder a la lectura de la línea y no la posean en su banco, permitiendo un copiado más rápido (Cache-Cache) en vez de tener que ir a fetchear el dato más abajo en la jerarquía de memoria.
- El Forwarder se asigna en base al nuevo solicitante, donde aquel core que haya accedido más recientemente es más probable que vuelva a acceder al dato, entonces se asignará a este core.
- Si se desaloja el Forwarder, la línea queda huérfana, todas las líneas en Shared, pero sin forwarder.
	- Cuando otro core pida el acceso, entonces se le otorga a este el rango Forwarder.

### MOESI

- Agrega un estado nuevo, Owned.
- Una línea pasa a este estado en el caso de que haya un Read Miss y esté en estado Modified, para evitar Write Backs hacia los niveles inferiores de memoria.
- El Owner de la línea se encarga de otorgar la línea a aquellos cores que necesiten acceder a la línea. Es el responsable de la coherencia.
- De nuevo, al igual que MESIF, permite hacer copias más rápidas (Cache-Cache), en vez de ir a niveles inferiores de memoria.
- En caso de tener que efectuarse un Write Back, el responsable de hacerlo es el Owner de la línea. El resto de las copias de la línea permanecen Shared y válidas.
- En caso de escribirse una línea en estado Owned o Shared, pasa a estado Modified, con la invalidación correspondiente en el resto de cores, sea Owned o Shared. No hace falta realizar Write Backs, pues la línea sigue estando dirty.