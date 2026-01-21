# Notas Instruction Level Parallelism

## Pipeline de instrucciones

- Von Neumann definió la máquina de ejecución. en 5 etapas. Una etapa por ciclo de clock, algo solo teórico.
	- **Fetch**
	- **Decode**
	- **Search Operand**
	- **Execute**
	- **Write Result**
- Hasta que aparece Intel, la mayoría de los procesadores ejecutaban el ciclo **en serie**.
	- Una etapa por ciclo de clock y nada más se ejecutaba esa etapa.
	- Si detectamos el estado de decodificación, sabemos que el bus está desocupado. Entonces agregamos señales de estado, esto lo hacemos con $\lceil log_2(\#estados + 1) \rceil$.
- **Pipeline**
	- Arquitectura que permite crear el efecto de superponer en el tiempo la ejecución de varias instrucciones a la vez.
	- Se formaliza **ILP**.
	- Poco o ningún hardware adicional.
	- Necesita que los bloques que resuelven la ejecución de una instrucción opere en forma simultánea.
	- Se logra si todos los bloques trabajan en paralelo pero con una instrucción distinta.
	- Cada etapa se llama **stage**.
	- El pipeline ideal es teórico pues, por ejemplo, el fetch de instrucciones no toma un solo ciclo de clock. Ni hablar de si hay branching.
	- **Primeras Conclusiones**
		- El pipeline tarda en llegar al primer resultado tantos ciclos de clock como cantidad de etapas haya.
		- Caso ideal: solo se consuma el primer ciclo de clock, y luego sea constante spit de resultados.
		- Este escenario es teórico. No se cumple todo el tiempo.
	
- **Eficiencia:** ¿Agregado de etapas más simples = mejor performance?.
	- Se tarda más en arrojar el primer resultado, pero cada etapa puede hacerse en menor cantidad de ciclos de clock.
	- $TPI = \frac{Tiempo \ por  \ instrucción \ en \ CPU \ ^"No Pipeline^"}{\#Etapas \ del \ Pipeline}$. Pareciera que el Time Per Instruction es proporcional con la cantidad de etapas.
	- ¿Es cierto? Si, pero hasta cierto punto. La cuenta no es 100% cierta.
		- Existen overheads introducidos por el pipeline, que suman demoras, pero el tiempo se aproxima al ideal.
	- La reducción se aprecia como si se requiriesen menos ciclos de clock para completar una instrucción.
	- **Conclusiones**
		- El pipeline no reduce el tiempo de ejecución individual, sino que incrementa la cantidad de resultados por unidad de tiempo.
		- El overhead no es significativo, pero algo agrega.
		- El rendimiento del procesador mejora notablemente al utilizar un Pipeline.

## Obstáculos/Hazards

- Degradan la performance del procesador respecto al pipeline *ideal*. 
- El efecto ocasionado por estos obstáculos es un *pipeline stall*.
- Tres categorías de obstáculos:
	- **Estructurales**
		- *Causas*
			- Una etapa no está lo suficientemente atomizada. Entonces la instrucción no puede realizarse en un ciclo de clock.
			- Si dos instrucciones que usan esta etapa están próximas del tiempo que necesita esta etapa para procesar su parte. Hay conflicto de recursos.
		- *Consecuencia*: Una de las instrucciones debe detenerse. Entonces aumenta el CPI +1 con respecto al pipeline ideal.
		- *Ejemplo*
			- Un procesador con una sola etapa para acceder a memoria y es compartida a datos e instrucciones.
			- Si se necesita un operando de memoria, el acceso para traer el operando interfiere con la búsqueda del operando de una instrucción por delante del programa. Es decir, interfiere con la unidad de FETCH.
		- *Posibles Soluciones*
			- Es necesario agregar hardware, el cual tiene que poder resolver los accesos a memoria por separado o combinadas.
				- Desdoblar L1 Cache en IL1Cache y DL1Cache es un acierto.
				- Usar buffers de instrucciones implementadas como colas FIFO.
				- Ensanchar los buses más allá de los anchos de palabra del procesador.
					- Entonces en un ciclo de clock leo mucha más información.
			- Aumentar la profundidad del pipeline, con cuidado, hace que haya más etapas que se resuelven en un ciclo de clock.
	- **Datos**
		- *Causa*: Requerimiento de un dato antes que esté disponible siguiendo la lógica del programa.
			- Es decir, si tenemos dependencias de registros para resolver otra instrucción como operando.
		- *Consecuencia*: Por cada dependencia, se pospone una operación.
			- $CPI = CPI + n$, con $n$ la distancia entre las etapas que requieren el mismo dato.
		- *Solución*: **Forwarding**
			- Se extrae el resultado directamente de la ALU, o FPU, o lo que corresponda, cuando está disponible y se hace un forward a la entrada de la etapa que lo requiere en el mismo ciclo de clock que se escribe el operando destino.
			- Permite disponer del dato en la siguiente instrucción, ahorrando el tiempo de escritura en el operando destino, sin necesidad de esperar el retiro de resultado.
			- Se aplica únicamente en aquellas instrucciones que producirían un `stall`.
			- Se consigue un $CPI = CPI + 1$, en vez de $n$, por cada dependencia.
			- **No siempre es factible**: por ejemplo, en loads desde memoria, pues no utilizan la ALU.
	- **Control**
		- *Causa*: branching
			- Un branch es discontinuar el flujo de ejecución. Totalmente contradictorio al comportamiento secuencial del pipeline.
			- Se debe hacer un descarte de todo lo pre procesado por el pipeline. Entonces se debe consumir como si fuese un reset del pipeline, los ciclos de clock correspondientes a las $n$ etapas del pipeline. 
				- Esto es conocido como **branch penalty**.
			- La cantidad de etapas que tengamos nos juega en contra, pues $CPI = CPI + n - 1$, pues debemos desechar todas las etapas previas luego de producirse el branching.
		- En las interrupciones sucede lo mismo.
		- *Principal inconveniente*: salto condicional
			- Debemos determinar si $COND = TRUE$ o $COND=FALSE$
			- Si la condición es TRUE, se habla de **branch taken**, se cambia el registro PC/IP a la dirección del salto.
			- Si la condición es FALSE, entonces estamos ante un **branch not-taken**, el PC aumenta a la próxima dirección secuencial, y continúa la ejecución.
		- *Determinar si hay un salto*:
			- En la etapa de EXECUTE, si es **condicional**.
			- En la etapa de FETCH, si es **incondicional** o **llamada a subrutina** con direccionamiento **INDIRECTO**.
			- En la etapa de DECODE, si es un salto **incondicional** o **llamada a subrutina** con direccionamiento **DIRECTO** (la dirección de salto viene a continuación).
		- *¿Soluciones...?*
			- Forwarding puede ayudar a disminuir el efecto, pero no es óptimo, porque solo reducimos algunos ciclos de clock del branch penalty, pero no se borra el efecto.
			- Una **BPU** (Branch Prediction Unit), que recurre a análisis pormenorizados, que tiene en cuenta el comportamiento de algoritmos y saltos.
 