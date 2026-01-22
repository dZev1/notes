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