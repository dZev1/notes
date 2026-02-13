# Notas Out of Order Execution

## Scheduling Dinámico

- Los pipelines que vimos, fueron con scheduling estático. 
- Simplemente buscan una instrucción y la despachan a la unidad de ejecución. En caso de tener dependencias con otra instrucción, se hacía un pipeline stall.
- En caso de superscalars, el estudio de dependencias se extiende a instrucciones en ambos pipelines.
- Se vuelve insuficiente el Forwarding.

## Ejecución Fuera de Orden (OOO-E)

- Consiste en tratar de enviar instrucciones a ejecución independientemente del orden en el que están en el código.
- Una vez decodificada, podemos saber si va a haber un atasco de datos o estructural, el cual va a provocar que la instrucción que se comience a ejecutar impida el envío de sus dependientes próximas.
- Cada vez que intentemos ejecutar fuera de orden, podemos lograrlo. Pueden aparecer riesgos que deben ser evaluados para no provocar errores.
- Se necesita agregar una etapa de escritura del resultado en el operando destino.
	- Vamos a tratar de avanzar en la ejecución fuera de orden, pero no queremos escribir en orden distinto al real los resultados.
	- Sino estamos cambiando la lógica del programa.
- No hablamos de STALLS, hablamos de **WAITS**.
- **RIESGOS** (Las siglas es lo que no quiero que pase)
	- **WAR**: Write After Read.
		- Escrituras antes de tiempo que trae efectos colaterales en los registros operandos de instrucciones anteriores. Por eso se agrega la etapa del Write.
	- **WAW**: Write After Write.
		- Escritura antes de tiempo del resultado de una instrucción más adelantada que la que estaba esperando, cosa que luego hace que cuando se termine la ejecución de la más antigua, pise el valor del registro destino de la operación si lo comparten ambas instrucciones.
	- **RAW**: Read After Write
		- Una instrucción posterior lee un operando que después es escrito por una instrucción previa. Es la dependencia de operandos típica que se solucionaba con un pipeline stall. Ahora haremos que el procesador *espere* el resultado para poder ejecutar.
- **Excepciones**
	- Se necesita preservar el comportamiento de modo tal que aparezcan los resultados en el que ocupan las instrucciones en el programa.
	- Al ejecutar fuera de orden, y se genera una excepción, tenemos que limpiar todo el pipeline, entonces se pueden generar **excepciones imprecisas**, que son aquellas donde el estado del procesador no es el mismo si se ejecutaban las instrucciones en orden.
	- Hay que esperar que se termina la ejecución de las instrucciones fuera de orden y luego limpiarlo y atender la excepción.

- **Scoreboarding** (Antiguo)
	- Método más sencillo, y menos sofisticado, para implementar OOO Execution, evitando riesgos WAR y WAW.
	- Implementado por primera vez en 1964, por CDC.
	- Paralelizaron la Unidad de Ejecución.
		- 4 FPUs.
		- 5 Unidades de referencia a memoria.
		- 7 ALUs.
	- Se dividió la Unidad de Decodificación en dos: una Unidad de Envío y una Unidad de Lectura de Operando.
	- Había una cola de pre búsquedas.
	- Era muy lenta cuando ejecutaba OOO.
	- Nuevos obstáculos estructurales
		- Cantidad de buses limitada para paralelizar las transferencias entre el scoreboard y los registros.
	- Ni siquiera podía aprovechar forwarding

## Algoritmo de Tomasulo (1967)

- Implementación de Scheduling Dinámico en la FPU.
- Se quería minimizar los riesgos RAW.
- Neutralizó WAW y WAR usando renombre de registros.
- ¿Qué se necesita para implementar OOO?
	- Mantener un enlace entre el productor del dato y quienes lo consumen.
		- Para solucionar esto, se usa **renombre de registros**.
			- Se asocia un tag con cada operando.
			- Tomasulo planteó utilizar dos registros internos, sub-arquitecturales, que actúen como registros de propósito general. 
				- El usuario no debe poder acceder a ellos. Solo la CPU puede escribir sobre ellos.
			- Cualquier uso del registro que produzca WAR o WAW será reemplazado por el temporario.
			 ![[Pasted image 20260122134413.png]]
			- El análisis de código busca mirar más adelante. Esto agrega más sofisticación y complejidad en el compilador.
				- Eventualmente se pudo realizar en el hardware.
			- Uso de una tabla, Register Alias Table, donde hay una entrada por cada registro.
				- A cada registro le puso dos campos: un TAG, que representa el renombre que se hizo al registro, y el Valor que tiene el registro temporal en ese momento.
				- Hay además un bit de validez. Por defecto, cuando se agregan tags, se marca no válido.
				- El Tag se usa para referenciar al registro.
	- Mantener instrucciones en espera hasta que estén listas para ejecutarse.
	- La instrucciones deben saber cuándo sus operandos estén `READY`.
	- Disparar la instrucción a su Unidad Funcional ni bien todos los operandos estén `READY`
	- Para resolver estas tres últimas funciones, se utiliza un subsistema de hardware conocido como **reservation station**.
		- Utiliza bancos o archivos de registros internos para cada instrucción.
		- Cada entrada tiene su tag que se corresponde el que se asignó en el renombre.
		- Cuando se coloca disponible un operando en la Unidad de Ejecución, se hace una retransmisión de su tag y su valor por todas las reservation stations.
		- Si un operando destino recibe múltiples escrituras, la reservation station solo aplica la última.
		- Cuando una instrucción tiene todos sus operandos READY, se dispara la instrucción a su Unidad Funcional correspondiente.
		- Si la Reservation Station tiene una cantidad de registros superior a los registros de la arquitectura, se **eliminan** todos los riesgos estudiados (**RAW**, **WAR**, **WAW**).
	- **Common Data Bus**
		- Crucial para el broadcast de los resultados.
		- Datapath que cruza la salida de todas las Unidades Funcionales, atraviesa las Reservation Stations, los Floating Point Buffers, los Floating Point Registers, y el Floating Point Operations Stack.
		- Cruza toda la arquitectura.

### Ejemplo

![[Pasted image 20260124142029.png]]

![[Pasted image 20260124150454.png]]

- El modelo asume que el procesador puede escribir cuatro resultados en el mismo ciclo de clock.
- Luego de la ejecutar la etapa de RESULT, se escribe el valor producido por una de las Unidades de Ejecución en los registros de la RS que tengas su tag.
#### Situación

- RAT de 16 entradas, R0-R15.
- Unidad de Multiplicación
	- RS de 4 entradas (`a`,`b`,`c`, `d`)
	- Cada entrada tiene Operando 1 - Operando 2.
- Unidad de Adición
	- RS de 4 entradas (`m`, `n`, `o`, `p`).
	- Cada entrada tiene Operando 1 - Operando 2.
- Unidad Load/Store
	- RS de 4 entradas (`w`, `x`, `y`, `z`).
	- Un solo operando por entrada.

#### Funcionamiento

- `ld R1, 0(R3)`
	- RAT todo inválido al comienzo
	- Tras comenzar la ejecución del `ld`:
		- `TAG(R1) = w`, `VALUE(R1) = 0xBABAB00`, `VALID = 0`
			- Es inválido porque es el operando en el que tenemos que escribir el resultado del `ld`.
		- `TAG(R3) = ~`, `VALUE(R3) = 0xF151C00`, `VALID = 1`
			- El campo del `TAG` no es relevante.
			- `0xF151C00` es el puntero al que apunta R3.
			- Tengo que guardarlo como válido porque voy a tener que usarlo para ejecutar la instrucción.
				- No quiero que cambie mid-execute.
			- Agrego el valor a la RS de la unidad L/S.
			- Ocupa la posición `w` de la RS. 
		- Como R3 ocupa la posición `w` en la Unidad L/S, el tag que le corresponde a R1 en el RAT es `w` (R1 depende del valor de R3).
			![[Pasted image 20260124152145.png]]
	- Se despacha para su ejecución.
- `mul R6, R1, R2`:
 ![[Pasted image 20260124153618.png]]

- `add R11, R11, R6`: ![[Pasted image 20260124154304.png]]

## Especulación Por Hardware

- Hay más líos con los saltos. Los BTB evitaban los stalls, pero con la OOO Execution se necesitaba repensar.
- Se comenzó a especular los saltos.
- Lo propuesto por Tomasulo no permitía con certeza liberar resultados en orden.
- Pero, el commit de los resultados DEBE ser en orden. Caso contrario atentamos contra la lógica del programa.

- **SOLUCIÓN: Re Order Buffer (ROB)**
	- Bloque parecido a la RS de Tomasulo.
	- Agrega registros que van almacenando resultados de las instrucciones ejecutadas en base a especulación por hardware.
	- El resultado de una instrucción permanece en el ROB desde que se obtiene el resultado hasta que se copie en el operando destino.
		- A diferencia de Tomasulo que inmediatamente largado el resultado se escribía en el operando de otra instrucción.
		- En el ROB se guarda el valor del resultado y su bit de validez. Cuando llega el momento, copia el resultado en el destino (registro/memoria).
	- Tiene la siguiente info:
		- **Tipo de instrucción**
			- Indica si la instrucción fue
				- un branch (en cuyo caso no hay resultado destino)
				- memory store (hay que calcular la dirección del operando)
				- una instrucción de la ALU (el resultado va a parar a un registro).
		- **Destino**
			- Número donde guardo el resultado, ya sea
				- Registros para operaciones memory load o de ALU.
				- Dirección de memoria donde guardar el resultado en un memory store.
		- **Valor**
			- Almacena el resultado de la instrucción hasta el commit.
		- **Ready**
			- Indica si la instrucción está libre y su resultado disponible.
			- Se fija si las anteriores, en caso de no ser la primera, tienen READY, si no lo están, espera a que lo estén, no se escribe el resultado.
	- **Implementación**
		- Requerimos de las siguientes etapas en el pipeline
			- **Envío**
				- Se obtienen instrucciones desde una cola de prebúsqueda, tratando de ubicarlas en una RS vacía, y un slot del ROB.
				- La instrucción se envía junto con los datos que corresponden a sus operandos si están disponibles en sus registros.
				- Si la RS no está disponible, o no hay slots libres en el ROB, se produce un STALL por obstáculo estructural.
				- En caso de escribir, se actualizan todas las estructuras internas con la información que corresponde.
			- **Ejecución**
				- Si faltan operandos, se monitores el bus de datos interno hasta que estén disponibles.
				- Se chequean riesgos RAW.
				- Se ejecutan las operaciones en las que se tienen los operandos READY en la RS.
				- Demora según lo que demore la instrucción.
			- **Write Result**
				- El resultado se escribe en el ROB.
				- Si alguna RS espera el resultado, este se hace un broadcast y también lo escribe en el registro que corresponde.
				- En caso de Memory Store, se escribe el resultado en el campo valor correspondiente del ROB.
			- **Commit**
				- Fase final.
				- El resultado solo está en el operando destino.
				- Si se trata de un branch con predicción incorrecta, se hace un flush de la entrada del ROB y vuelve a comenzar a partir de la dirección sucesora correcta del branch.
					- Si predijimos salto non-taken, metimos instrucciones secuenciales al salto, pero si el salto se tomó, todo esto es basura. Se tira.
				- Para una predicción correcta, se copia el valor del ROB en el operando destino.
				- Si la operación es un memory store, es similar pero se escribe en una dirección de memoria.