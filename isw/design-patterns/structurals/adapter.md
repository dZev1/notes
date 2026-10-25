# Adapter Pattern

## Intent

Permitir la colaboración entre objetos con interfaces incompatibles.

## Motivación

A veces una clase "toolkit" diseñada para ser reusada no es reusable porque no cumple con la interfaz esperada en nuestro dominio de problema.

Supongamos en un programa de dibujo, tenemos elementos gráficos, como polígonos, líneas, texto, etcétera. El usuario puede dibujarlos en la pantalla y moverlos por ejemplo. Para esto usa el programa una interfaz **Shape**, que tiene una figura editable y puede dibujarse a sí mismo.

Clases de figuras geométricas elementales cumplen fácilmente esta interfaz (**PolygonShape**, **LineShape**, etc). Pero... qué sucede en el caso de una **TextShape**? Manejar texto es complicado, hay que meterse con manejos de buffers, screen updates, entre otras cosas. Por eso capaz nos convendría poder usar una clase *TextView* ya implementada, pero esta clase lo más seguro es que no esté hecha para ser un TextShape en un programa de dibujo.

Por lo tanto hay que adaptar la interfaz de TextView a una posible TextShape. Hay dos maneras de hacerlo:
1) En lenguajes que acepten **herencia múltiple**, podemos crear una clase adaptadora que herede tanto la interfaz de TextView como la interfaz de TextView y armar así las requests de TextShape con métodos/mensajes de TextView.
2) Usar un objeto adaptador, que tenga un **adaptee** como colaborador interno, el cual es el encargado de hacer la request específica que requiera la TextShape de la TextView.

## ¿Cuándo usar?

- Cuando queremos usar una clase existente, pero su interfaz no coincide con la de nuestro dominio de problema.
- Cuando queremos crear una clase reutilizable que coopere con clases no relacionadas o desconocidas.
- Cuando necesitemos usar múltiples subclases existentes, pero es impráctico adaptar su interfaz subclasificando cada una.

## Estructura

![[Pasted image 20261025005533.png]]

## Consecuencias

- **La adaptación puede variar**. Puede ser tan solo un renombre de interfaz, hasta crear funcionalidad nueva a partir de estos objetos.
- **Pluggable adapters**. Usados en Smalltalk, son utilizados porque una clase es más reutilizable cuando minimizamos la cantidad de suposiciones queotras clases hacen al usarla.
- **Two-way adapters para proveer transprencia total**.

## Usos conocidos

- ET++Draw, ejemplo de la motivación.
- InterViews 2.6.
	- Usa una clase abstracta Interactor para los elementos de interfaz, y define una clase abstracta Graphic para líneas, círculos, etc.
	- Ambos tienen apariencias gráficas, pero tienen distintas interfaces e implementaciones.
	- Usa un adapter llamado **GraphicBlock** que adapta la interfaz de Graphic a Interactor.
- Pluggable Adapters en Smalltalk.
	- ValueModel class para mostrar single values. Protocolo `value, value:`.
	- Los desarrolladores acceden al `value` a través de mensajes que estén más dentro de su dominio de problema, digamos `width, width:`, pero no deberían ser subclases de ValueModel para adaptar estos nombres domain-specific a la interfaz de ValueModel.
	- Usa **PluggableAdaptor** para parametrizar con closures como getters y setters para value.