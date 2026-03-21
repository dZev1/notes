# Builder

## Problema

Supongamos que tenemos un objeto complejo que requiere inicializaciones laboriosas de muchas propiedades y objetos anidados. Este código de inicialización está o en un constructor monstruoso con muchos parámetros o a lo largo de toda la codebase.

Supongamos que tenemos un objeto `Casa`. Para construir una casa necesitamos 4 paredes, un piso, una puerta, ventanas y un techo. Pero ahora, ¿si queremos una casa con un garage? ¿o que tal una casa con un jardín o instalación de calefacción?

Un primer approach que se puede ocurrir es **extender** la clase base `Casa` y crear un conjunto de subclases con todas las posibles combinaciones de los parámetros... YIKES.
Otro approach puede ser armar un constructor *enorme* con todos los parámetros posibles sobre la clase `Casa`. Eliminamos la gran cantidad de subclases, pero ahora agregamos otro problema. No vamos a usar la mayoría de los parámetros del constructor a la vez.

## Solución

El patrón **Builder** sugiere que extraigas el código de construcción del objeto por fuera de su clase a varios objetos llamados *builders*.

Con el ejemplo de la clase `Casa` podemos tener un `CasaBuilder` que tenga los métodos `construirParedes()`, `construirPuertas()`, `construirVentanas()`, `construirTecho()`, ..., `obtenerCasa(): Casa`. Para construir a `Casa` no hace falta llamar a todos los métodos, sino que solo vamos a necesitar llamar a aquellos que lo requieran para la configuración deseada.

Digamos ahora que una cabaña tiene paredes de madera y un castillo de piedra, bueno entonces cada uno tendrá su clase builder correspondiente que implemente los mismos pasos de construcción, pero de manera distinta. Esto se hace teniendo una interfaz en común entre cada `Builder`.

### Director

Es una clase aparte que define el orden en que hay que ejecutar los pasos de construcción, mientras que el builder provee la implementación de esos pasos. No es necesario tener un director, pero es un buen lugar para colocar las rutinas de construcción para volver a utilizarlas a lo largo del programa.

El director esconde completamente detalles de la producción al código cliente.

## Estructura

![[builder_structure.png]]