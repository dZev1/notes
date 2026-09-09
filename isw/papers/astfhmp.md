# A Simple Technique for Handling Multiple Polymorphism - Daniel Ingalls

- Ingalls menciona que los procedimientos en un lenguaje extendible debían ser polimórficos, debían poder manejar argumentos de muchos tipos.
	- La solución convencional a ese polimorfismo era hacer una comparación para cada tipo, y ejecutar el código adecuado para el caso.
	- Esto rompe el **principio de modularidad**.
- La introducción del paradigma de computación mediante mensajes permitió un nuevo método.
	- El proceso de envío de mensajes absorbe la necesidad del check de tipos, y los procedimientos/métodos, siendo locales de un tipo/clase particular, no son polimórficos y no dependen de otro tipo en el sistema.
	- Una variable o expresión que representa al receptr de un mensaje, puede variar en tipo dinámicamente.
- Acá introduce el problema a solucionar en el paper
	- Bajo ciertas situaciones, en la que más de una variable en una expresión es independientemente polimórfica, volvemos a caer en el type checking de antes y volvemos a romper la modularidad.
	- Entonces tenemos acá una interacción **doblemente polimórfica**.
	- Bajo esta situación empezamos a tener algo de la siguiente forma:
```smalltalk
MotorElectrico>>acelerar: unVehiculo
	(unVehiculo isKindOf: Auto)
		ifTrue: [unVehiculo velocidad: unVehiculo velocidad + 3 * kilometer/hour].
	(unVehiculo isKindOf: Helicoptero)
		ifTrue: [
			unVehiculo velocidad: unVehiculo velocidad + 3 * kilometer/hour.
			unVehiculo altitud: unVehiculo altitud + 2 * meter
		].
```
- El código en el ejemplo está distribuido para cada tipo de motor, entonces sería fácil extender este código a un nuevo motor.
- Pero si ahora, supongamos que, tenemos un bote.
	- Tenemos que ir clase por clase, extendiendo el código del bote para cada uno de esos motores. 
- Entonces volvemos a aumentar la complejidad a medida que aumentamos el nivel de polimorfismo, y así presentamos una barrera para cualquier desarrollador que quiera agregar un estado nuevo. O no...
- La solución a este problema está en todos los lenguajes orientados a objetos.
	- Solamente requiere entender cómo se relacionan el enviado de mensajes y el polimorfismo.
	- Cada transmisión de mensaje reduce unavariable polimórfica a una monomórfica, por el tipo inherente del message lookup.
	- Solo el receptor es polimórfico.
	- Entonces, enviémos nuevamente un mensaje, pero ahora del receptor al emisor, para así volver a reducir el polimorfismo que queda.
- Entonces, con esto, lo que nos queda es:
```smalltalk
MotorElectrico>>acelerar: unVehiculo
	^unVehiculo acelerarConMotorElectrico.
	
MotorAGas>>acelerar: unVehiculo
	^unVehiculo acelerarConMotorAGas.
```

- La información que ganamos en el priimer dispatch, la preservamos introduciendo una nueva familia de mensajes específicos a los tipos de motor.
- Entonces ahora lo unico que tenemos que hacer es ir a las clases de vehiculos e implementar los `acelerarConMotorXXX`:
```smalltalk
Auto>>acelerarConMotorAGas
	velocidad = velocidad + 2 * kilometer/hour.
Auto>>acelerarConMotorElectrico
	velocidad = velocidad + 3 * kilometer / hour.
...
Helicoptero>>acelerarConMotorAGas
	"blabla".
Helicoptero>>acelerarConMotorElectrico
	"xxxxx".
```
- Entonces ahora conservamos la modularidad del paradigma de objetos, pues al agregar el bote por ejemplo, solo debemos implementar los mensajes de aceleracion con motorXXX.
- Podemos hacer una cadena de estos mensajes para ir reduciendo el grado de polimorfismo con cada dispatch.
- Termina Ingalls el paper diciendo dónde esto es útil:
	- Interacciones entre **event types** y **event handlers**.
	- **Conexión entre programación lógica**, donde tanto el receptor y el argumento del mensaje unifyWith: son polimorficos en constantes, variables, términos, etc.
	- Rewrite experimental de la aritmética de Smalltalk-80.