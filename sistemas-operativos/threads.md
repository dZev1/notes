# Threads

## ¿Qué es un hilo (thread)?

Es una abstracción sobre un solo proceso. En vez de un solo punto de ejecución en un programa, los programas *multi-threaded* tienen más de un punto de ejecución. Es decir, tienen más de un PC, cada uno haciendo fetch y ejecución de instrucciones distintas).

Se los puede pensar como procesos distintos, pero con la diferencia que **comparten el espacio de direccionamiento** y, por lo tanto, **acceden a los mismos datos**.

Cada thread tiene su propio register file que usa para computar. Entonces, si dos threads corren en el mismo procesador, cuando se cambia de un thread a otro, hay que hacer un *context switch*. Para guardar los estados de los threads, usamos en vez de PCBs, *TCBs*, o **Thread Control Block**. Como se mantiene el mismo espacio de direccionamiento, el context switch entre threads no debe cambiar las páginas de la Page Table que se está usando.

El stack no se comparte, sino que cada thread tiene su propio stack.

## ¿Por qué usarlos?

Dos razones principales

### Paralelismo

Imaginemos que estamos escribiendo un programa que realiza operaciones en arrays muy grandes. Si corremos el programa en un solo procesador, la tarea es sencilla, hacer cada operación y listo. Pero, si llegásemos a correrlo en un sistema con multiples procesadores, podríamos correr las operaciones en menor tiempo, haciendo que cada procesador haga una parte del proceso.

Transformar un programa *single-threaded* a un programa que hace este tipo de trabajos en CPUs múltiples se llama **paralelización**, y usando un thread por CPU para hacer que esto funcione es la manera natural y típica de hacer que los programas corran más rápido con el hardware actual.

### Evitar Bloqueos

Imaginemos que estamos escribiendo un programa que hace diferentes tipos de I/O, ya sea esperar para mandar o recibir un mensaje, un I/O a disco para completar, o resolución de un page fault.

En vez de esperar, el programa quisiera poder hacer otra tarea, incluyendo utilizar el CPU para computar, o hacer más requests I/O. El threading permite el overlapping de I/O con otras actividades en un solo programa, algo símil a lo que hacía la **multiprogramación** para procesos a través de los programas. Como resultado, muchas aplicaciones server-based usan threads en sus implementaciones.

## Thread API

### Creación de threads

Es POSIX, se usa:

```c
#include <pthread.h>

int pthread_create(
	pthread_t            *thread,
    const pthread_attr_t *attr,
    void           *(*start_routine)(void*),
    void           *arg
);
```

Donde `thread` es un puntero a una estructura de tipo `pthread_t`. Usamos esta estructura para interactuar con el thread, y así tenemos que pasarla a `pthread_create` para inicializarla.

`attr` especifica los atributos que tiene el thread. Un atributo se inicializa con otra llamada a `pthread_attr_init()` (ver `man pthread_attr_init` para más detalle). En muchos casos vamos a usar los defaults, y/o pasamos `NULL`.

El tercer argumento es aquel que dice qué función debería correr ese thread. Por último `arg` son los argumentos que se debe pasar a la función `start_routine`. Se usan `void` pointers para poder pasar cualquier tipo de valor.

### Completar Threads

¿Qué pasa si queremos esperar a que un thread termine su cómputo? Usamos una llamada a `pthread_join()`:

```c
int pthread_join(
	pthread_t thread,
	void **value_ptr
);
```

Esta rutina pide dos argumentos. El primero es el thread al cual queremos esperar. El segundo argumento es un puntero al valor de retorno que esperamos cuando termine el cómputo del thread.

### Locks

Para exclusión mutua tenemos funciones para bloquear las secciones críticas usando **locks**. Esto se hace con las llamadas a las siguientes rutinas:

```c
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

Para inicializar locks tenemos dos maneras. La primera es con `PTHREAD_MUTEX_INITIALIZER` de la siguiente manera:

```c
ptrhead_mutex_t lock = PTHREAD_MUTEX_INITIALIZER
```

Hacer esto setea el lock a sus valores default y hace al lock utilizable.

La manera dinámica de hacerlo es usando la llamada `pthread_mutex_init()`, de la siguiente manera:

```c
int rc = pthread_mutex_init(&lock, NULL);
assert(rc == 0);
```

El primer argumento es la dirección al lock en sí mismo, el segundo una lista de argumentos opcionales. Preferimos usar los locks dinámicos frente a `PTHREAD_MUTEX_INITIALIZER`. Debemos recordar también de usar `pthread_mutex_destroy()` luego cuando terminamos de usar el lock.

Tenemos también dos otras rutinas:

```c
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_timedlock(pthread_mutex_t *mutex, struct timespec *abs_timeout);
```

Estas dos llamadas se usan para adquirir los locks. `trylock` falla si el lock ya está en uso, y `timedlock` retorna si hay un timeout o después de adquirir el lock, lo que suceda primero. Debemos evitar usar estas llamadas... ok?

### Condition Variables

Son útiles para cuando se necesite usar algun tipo de método de señales (semáforos) entre threads. Las llamadas son dos principalmente:

```c
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *cond);
```

Para usarlas, se necesita además un lock que esté asociado a la condición. Cuando se llama a estas rutinas, debemos tener control del lock.

`pthread_cond_wait` pone al thread llamador en `sleep`, y espera a que otro thread mande la señal. Además de mandar a dormir al thread, `wait` libera el lock.

### Compilar y correr

Para compilar el código debemos haber incluido el header `pthread.h` en nuestro código, y luego linkearlo con la flag `-pthread`.