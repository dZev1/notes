# Abstract Factory

## Problema

- Tenemos una clase que representa una familia de productos relacionados
	- Ejemplo: `Chair`, `Sofa`, `CoffeTable`
- Además tenemos variantes de esta familia.
	- Ejemplo: `Chair` +  `Sofa` + `CoffeeTable` están disponibles en las variantes
		- `ArtDeco`
		- `Victorian`
		- `Modern`

![[abstract_factory_ex.png]]

- Necesitamos una manera de crear objetos de muebles individuales para que matcheen con otros objetos de su misma familia.
- No queremos cambiar el código existente al añadir nuevos productos o familias de productos al programa.

## Solución

1. Declarar interfaces para cada producto distinto de la familia, como puede ser `Chair, Sofa, CoffeeTable`.

```Java
public interface Chair {
	public bool hasLegs();
	public void sitOn();
}
```

Luego, hacemos una clase para cada variante del producto que siga esa interfaz. Para el caso de la silla, tendríamos que implementar `VictorianChair`, `ModernChair`, `ArtDecoChair`.

```Java
public class VictorianChair implements Chair {
	//...
	public bool hasLegs() {
		return true;
	}
	
	public void sitOn() {
		this.occupied = true;
	};
}

public class ModernChair implements Chair {
	//...
	public bool hasLegs() {
		return true;
	}
	
	public void sitOn() {
		this.occupied = true;
	};
}
```

2. Luego, debemos declarar la **Abstract Factory**.
	- Una AF es una interfaz con una lista de métodos de creación para todos los productos que son parte de la familia:
		- `createChair()`
		- `createCoffeeTable()`
		- `createSofa()`
	- Estos métodos deben retornar un tipo abstracto de producto, representado por las interfaces extraídas previamente.

```Java
public interface AbstractFurnitureFactory {
	public Chair createChair();
	public CoffeeTable createCoffeeTable();
	public Sofa createSofa();
}
```

3. Creamos las clases fábricas para la creación de las distintas variantes de productos:

```Java
public ModernFurnitureFactory implements AbstractFurnitureFactory {
	public Chair createChair() {
		return new ModernChair();
	}
	
	public CoffeeTable createCoffeeTable() {
		return new ModernCoffeeTable();
	}
	
	public Sofa createSofa() {
		return new ModernSofa();
	}
}
```

y así para todas las demás variantes.

- Normalmente, se declara un campo para la clase Cliente donde se inyecta el tipo de Abstract Factory a ser utilizada.

```Java
public class Client {
	private AbstractFactory factory;
	
	public Client(AbstractFactory f) {
		this.factory = f;
	}
}
```

## Estructura

![[abstract_factory_structure.png]]

## ¿Cuándo uso Abstract Factory?

- Cuando tu código necesita trabajar con varias familias de productos relacionados, pero no queremos que dependan de de clases concretas de esos productos.
	- Pueden ser desconocidos en un principio o por extensibilidad de la codebase.
- Provee al usuario con una interfaz para crear objetos de cada clase de una familia de productos.
	- Siempre y cuando el código cree objetos mediante la interfaz, no necesitamos preocuparnos por crear la variante equivocada de una variante de producto no deseada.
- Considerar implementar Abstract Factory cuando tenemos un set de Factory Methods que difusa la responsabilidad principal.
	- Cuando una clase se enfrenta a multiples tipos de producto, lo mejor es extraer sus factory methods a una factory class standalone o a una Abstract Factory.

## Pros & Cons

### Pros

- Garantiza que los productos de una misma fábrica son compatibles entre sí.
- Esquiva el coupling entre productos concretos y el código del cliente.
- Se cumple el Principio de Responsabilidad Única.
- Se cumple el Principio Open/Closed.
	- Podemos crear nuevas variantes sin romper código existente.

### Cons

- El código se puede volver más complicado de lo necesario, pues con el patrón se introducen demasiadas nuevas interfaces y clases.

## Relación con otros patrones

- Muchas veces los diseños comienzan con un **Factory Method** y luego este evoluciona hacia un **Abstract Factory**, un **Prototype** o un **Builder**.
- Las clases **Abstract Factory** están basadas en un conjunto de **Factory Methods**, pero también se puede usar **Prototype** para componer los métodos en estas clases.
- **Abstract Factories**, **Builders** y **Prototypes** pueden ser implementadas como **Singletons**