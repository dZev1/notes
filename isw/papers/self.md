# Self: The Power of Simplicity - David Ungar & Randall B. Smith

- Self es un lenguaje de programación orientada a objetos.
- En vez de basarse en clases, como Smalltalk, se basa en prototipos.
- Tampoco tiene variables.
- Ideas de: prototipo, slots y behavior
- **Prototipos**
  - Combinan inheritance e instantiation.
  - Marco más simple y flexible.
  - Los prototipos son ejemplos existentes en lugar de descripciones de formato e inicialización, como son las clases.
- **Slots**
  - Unen variables y procedimientos en un único construct.
  - No hay diferencia entre estado y behavior.
    - Esto reduce la grieta entre objetos ordinarios, procedimientos y closures.
  - Un objeto accede a su información de estado enviando mensajes a `self`, el receptor del mensaje actual.
    - De allí el nombre del lenguaje.
- **One-of-a-kind objects**
  - Se pueden incluir objetos únicos con su propio comportamiento.
  - Como cada objeto tiene slots nombrados donde guardar comportamiento o estado, cada objeto puede tener comportamiento único sin usar otra instancia.
  - Los lenguajes basados en clases no pueden hacer esto, pues estos están pensados para tener muchos objetos con un mismo comportamiento y es difícil llevarlos a una sola instancia.
- Cuando se escribió el paper, no había **encapsulamiento**, pero se estaba planeando agregarla.
- Otra cosa que estaba en proceso es la **herencia múltiple**.
  - Permitiría más expresividad.
  - Cada objeto tendría muchos parents.
  - Fábrica de behavior.

