# Patrones de diseño

## ¿Qué son?

- Soluciones generales y reutilizables a problemas comunes que surgen al diseñar software.

- Al aplicar patrones, se facilita la creación de software más flexible, mantenible y *escalable*.

- **¡NO SIEMPRE SON LA MEJOR OPCIÓN!**

- Son HEURÍSTICAS

## ¿Por qué usarlos?

- Implementan soluciones sin la necesidad de tener que replantear las mismas para nuestro problema.

- Simplifican y acortan tiempo de desarrollo.

- Implementan soluciones ya validadas y comprobadas.

## ¿Contras?

- Pueden agregar complejidad a proyectos o problemas pequeños.

- Nos podemos olvidar que son adaptables a cada situación o escenario.

- Deberían servir para **facilitar** y **simplificar** soluciones de desarrollo, no **complejizarlo**.

## Tipos

### Creacionales

Conjunto de soluciones probadas para ayudarnos a **crear objetos** de manera eficiente, flexible y controlada.

En lugar de instanciar objetos directamente cada vez que se necesitan, se definen métodos y estructuras que guían el proceso de creación.

Resuelven problear relacionados a:

- cómo y cuándo crear objetos
- evitar código repetitivo o acoplado a clases concretas
- encapsular lógica de creación para que no esté distribuida en todo el sistema
- facilitar cambios a futuro

Se aplican cuando:

- necesitás crear muchos objetos similares o que son configurables
- la forma de creación puede cambiar con el tiempo
- el proceso de construcción puede ser complejo o tener muchos pasos
- querés evitar que el cliente conozca los detalles internos de cómo se crean los objetos
- buscar reducir acoplamiento del código

#### Algunos ejemplos

- [Factory Method](factory-method.md)
- [Abstract Factory](abstract-factory.md)
- [Builder](builder.md)
- [Prototype](prototype.md)
- [Singleton](singleton.md)

### Estructurales

Explican cómo clases y objetos se componen para formar estructuras más complejas. Usan herencia para componer interfaces o implementaciones.

Describen maneras de componer objetos para que estos realicen nuevas funcionalidades. Esta flexibilidad de la composición de objetos trae la habilidad de cambiar en tiempo de ejecución la composición de un objeto.

#### Algunos ejemplos

- [Adapter](adapter.md)
- [Composite](composite.md)
- [Decorator](decorator.md)
- [Proxy](proxy.md)

### Comportamineto

#### Algunos ejemplos

- [Null Object](null-object.md)
- [State](state.md)
- [Strategy](strategy.md)
- [Observer](observer.md)
- [Visitor](visitor.md)
- [Template Method](template-method.md)
- [Object Recursion](object-recursion.md)