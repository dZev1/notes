# Memoria

## Multiprogramación y Time sharing 

Tenemos multiples procesos que están listos para correr en un tiempo dado, y el SO alterna entre ellos, como en el caso de un I/O. De esta manera ganamos una utilización de CPU efectiva.

Luego de un tiempo, el usuario demandó programas interactivos, y así nació el Time Sharing. Una manera de implementar el time sharing es corriendo un proceso por un tiempo corto, dándole acceso completo a la memoria y luego pararlo, guardar todo su estado en algún disco, incluyendo toda su memoria física, cargar el estado de otro proceso, correrlo por un rato, y así implementar un sharing bruto en la máquina.

El problema es que esto sería muy lento, más a medida que crece la memoria. Guardar y cargar de registros es rápido, pero guardar en un disco es realmente lento. Así que busquemos otra forma, una en la que podamos cambiar el proceso rápidamente, dejando procesos en memoria para que sea eficiente el time sharing. Esto trae problemas de **protección** que debemos resolver.

## Espacio de direccionamiento

Necesitamos una abstracción fácil de usar de la memoria física. Llamamos a esta abstracción *espacio de direccionamiento* o *address space*, y es la vista que tiene el proceso que está corriendo sobre la memoria del sistema.

El address space de un proceso contiene todo el estado de un proceso del programa que está corriendo. Por ejemplo, el **código** del programa tiene que vivir en memoria, por lo que está dentro del address space. También así el programa usa un **stack** para guardar sus direcciones de retorno de llamados a funciones, valores de variables globales, y pasar parámetros a funciones y valores de retorno. Por último usa el **heap** para aquellas entidades que requieran alojo dinámico, como una llamada a `malloc()` en C, o el uso de `new` para lenguajes orientados a objetos como Java.

Al heap y al stack los ponemos en lados opuestos del address space, ambos creciendo hacia el mismo sector, en direcciones opuestas: el heap positivamente y el stack negativamente.

Obviamente esto es una descripción de la abstracción al usuario del address space, El programa está cargado en alguna dirección arbitraria de la memoria física. Y aquí donde surge el problema de cómo virtualizar la memoria. ¿Cómo hago que si yo quiero cargar el programa en el address 0 Kb de memoria **virtual**, el SO cargue el programa que en realidad está en 320 Kb de la memoria física?

## Objetivos

Buscamos **transparencia**. El SO debe implementar memoria virtual de manera tal que es invisible para el programa corriendo. Así el programa no debería saber que su memoria está virtualizada, sino que el programa actúa como si tuviese su propia memoria física privada. Por detrás el SO y el hardware hacen todo el trabajo de multiplexar la memoria entre los diferentes trabajos, implementando la ilusión.

En segundo lugar buscamos **eficiencia**. El SO debe implementar lo más eficientemente posible la virtualización, en terminos de tiempo y espacio. El SO deberá apoyarse en el hardware para hacer esto, con dispositivos como la TLB.

Por último, buscamos la **protección**. El SO debe asegurarse de proteger tanto a procesos de otros, como al SO de sus procesos. Cuando un proceso hace un load, un store, o un instruction fetch, no debe ser posible acceder o afectar los contenidos de memoria de otros procesos o del SO en sí mismo. Entonces da la ilusión de aislamiento entre procesos. 

## Memory API

### Tipos de memoria

Empecemos con el **stack**, los alojos y desalojos de este tipo de memoria los maneja implícitamente el compilador por nosotros. Es por eso que a esta memoria se la refiere como memoria *automática*.

Es fácil de declararla, tan solo dentro de una función declaramos una variable nueva, es decir:

```C
void func() {
	int x; // declara una variable entera en el stack
}
```

El compilador se encarga del resto. Cuando retornamos del llamado de la función, el compilador desaloja completamente esta memoria para nosotros y, por ende, si queremos guardar el valor más allá del llamado, no es buena idea usar este tipo de alojo, el cual tiene un *scope local*.

Para este tipo de tareas, que requieren datos que vivan en memoria por un tiempo mayor usamos la memoria del **heap**. En este tipo de memoria los alojos y desalojos de memoria los maneja el programador explícitamente, no el compilador. Para hacer esto tenemos llamados especiales. Veamos este caso:

```c
void func() {
	int *x = (int *) malloc(sizeof(int));
}
```

Acá estamos alojando un entero en el heap. El llamado a `malloc()` devuelve el puntero en caso de que haya tenido éxito y `NULL` en caso de que haya habido algún error. En el snippet se están teniendo ambos tipos de declaración, pues se está guardando el puntero al área del heap donde se guarda el valor `&x` en la variable local `x`.

### Llamada a `malloc()`

`malloc()` es simple. Se le pasa como argumentos el tamaño, en bytes, que queremos alojar en el heap, y luego si tiene éxito nos devuelve el valor del puntero al comienzo del espacio alojado, o `NULL` en caso de que no haya tenido éxito.

```C
#include <stdlib.h>

void *malloc(size_t size);
```

es la firma de `malloc()`. No se le suele pasar el valor numérico de tamaño, sino que se suele usar `sizeof(type)` para hacerlo. Además `malloc()` retorna un puntero vacío. Hacer el cast al tipo de puntero que queremos no es realmente necesario, pero es buena práctica para demostrar que *"sabes lo que estás haciendo"* (no).

### Llamada a `free()`

Esta llamada libera del heap el puntero que usamos para alojar nuestros datos. Lo más difícil es saber cuándo alojar, y cuando desalojar la memoria. La rutina solamente toma un argumento: el puntero que había sido devuelto por `malloc()`.