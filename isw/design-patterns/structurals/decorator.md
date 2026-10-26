# Decorator Pattern

## Intent

Agregar responsabilidades adicionales a un objeto de forma dinámica. Proveen una alternativa flexible a la subclasificación por extensibilidad funcional.

## Motivación

A veces queremos agregar responsabilidades a objetos individuales, pero no a una clase entera. Supongamos un toolkit para UI, el cual debería permitir al usuario agregar propiedades como bordes, o comportamientos como scrollear en componente de la UI.

Una forma de agregar responsabilidades es mediante la herencia, lo cual es inflexible, porque la elección se hace estáticamente. Un cliente no puede controlar cómo ni cuándo decorar el componente con el borde.

Una manera más flexible es encapsular al componente en otro objeto que añada el borde. Este componente es el **Decorator**, que implementa la interfaz del componente que decora de manera tal que su presencia sea transparente a la clase de los componentes del cliente. Reenvía las requests del componente y puede realizar acciones adicionales antes o luego del reenvío. Esta transparencia permite _anidar decoradores recursivamente_, permitiendo infinitas responsabilidades agregadas.

## Para qué/cuándo usar

- Agregar responsabilidades de manera dinámica y transparente a objetos individuales.
- Para responsabilidades que pueden ser quitadas.
- Cuando extensión mediante subclasificación deja de ser práctico.
	- Un gran número de extensiones independientes puede ser posible, que produce una explosión de subclases que soporten cada combinación posible.

## Estructura

![[Pasted image 20261025220010.png]]

![[Pasted image 20261025220109.png]]

## Consecuencias

- Más flexibilidad que herencia estática.
	- Con decoradores, las responsabilidades puede añadir o quitar responsabilidades en tiempo de ejecución acoplando o desacoplando decoradores.
- Ofrece un approach *paga mientras pasas* para agregar responsabilidades.
	- A partir de una clase simple, se le puede añadir con decoradores.
- Un decorador y su componente no son idénticos.
	- Desde una el punto de vista de la identidad de un objeto, un componente no es idéntico al objeto decorado. Entonces, no deberíamos depender de la identidad de los objetos para los decoradores.
	- Sumo a esto que hay pérdida de self al hacer forwarding. Si `D` es decorado por `W` y tenemos el mensaje `m1` que hace en algún momento `self algo`, y luego a esto es decorado por `W`, ahora `self algo` es una instancia de `W`, no de `D`.
- Hay muchos objetos pequeños.
	- Muchos objetos pequeños que se parecen.
## Usos conocidos

- Smalltalk class library.
- PassivityWrapper de ParcPlace Smalltalk.
	- Habilita o deshabilita interacciones de usuario con el componente.
- **AbstractStream** → *MemoryStream* | *FileStream*
	- **AbstractStream** implementa las operaciones `PutInt()` `PutString()` y define un método abstracto `HandleBufferFull()`.
	- Podemos tener un decorador para el stream que se encargue de que cuando se llene el buffer del MemoryStream o del FileStream, comprima los datos almacenados en el buffer del stream (`CompressingStream←StreamDecorator`).
