# Patrones de diseño

## ¿Qué son?

- Soluciones generales y reutilizables a problemas comunes que surgen al diseñar software.

- Al aplicar patrones, se facilita la creación de software más flexible, mantenible y *escalable*.

- **¡NO SIEMPRE SON LA MEJOR OPCIÓN!**

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
- [builder](builder.md)
- [prototype](prototype.md)
- [singleton](singleton.md)

### Estructurales

### Comportamineto
