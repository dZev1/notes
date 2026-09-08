# Scheduling

## Intro

- Una de las principales huellas de identidad de un SO.
- Algunos SO proveen más de una.

- ¿Qué busca optimizar la política de scheduling?
	- *fairness*: que cada proceso reciba una dosis "justa" de CPU.
	- *eficiencia*: buscamos que la CPU esté ocupada todo el tiempo.
	- *carga del sistema*: minimizar la cantidad de procesos listos que están esperando CPU.
	- *tiempo de respuesta*: minimizar el tiempo de respuesta que perciben los usuarios.
	- *latency*: minimizar el tiempo requerido para que un proceso empiece a dar resultados.
	- *tiempo de ejecución*: minimizar el tiempo total que le toma a un proceso ejecutar completamente.
	- *throughput (rendimiento)*: maximizar el nº de procesos terminados por unidad de tiempo.
	- *liberación de recursos*: hacer que terminen lo más antes posible los procesos que usan más recursos.
- Muchos de esos objetivos se contradicen entre sí.
- Cada política de scheduling va a buscar maximizar una función objetivo, que va a ser combinación de esas metas buscando impactar lo menos posible en el resto.

## Tipos de scheduler

- **Preemptive** $\rightarrow$ con desalojo.
	- El scheduler se vale de la interrupción del clock para decidir si el proceso actual sigue corriendo o le toca a otro.
	- No dan garantías de continuidad a los procesos.
	- El scheduler analiza la situación cuando el kernel toma el control. Especialmente cuando se hace I/O.
	- Se suele proveer llamadas explícitas para permitir que se ejecuten otros procesos.
- **Non-preemptive** $\rightarrow$ sin desalojo.
	- Corre el proceso entero hasta terminar.

## Políticas de scheduling

- **FCFS**
	- Nonpreemptive
	- Si llega un proceso que requiere muuucha CPU, tapa a los demás.
	- Si le agregamos prioridades, como en una sala de espera:
		- Puede haber starvation.
		- Solución a eso: aging.
		- **Cualquier esquema de prioridades fijas corre riesgo de starvation**.
- **RR**
	- Preemptive
	- Si el quantum es muy largo, puede parecer que el SO no responde.
	- Si el quantum es muy corto, el tiempo de scheduling y de context switch se vuelve una proporción muy importante del quantum.
	- Se lo combina con prioridades:
		- Pueden estar dadas por el tipo de usuario, o decididas por el proceso (esto no funca).
		- Estas prioridades decrecen a medida que los procesos reciben su quantum, as to avoid starvation.
		- Los procesos que hacen I/O, reciben crédito por ser "buenos compañeros".
- **MLFQ**
- **SJF**
	- Ver en práctica.

## Real Time Scheduling

- Las tareas tienen deadlines estrictas.
- Se usan en entornos críticosa.
	- Si un deadline no se cumple, pasa algo.
- Es un problema en sí mismo.

## Scheduling en SMP

- Problema bastante distinto.
- El problema principal es el cache.
- Si hace saltar un proceso a otro procesador, el cache llega vacío.
	- Tardando más que si se ejecutaba en el mismo que antes.
- Así se llega al concepto de *afinidad al procesador*, donde tratamos de usar el mismo procesador, aunque se tarde un poco más en obtenerse.
	- Si se respeta hard, es *afinidad dura*.
	- Si es solo un intento, *afinidad blanda*.
- A veces intenta distribuir la carga entre todos los procesadores.

## Fin

- En la práctica es muy complicado elegir el "mejor" algoritmo de scheduling.