# Administración de Memoria

- El SO tiene un módulo dedicado al manejo de la memoria.
	- Maneja el espacio libre y ocupado.
	- Asigna y libera memoria.
	- Controla el swapping.
- El swapping solo ocurre con procesos que **no** se están ejecutando.
- Evitamos el swapping lo más que se pueda.
- Si llega a haber, 

- Problemas
	- **Fragmentación**
	- **Reubicación**
	- **Protección**
	- **Manejo del espacio libre**

## Organización de memoria

### Segmentación

- Segmentamos para evitar fragmentación en 4 segmentos importantes
	- Stack creciendo en sentido negativo.
	- Heap creciendo en sentido positivo.
	- Data donde guardar variables globales sin inicializar y estáticas
	- Texto el código del programa.
- Lo del stack y el heap creciendo de manera contraria es lo que evita la fragmentación

### Bitmap

- Usamos un bitmap, dividido en bloques de igual tamaño.
- Cada posición del bm, es un bloque, que puede estar ocupado o libre.
- Asignar y liberar es sencillo, encontrar bloques consecutivos requiere una búsqueda lineal.
- No es usado.

### Lista doblemente enlazada
- Nodos son procesos o bloques libres. Tienen su tamaño del bloque y sus límites.
- Liberar en O(1).

>[!Important] Muchos sistemas combinan ambas!

- Asignar en una lista:
	- **First Fit**: asignamos en el primer hueco libre donde entre.
		- Es rápido.
		- Tiende a fragmentar la memoria.
	- **Best Fit**: asignamos en el hueco donde entre más justo.
		- Más lento
		- Llena la memoria de bloques chicos que no sirven.
	- **Quick Fit**: tenemos una lista de bloques libres y de los tamaños solicitados con frecuencia.
- Todo eso falla. Hay que mezclar!
	- Algunos producen fragmentación externa, y otros interna.

## Reemplazo de página

- Algoritmos
	- **FIFO**
	- **Second chance**
		- Como FIFO, pero si la página se está usando, la salteo en una primera vuelta.
		- Mucho mejor que FIFO.
	- **Not Recently Used (NRU)**
		- Aquellas páginas que no se referenciaron ni modificaron son lo mejor para desalojar.
		- Basado en el principio de localidad.
	- **LRU**
- Se usan todos a la vez!
- Se desaloja y aloja en bloques.

- Si no hay memoria y hay mucha competencia entre muchos procesos para usarla, el SO se la pasa cambiando páginas de memoria a disco y a memoria a disco y ...
- Esto es **THRASHING**.

- Cuando hacemos `fork()`, se duplica todo el programa, pero **no** todas las páginas de memoria.
- En su lugar, ambos procesos usan las mismas páginas, hasta que alguno de ellos escribe en las mismas.
- Una vez que ocurre eso, se duplican y cada uno se queda con una copia independiente.