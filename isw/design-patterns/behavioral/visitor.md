
# Visitor Pattern

## Intent

Representar una operación a realizar sobre elementos de una estructura de objeto. Permite definir una nueva operación sin cambiar las clases de los elementos sobre los que opera.

## Motivación

Considerar un AST, tenemos nodos que representan `Statements` de los cuales tenemos distintos tipos de Statements:
- **ExprStmt**: que subclasifica en:
	- BinaryExpr
	- UnaryExpr
	- ConditionalExpr
	- CaseExpr
	- ...
- **LetStmt**: para asignar variables.

Cada uno de estos nodos de la estructura de un AST, se comporta distinto a la hora de evaluar o parsear. Y no queremos estar haciendo type checking para invocar a un  método `pretty_print()` que imprima el AST. Por lo tanto podemos hacer una clase abstracta `ASTVisitor` que se encargue de visitar a todos los nodos, mediante un mensaje `visit()` que cada uno de los nodos del AST sabe responder, que acepte, entre otros visitantes, un `ASTPrinter` y desde allí, mediante un double dispatch, el printer imprimirá la representación concreta de cada nodo.

## Cuándo usar

- Cuando la estructura de un objeto contiene muchas clases de objetos que difieren en interfaces, y queremos realizar operaciones sobre estos objetos que dependen de sus clases concretas.
- Cuando operaciones sin relación y distintas tienen que realizarse sobre objetos en la estructura de un objeto, sin contaminar sus clases con estas operaciones.
- Las clases que definen la estructura del objeto raramente cambia, pero que queremos definir nuevas operaciones sobre su estructura.

## Estructura
![[Pasted image 20261027101537.png]]

## Consecuencias

- Podemos añadir operaciones fácilmente.
- Junta operaciones relacionadas y separa las que no.
- Añadir clases de elementos concretos es difícil, porque tenemos que extender implementar nueva funcionalidad en la interfaz de nuestro Visitor abstracto, que lleva a tener que reimplementar el método en cada uno de los ConcreteVisitor.
- Permite visitar sobre jerarquías de clases.
- Se acumula el estado. 
- Rompemos encapsulamiento.

## Known Uses

- Cualquier tipo de parser hecho en algún lenguaje con elementos de POO.
- ProgramNodeEnumerator en Smalltalk-80.
	- Usado por algoritmos que analizan el código fuente.
- Iris Inventor
	- Representación de escenas 3D como jerarquías de nodos.
	- Renderizar o mappear un evento de input requiere un visitor para atravesar todos los nodos.