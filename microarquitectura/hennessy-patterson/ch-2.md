# Notes Chapter 2

- Buscamos proveer un sistema de memoria con un costo por byte que sea tan bajo como el nivel más barato de la jerarquía de memoria, y una velocidad casi tan rápida como la del nivel más caro.
- **Propiedad de inclusión** $\longrightarrow$  los datos incluidos en el nivel más bajo de la jerarquía son un superset de los datos en los niveles de memoria más altos.
- Se gana una gran performance haciendo un multiport y un pipeline de los caches. 
	- Usando 3 niveles de cache, dos privados (L1, L2) y uno compartido (L3), y usando un L1 separado para Datos y para Instrucciones.

## GDRAM

- Son una clase especial de DRAM basadas en SDRAM pero ajustadas para manejar una demanda de ancha de banda superior que proviene de las GPU.
- GDDR5 está basada en DDR3
	- GDDRs más viejas están basadas en DDR2.
- GDDRs tienen diferencias:
	- Intefaces más amplias (32 bits contra 4, 8 o 16 de diseños actuales).
	- Mayor clock rate máximo en los pines de datos.
		- Esto para permitir un rate de transferencias más alto sin incurrir en problemas de señales.
	- Están comunente soldadas a la GPU, no como la DRAM que está en un arreglo de DIMMs expandibles.

## Stacked/Embedded DRAMs

- Innovación en 2017.
- Colocación de DRAMs en una forma adyacente o apilada dentro del mismo paquete del procesador (si es dentro del chip, se dice que es **Embedded DRAM**).
- Hacer esto permite reducir el latency de acceso e incrementar el ancho de banda al admitir más conexiones más rápidas CPU-DRAM.
- Esto los productores lo llaman **HIGH BANDWITH MEMORY (HBM)**.
- **COLOCACIÓN**
	- Una versión coloca el die DRAM directamente sobre el die de la CPU.
		- Si se consigue una disipación de calor adecuada, se pueden colocar varios dies DRAM así.
	- Otra versión stackea las DRAM y las coloca junto a la CPU en un paquete solo usando un sustrato que contiene las conexiones. 

## FLASH

- Lecturas son secuenciales y leen una página entera, que pueden ser 512 Bytes, 2 KiB o 4 KiB (en el TP usamos este último).
	- Tiene un gran delay accediendo al primer byte de la página, pero luego lee el resto a 40MiB/s.
		- Sigue siendo mucho más lento que la SDRAM.
- Se debe borrar antes de ser reescrita y se borra en bloques, no en bytes individuales o palabras.
- No volátil
- Limita el número de veces que un bloque se puede escribir.
	- Asegurando distribución uniforme de los bloques escritos a lo largo de la memoria, se puede maximizar la vida de la memoria flash.
- Es más barato fabricar NAND Flash que SDRAM pero más caro que HDDs.
- Los **flash controllers** manejan las transferencias de páginas, proveen cache de páginas y manejan el write leveling.

## Para evitar errores...

todas las memorias tienen filas y addresses de más para evitar que los defectos de fábrica o de uso rompan el sistema, o hagan que dure más.


## 10 optimizaciones de performance

- **CATEGORIAS según las siguientes métricas**
	- Reducción de Hit Time.
		- Caches de primer nivel más simples y pequeños.
		- Implican decrementar el consumo de energía
	- Incremento de ancho de banda del cache.
		- Caches en pipeline, multibancos y que no bloqueen.
	- Reducción de Miss Penalty.
		- Critical word first, merge write buffers.
		- Poco impacto en la energía.
	- Reducción de Miss Rate
		- Optimizaciones de compilador.
	- Reducción de miss penalty/rate con paralelismo.
		- Prefetching de hardware y prefetching de compilador.
		- Incrementa el consumo energético. Principalmente por datos prefetcheados que no son usados.
### 1. Primeros caches pequeños y simples para reducir hit time y consumo.

- Bajos niveles de asociatividad reducen el hit time y el consumo.
- Reduce el consumo porque hay menos líneas de cache a ser accedidas.
- Caches multibanco para que los accesos solo activen una porción del cache.
	- Se usan para incrementar el ancho de banda del cache.
	- Reducen el consumo porque una parte menor del cache es accedida.
	- Los L3 son multibancos.
- Se siguen usando L1 de 8 vías, aunque sea más lento, porque no se accede secuencialmente, sino que se accede con un pipeline.
- Sumar a esto que el procesador tarda como mínimo 2 ciclos en acceder al cache, entonces la performance no se ve tan afectada.
- También hay que mantener a la TLB fuera del camino crítico. Por esto todos los L1 Cache deben indexarse virtualmente.
- Por último la introducción del multithreading hace que pueda haber más conflict misses, haciendo la asociatividad más alta mejor.

### 2. Predicción de Vía para reducir Hit Time

- Reduce conflict misses y mantiene el Hit Speed de Direct Mapped.
- Se colocan bits extra en el cache para predecir la vía, o el bloque dentro del set, que va a ser accedido en el próximo acceso.
	- El multiplexor envía antes de tiempo el bloque deseado y solamente se hace una comparación de tag en paralelo al leer del cache.
	- Un Miss Predict lleva a tener que leer los demás sets en el próximo ciclo de clock.
- El set predictor tiene una certeza del 90% para 2-way set associative, y 80% en 4 vías, con mejor certeza en los ICaches que en los DCaches.

### 3. Acceso en pipeline y Caches Multibanco para incrementar el ancho de banda.

- Son duales con los approaches de los superpipeline y los superscalar de incrementar la cantidad de instrucciones por unidad de tiempo.
- Hacer un pipeline sobre I-L1 permite un ciclo de clock mayor, pero a costa de latency incrementado.
- Lleva a mayor penalización en branches con predicción errónea.
- Lleva a más ciclos de clock entre el store del dato y el uso del mismo.

### 4. Caches nonblocking para incrementar el ancho de banda.

- Necesitamos que no se stallee el procesador ante un data miss para OOO-Execution.
- Queremos que, por ejemplo, el procesador continúe fetcheando instrucciones del I-L1 mientras que se arregla el Data Miss en el D-L1Cache.
- Un Nonblocking Cache permite que el Data Cache continue supliendo Cache hits durante un Miss.
- Reduce el Miss Penalty.

### 5. Palabra crítica primero y Early Restart para reducir Miss penalty

- El procesador normalmente requiere solo una palabra de un bloque a la vez.
- No esperemos a que se cargue todo el bloque antes de enviar la palabra solicitada y reiniciar el procesador.
- **Dos estrategias**
	- *Critical Word First* $\longrightarrow$ Request del dato faltante primero desde memoria y enviarlo al procesador apenas llegue. Permitir que el procesador continue su ejecución mientras se llena el resto de las palabras del bloque.
	- *Early Restart* $\longrightarrow$ Fetchear las palabras en orden normal, pero apenas llegue la palabra que se necesite, enviarla y dejar que el procesador continue su ejecución.

### 6. Mergear Write buffers

- Si un cache tiene un dato en su write buffer que estoy queriendo acceder luego de haberlo desalojado, mergeamos el write buffer con una línea de cache para no tener que esperar a que se escriba abajo.