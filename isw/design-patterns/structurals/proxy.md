# Proxy Pattern

## Intent

Proveer un sustituto o placeholder para otro objeto.

## Motivación

Crear objetos es una operación costosa, y queremos capaz poder postergar esta inicialización hasta cuando sea realmente necesario.

Pongamos el ejemplo de un editor de documentos, donde podemos adjuntar imágenes al documento. Estas imágenes pueden ser costosas de crear, pero necesitamos aún así que abrir el documento sea rápido.

Entonces, lo que podemos hacer, es evitar crear todas las imágenes juntas hasta que sean visibles para el usuario, e ir creando en ese caso una a una *on-the-go*.

Aún así, necesitamos tener un placeholder para las imágenes porque siguen siendo parte del documento. Para ello creamos un objeto `ImageProxy`, que actúe como intermedio para la imagen real y se encarga de instanciarla cuando se requiera.

Cada request hecha a la imagen será atendida de allí en adelante por la imagen real, mediando con el proxy de por medio.

## Cuándo usar?

- Cuando exista la necesidad de una referencia más versátil o sofisticada a un objeto más allá de un simple puntero.
- Existen varios tipos de proxy, que debemos utilizarlo según el caso:
	- **Remote Proxy:** Provee un representante local ara un objeto en otro address space.
	- **Virtual Proxy:** Crea objetos on demand. (caso de [#Motivación](#Motivación)).
	- **Protection Proxy:** controla el acceso al objeto original.
	- **Smart Pointers**: mantienen una cuenta de referencias al objeto real. Cuando llega a 0 se libera automáticamente. 

## Estructura

![[Pasted image 20261026143649.png]]

## Consecuencias

- Introduce un nivel de indirección cuando accedemos a un objeto.
- Puede esconder que un objeto vive en otro address space.
- Puede hacer optimizaciones como, por ejemplo, crear un objeto on demand.
- Permiten hacer más tareas de mantenimiento antes de acceder al objeto.

## Casos de uso conocidos

- ET++ con sus clases de text building block.
- NEXTSTEP con su NXProxy para representar objetos que pueden ser distribuidos.
- Un server crea proxies para objetos remotos cuando los clientes los piden.
- Proxies en Smalltalk para acceder objetos remotos.