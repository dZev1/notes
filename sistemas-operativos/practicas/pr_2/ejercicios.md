# Práctica 2

## Ejercicio 1

![[Pasted image 20260903103820.png]]

- Ráfagas de CPU: $T_{CPU} = 8$
	- T: \[0 - 2\] $\Rightarrow$ |T| = 3
	- T: \[11 - 13\] $\Rightarrow$ |T| = 3
	- T: \[21 - 22\] $\Rightarrow$ |T| = 2
- Ráfagas de E/S: $T_{I/O} = 15$
	- T: \[3 - 10\] $\Rightarrow$ |T| = 8
	- T: \[14 - 20\] $\Rightarrow$ |T| = 7

## Ejercicio 2

No convendría usar RR, porque seria un derroche de recursos de hardware. El único proceso que puede completamente usar su quantum es P2, por ser CPU bound. Pero, en el caso de P1 o P0, apenas reciben el control, puede que les llegue una operación de E/S y se bloqueen, nuevamente dándole el control a P2, y nuevamente esperando al final de la cola de procesos.

Es por esto que convendrían colas de prioridad, 2 para ser exactos. La de prioridad alta es para los procesos que tienen mucho I/O, como P0 y P1, así no deben esperar a que P2 termine su turno para bloquearse nuevamente. Y en la cola más baja tenemos a P2, así puede aprovechar cuando tanto P0 como P1 están bloqueados para correr.

## Ejercicio 4

Los que pueden resultar en starvation son:

- **Por prioridad**: Si tenemos un proceso de prioridad baja, y se siguen encolando procesos de una prioridad mayor, nunca vamos a ejecutar el de prioridad baja, por lo que hay starvation.
- **SJF**: Misma situación, si tenemos un proceso que tarda mucho en terminar, y se encolan procesos con menor tiempo de ejecución, ocurre starvation del proceso más largo, pues nunca le dan CPU.
- **SRTF**: Idéntico a SJF, pero en vez de ser un proceso largo, es que le falte más tiempo en terminar que tiempo de ejecución de procesos nuevos encolados.
- **Colas Multinivel**: Exactamente idéntico a las prioridades.

## Ejercicio 5

Esta modificación haría que los procesos de cierta manera tengan una prioridad, pero manteniendo el fairness. Es decir, aquellos que estén encolados más veces, son aquellos que tienen una prioridad mayor, y por ende tienen un mayor quantum, y los que están encolados pocas veces, son aquellos que no se les da mucha CPU.

Esto trae como ventaja el hecho de las prioridades. Aquellas tareas que capaz no necesiten mucha CPU, pueden darse con una prioridad más alta así terminan más rápidamente que aquellas que si necesiten, además evitando la starvation, pues seguimos siendo un RR. Bajo esta implementación, siguen estando los cambios de contexto, aún sea para el mismo proceso, generando overhead innecesario, por lo que esto es una gran desventaja del modelo.

Para mantener las ventajas sin duplicar las entradas en la cola de procesos, podemos usar un esquema de prioridades, donde aquellas que tengan mayor prioridad, se le da un quantum mayor que a las que tengan menor prioridad.

## Ejercicio 6

 ### Ejercicio 6.A
 
[[Drawing 2026-09-03 13.50.09.excalidraw.md#^zvh9302v|6a FCFS Gantt]]