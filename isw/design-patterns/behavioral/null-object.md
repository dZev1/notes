# Null Object Pattern

## Intent

Proveer un sustituto de otro objeto, que cumple con la misma interfaz, pero no hace nada. Encapsula las implementaciones de _cómo no hacer nada_ y esconde esos detalles de los colaboradores.

## Motivación

A veces una clase requiere un colaborador que no necesita que el colaborador haga algo; pero aún así queremos tratar a este colaborador como uno más que provea comportamientos.

Supongamos un modelo MVC, donde haya una clase View que necesite una instancia de Controlador para funcionar. Supongamos que una instancia particular es read only, y por lo tanto no necesita controlador. ¿Qué le pasamos como instancia de controlador? 

Una manera de solucionar este problema es poner a `nil/null` como la instancia de ese colaborador. Esto no funcionará bien porque la clase sigue mandando mensajes que solo conoce la clase de Controlador. Como la clase de `nil` no conoce esos mensajes, tendríamos que checkear si el controlador es `nil` antes de enviar el mensaje. Esto ensuciaría el código de checkeos condicionales, como `controller ifNil: [^self error: 'un mensaje de error'] ifFalse: [^ejecutarCaminoNormal]`.

Otra manera consistiría en usar un Controlador read-only, de esta manera ignoraría el input. El tema es que este controlador aún procesa inputs, no haciendo nada, pero aún los obtiene. Esto es mucho para un controlador read-only. Si es read-only entonces no debería procesar inputs para nada.

En otras palabras, no habría Controlador, es decir, una subclase NoControlador, que esté hecha de manera tal que su comportamiento sea read-only/no hacer nada. Implementa toda la interfaz de Controlador, pero sin hacer nada. Este controlador es un **Null Object**, el cual encapsula el cómo un tipo de objeto hace nada.

## Cuando usar Null Object

- Un objeto requiere un colaborador. No introduce esta colaboración, sino que usa la existente.
- Algunas instancias del colaborador no hacen nada.
- Queremos ignorar la diferencia entre colaborador que de un comportamiento y el que no hace nada.
- Reutilización del no hacer nada para que aquellos que necesiten el comportamiento funcionen de manera consistente.
- Todo el comportamiento que pueda necesitar el no hacer nada esta encapsulado en la clase del colaborador.

## Estructura

![[Pasted image 20260915111819.png]]

## Consecuencias

- Define jerarquías de clase que consisten de objetos reales y null objects. Cuando el cliente espera un objeto real, también puede tomar un null object.
- Código de cliente simple.
- Encapsula el no hacer nada en una null object class.
- Código de no hacer nada es fácil de reutilizar.
- No mezcla el comportamiento de no hacer nada con el comportamiento real de varios objetos en una colaboración.
- Puede necesitar una clase null object para cada clase abstracta.
- Puede ser difícil de implementar si varios clientes no se ponen de acuerdo en cómo es no hacer nada.
- Siempre actúa como no hacer nada.