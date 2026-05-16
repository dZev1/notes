# Free-Space management

Controlar el espacio libre puede ser fácil, como sucede en la **paginación**, pues el espacio que estamos manejando está dividido en espacios de tamaño fijo. En este caso guardamos en una lista los tamaños fijos de cada uno de las divisiones. Cuando pidamos una, devolvemos la primera entrada.

El manejo se vuelve más complicado cuando el espacio libre consiste de lugares en memoria de tamaños variables. Esto surge cuando usamos, como usuarios, librerías de manejo de memoria como `malloc()` o `free()` o cuando, como SO, maneja la memoria física utilizando **[[segmentacion#Segmentación|segmentación]]** para implementar la memoria virtual.

Lo que sucede en estos casos se llama **fragmentación externa**. El espacio libre se divide en pequeños fragmentos de distintos tamaños y, por ende, fragmentado. Solicitudes subsecuentes pueden fallar porque no hay un espacio contiguo como un solo bloque que pueda satisfacer la solicitud, aunque el espacio total de memoria libre si lo hace.

### Suponemos:

- Interfaz básica como la que provee `malloc()` y `free()`.
	- Específicamente, `void *malloc(size_t size)` toma el parámetro `size`, que toma el tamaño en bytes a alojar en memoria, y devuelve un puntero sin tipo específico, es decir un **void pointer**.
	- `void free(void *ptr)` toma un puntero como parámetro y libera ese correspondiente trozo de memoria. La librería va a tener que llevar cuenta de cuál es el tamaño pedido por el puntero.
	- El espacio que maneja la librería es conocida como `heap`. La estructura de datos genérica que se usa para manejar el espacio libre en el heap es llamada **free list**, la cual contiene referencias a todos los lugares libres de la region de memoria correspondiente.
- Los allocators no sufren de *fragmentación interna*.
- Cuando la memoria se da al cliente, no puede volver a alojarse en otra ubicación de la memoria. Entonces no puedo compactar el espacio libre, lo que sería útil para combatir la fragmentación.
- El allocator maneja un espacio contiguo de bytes. En algunos casos, el allocator puede pedir que esta region manejada crezca. Vamos a suponer que esto no sucede, y que la región tiene tamaño fijo.

## Mecanismos de bajo nivel

### Splitting and Coalescing

Una free list es un conjunto de elementos que describen el espacio libre en el heap. Entonces, una free list se vería algo como:

$$
head \longrightarrow \{addr: 0, len: 10\} \longrightarrow \{addr: 20, len: 10\} \longrightarrow NULL 
$$

Un request de más de 10 bytes, va a fracasar, devolviendo NULL. No hay un espacio contiguo de memoria con ese tamaño. Una solicitud de exactamente 10 bytes, si se puede satisfacer fácilmente por cualquiera de los dos segmentos libres de memoria. ¿Pero qué hacemos si pedimos algo menor a 10 Bytes?.

En ese caso el allocator va a realizar un **splitting** en el fragmento de memoria. Va a encontrar un fragmento libre de memoria que satisfaga el pedido. Una vez encontrado, lo va a dividir en dos: un trozo de tamaño pedido, el cual va a ser devuelto por la función llamadora del allocator, y el otro seguirá en la lista.

Si pidiésemos 1 byte a la free list de arriba, la free list se actualizaría a esto:

$$
head \longrightarrow \{addr: 0, len: 10\} \longrightarrow \{addr: 21, len: 9\} \longrightarrow NULL 
$$

Un mecanismo que surge como corolario al splitting, es el **coalescing** del espacio libre. Tomemos el ejemplo de arriba, que tiene 10 bytes en uso desde en posiciones [10-19] ¿Qué pasa si hacemos `free(10)` o cualquier posición en uso que esté en el medio del heap? Si solamente lo agregamos a la free list sin pensar mucho, podríamos llegar a tener una lista así:

$$
head \longrightarrow \{addr: 10, len: 10\} \longrightarrow \{addr: 0, len: 10\} \longrightarrow \{addr: 20, len: 10\} \longrightarrow NULL 
$$

El problema acá es que si bien todo el heap está libre, parece que nos quedó fragmentado en 3 pedazos de 10 bytes cada uno. Entonces, si hacemos una solicitud de 20 bytes, una búsqueda a través de la lista no va a encontrar ese trozo de 20 bytes.

Para evitar esto, el allocator va a combinar el espacio libre cuando el trozo de memoria se libere, en caso de que sea posible. Cuando liberamos un trozo de memoria, vemos sus addresses cercanas y si hay ubicaciones libres a su alrededor, combinamos esas ubicaciones libres con el trozo liberado, así dando un solo trozo grande de memoria libre. Quedando la free list como:

$$
head \longrightarrow \{addr: 0, len: 30\}\longrightarrow NULL 
$$

Para mantener un registro del espacio alojado en memoria, `malloc()` no pide solo los bytes pasados por parámetro, sino que pide unos bytes extra que los usa de header, para guardar el tamaño del espacio alojado y un número "mágico" que sirve para checkear integridad, y otras informaciones:

```c
typedef struct {
	uint64_t size;
	uint64_t magic;
} header_t;
```

Cuando llamamos a `free(ptr)`, la librería simplemente usa aritmética de punteros para saber dónde comienza el header:

```c
void free(void *ptr) {
	header_t *hptr = (header_t *) ptr - 1;
	...
}
```

Después de obtener ese puntero al header, podemos simplemente determinar si el número mágico es el esperado como un sanity check (`assert(hptr->magic == 1234567)`) y calcular así el tamaño total de la región liberada con matemática simple (sumando el tamaño de la región al tamaño del header). Entonces, cuando pedimos $N$ bytes usando `malloc()`, estamos pidiendo realmente $N + sizeof(header)$ bytes.

## Armando la Free List

Cuando queremos armar nodos de la free list, deberíamos poder alojarlo con `malloc()`... pero no podemos hacer eso en la propia librería que maneja la memoria. Tenemos que construir la lista *dentro* del espacio libre mismo.

Para hacer esto, tenemos que inicializarla primero con una sola entrada del tamaño del heap (digamos 4KB o 4096 Bytes). Esta es la descripción de un nodo:

```c
typedef __node_t {
	int size;
	struct __node_t *next;
}
```

Ahora veamos un fragmento de código que inicialice el heap y ponga el primer elemento de la free list en ese espacio. Asumimos que el heap se construye sobre espacio libre adquirido por llamada a la syscall `mmap()`. Acá está el código:

```c
node_t *head = mmap(NULL,
	4096,
	PROT_READ | PROT_WRITE,
	MAP_ANON | MAP_PRIVATE,
	-1,
	0
);
head->size = 4096 - sizeof(node_t);
head->next = NULL;
```

Después de correr este código, tenemos una lista con una sola entrada de tamaño 4088. El puntero `head` contiene la dirección de comienzo de este rango.

## Estrategias para manejo de memoria libre

### Best Fit

Iterar sobre la free list hasta encontrar un chunk de memoria de igual o mayor tamaño que el que queremos alojar, luego de entre estos, devolvemos el fragmento más pequeño.

Trata de reducir el espacio malgastado, pero trae performance penalty cuando se hace una búsqueda exhaustiva del bloque correcto.

### Worst Fit

Totalmente opuesto a Best Fit: buscamos el chunk de memoria más grande y devolvemos la cantidad solicitada, manteniendo el resto del trozo de memoria en la free list. Esto sigue requiriendo una búsqueda exhaustiva y por lo tanto es costoso. Peor aún, da peor performance y fragmentación excesiva con altos overheads.

### First Fit

Buscamos el primer bloque de la free list que tenga un tamaño mayor al solicitado y devolvemos la cantidad solicitada. El espacio libre restante se mantiene libre para solicitudes siguientes.

Es más rápido, no hay que hacer búsqueda exhaustiva, pero infesta el comienzo de la free list con objetos pequeños. Además ahora hay que ver cómo el allocator maneja el orden de la free list.

### Next Fit

En vez de comenzar la búsqueda del first fit desde el comienzo siempre, next fit mantiene un puntero extra a la ubicación del último lugar de la lista que se miró y devolvió. La idea es expandir las búsquedas equitativamente del espacio libre. En performance es similar a first fit, y se evita una búsqueda exhaustiva.

### Otros approaches

- Segregated lists
- Buddy Allocation
- ...
