# Composite Pattern

## Intent

Permitir componer objetos en estructuras de árbol y trabajar con estas estructuras como si fuesen objetos individuales.

## Motivación

En programas gráficos o de dibujo, el usuario puede construir diagramas complejos a partir de componentes simples. Luego, el usuario puede agrupar estos componentes para formar componentes más grandes que a su vez pueden agruparse en componentes aún más grandes.

Una implementación simple podría definir clases para las primitivas gráficas (Text, Line, ...) y otras clases para contenedores de esas primitivas.

El problema es que el código que use estas clases debe tratar a los objetos primitivos y contenedores de manera distinta, aun así cuando el usuario los trata de forma idéntica. Este patrón permite describir este modelo de manera tal  que mediante el uso de composición recursiva, los clientes no deban hacer la distinción entre contenedor y primitiva.

Para ello, usamos una *clase abstracta* que represente tanto a una primitiva como a un contenedor. En este caso sería una clase **Graphic**. Esta clase declara operaciones como `Draw`, que son específicas de los objetos gráficos. Declara además operaciones que todos los objetos compuestos comparten, como acceder o administrar los componentes hijos.

Entonces, primitivas como **Line**, **Rectangle**, **Text** implementan solamente `Draw` para dibujar líneas, rectángulos o texto respectivamente en pantalla, y no implementan nada relacionado con comportamiento de hijos.

Una clase **Picture** define un conjunto de objetos Gráficos, por lo que este implementa el método `Draw` que delega a cada uno de sus componentes hijos el método `Draw`.

## Cuándo usar

- Cuando tengamos que implementar una estructura de objetos con forma de árbol.
- Cuando queramos que el código cliente trate elementos simples y complejos de la misma forma

## Estructura

![[Pasted image 20261025195544.png]]

![[Pasted image 20261025195609.png]]

## Consecuencias

- Define clases jerárquicas que consisten de objetos primitivos y compuestos. Los primitivos se pueden componer en objetos más complejos, que pueden componerse recursivamente y así.
	- Esto permite al cliente usar tanto objetos primitivos como compuestos indestinguiblemente.
- Código cliente simple. El cliente no sabe qué tipo de objeto es, y no lo necesita saber, puede usar tanto la hoja como el compuesto.
- Permite facilidad a la hora de agregar nuevos componentes.
- Permiten hacer un mejor diseño.

## Usos conocidos

- Clase `View` de Smalltalk MVC.
	- `View` en la versión original de MVC actuaba como Component y como Composite.
- El framework RTL Smalltalk Compiler.
	- `RTLExpression` es un Component para parse trees.
	- `BinaryExpression` de RTLExpression que contiene otros `RTLExpression` como hijos.