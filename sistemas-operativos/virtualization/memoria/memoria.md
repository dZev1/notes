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

## Traducción de direcciones

Vamos a apoyarnos en el hardware para conseguir la mayor eficiencia y control posible. Esto va a comenzar muy simple (registros) y luego se va a complejizar bastante (TLB, page tables, etcétera).

El control implica que el SO es el que se asegura que una aplicación no acceda a otra memoria más que la suya propia, por lo que para proteger tanto a las aplicaciones entre sí, como al SO de las aplicaciones, también vamos a necesitar apoyo del hardware.

Vamos a querer que los programas puedan usar sus address spaces como quieran, haciendo al sistema más fácil de programar.

### Hardware-based address translation

El hardware va a transformar cada acceso a memoria, cambiando su dirección virtual dada por la instrucción en la dirección física donde la información está realmente ubicada. Entonces en cada referencia a memoria, el hardware hace una traducción a su lugar real en la memoria física.

El SO debe involucrarse en momentos clave para configurar el hardware para hacer la traducción correcta. Debe entonces hacer **memory management**, manteniendo un registro de cuáles localizaciones están libres y cuales en uso (una cache).

### Asumimos

- Address space del usuario contiguo en memoria física.
- Address space no muy grandee, es menor que el tamaño de la memoria física.
- Cada address space tiene el mismo tamaño.

Luego romperemos estas suposiciones para ir a lo real.

## Reubicación dinámica (por hardware)

- ¿Cómo reubico un proceso en memoria de forma que sea transparente para el proceso?
- ¿Cómo damos la ilusión de que el espacio virtual de memoria empieza en 0, cuando en realidad comienza en otra ubicación de la memoria física?

Necesitamos dos registros en cada CPU:
El **base** register y el **bounds** (o **limit** register). Con este par podemos ubicar dónde queramos las cosas en el address space, sabemos dónde empieza y termina.

Con esta configuración, el SO decide dónde cargar el programa en la memoria física, y setea el **base** register en ese valor. Ahora, cuando cualquier referencia de memoria, generada por el proceso, se traduce de esta manera:
$$
DirecciónFisica = DirecciónVirtual + Base
$$
Este proceso de transformar una dirección virtual en una física es la **traducción de direcciones**, y como este mecanismo se da en tiempo de ejecución, y porque podemos mover address spaces hasta una vez que el programa esté corriendo, se conoce a este mecanismo como **Reubicación Dinámica**.

El **bounds** register, lo que hace es fijar un fin al address space del programa, a fin de proteger los procesos. Si una dirección está entre $Base$ y $Base + Límite$ (o $Base$ y $Límite$ si se guarda la dirección física de donde termina el address space).

Estos registros son parte del hardware para ayudar a traducir las direcciones, y forman parte de la **Memory Management Unit** o (**MMU**), junto a otros circuitos más. La rMMU debe poder ser manejada por el SO en modo kernel y, la CPU debe poder generar excepciones en caso de que un proceso quiera acceder a direcciones de memoria ilegales, para que el SO decida qué hacer ante esto.

## Manejo del espacio libre


