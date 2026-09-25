# The Object Recursion Pattern - Bobby Woolf

## Intent

- Distribuir procesamiento de requests sobe una estructura delegando polimórficamente.
- Permite a una request ser subdividida en partes más pequeñas que son más faciles de manejar.

## Motivación

- Considerar igualdad entre dos objetos.
- Objetos simples y primitivas son fáciles de comparar.
- La dificultad está en objetos complejos.
- Un approach para resolver esto es usar un objeto `Comparer`, que compara dos objetos complejjos y te dice si son equivalentes o no.
- Esto no está bueno, pues el Comparer tiene que reconocer que tipo de objeto son los comparados y saber descomponerlos en objetos más simples que tienen que ser comparados.
  - Cuanto más complejo el sujeto a comparar, más complejo el código de comparación.
- Este approach requiere mucho código complejo que es dificil de mantener.
- Entonces, podemos hacer que los objetos se comparen ellos mismos.
- Podemos hacer que los objetos mismos determinen si otro objeto es equivalente a él mismo.
- Pone el ejemplo de un `Motor` que tiene de variables de instancia `size` y `power`, y sabemos que lo único que debemos comparar es números, entonces nos apoyamos en que `Number/Integer` saben responder al mensaje `=`, entonces solo debemos hacer `size = anEngine size` y `power = anEngine power`.
- El cliente solo tiene que mandar `unEngine = otroEngine`.
- Este approach donde los objetos que componen a otro son encargados de subdividirse a sí mismos y resolver sus subproblemas es lo que llamamos **Object Recursion**.

## Claves

- Dos clases polimórficas, una que maneja la request recursivamente y otra que maneja la request sin recursión.
- Un mensaje separado, en una tercera clase, que no es polimórfica con las otras dos, que es la que inicia la request.

## ¿Cuándo usar?

- Pasar un mensaje a través de una estructura enlazada, donde el destino final no es conocido.
- Broadcast de un mensaje a todos los nodos de una estructura.
- Distribuir la responsabilidad del comportamiento a través de la estructura enlazada.

## Participantes

- Iniciador
  - Inicia la request
  - No es un subtipo del handler. `makeRequest()` es otro mensaje distinto de `handleRequest()`.
- Handler
  - Define el enlace sucesor.
  - Maneja la request delegándola a los sucesores.
  - Los sucesores relevantes a una request pueden variar por request.
  - Pueden hacer comportamiento extra.
  - Puede ser un terminador o una request distinta.
- Terminador
  - Completa la request implementando completamente y no delegando a nadie su implementación.
  - Puede ser recursor para otra request.

## Esquema general

1. Iniciador hace una request pidiendo al handler que la maneje.
2. Cuando el handler es recursor, hace lo que tiene que hacer y delega al sucesor/es que maneje la request y devuelve según lo que haga el sucesor/es.
3. Si el handler es terminador, maneja la request sin delegar nada a nadie, y retorna el resultado.

## Ventajas

- Procesamiento distribuido
- Flexibilidad de Responsabilidad 
- Flexibilidad de roles
- Encapsulamiento incrementado.

## Desventajas

- Complejidad de programación: mucha recursión.

## Usos conocidos

- Mensaje de igualdad de objetos.
- Copy/Clone.
- Imprimir un objeto como un string. (`toString()` en Java o `printString` en ST).
