# Introducción

- La mecánica cuántica utiliza números complejos en su forma fundamental.
- La mecánica cuántica temprana estaba basada en gran medida por la mecánica de ondas.
- Los objetos a escalas muy pequeñas no poseen un estado bien definido y un único lugar, a menos que los estemos observando.
- En vez de decir que un objeto está en una posición u otra, decimos que está en una **superposición**.
	- No solo la posición espacial está sujeta a esto, sino que también otras propiedades físicas, como la energía, el ímpetu y otras propiedades únicas al reino cuántico, como puede ser el *spin*.
	- Nosotros no observamos la superposición de estados, sino que una vez que *midamos* la superposición, esta se *colapsa* a un solo estado bien definido.
- La mecánica cuántica no es determinista, sino que es **probabilística**.
- Hay limitaciones inherentes a la cantidad del conocimiento que se puede adquirir de un sistema físico.

## Implicaciones del mundo cuántico a las ciencias de la computación

- **Arquitectura**
	- Análogo cuántico de los bits: **qubits**.
		- La superposición permite a un bit en una superposición de 1 y 0 simultáneamente.
	- Poner muchos qubits juntos forma **Registros Cuánticos**.
	- Esta superposición es la base de la computación cuántica.
	- La noción de **compuertas** que manipulan bits también se extiende un dominio cuántico.
- **Algoritmos**
	- Fundamentalmente usan la superposición de estados.
	- Uno podría pensar que esto es un gran paralelismo, pero no podemos medir cuando está en superposición, pues si lo hacemos colapsa la superposición en un solo estado.
	- Los algoritmos cuánticos comienzan con la computadora cuántica en un solo estado. Luego, posicionamos delicadamente hasta colocarlo en una superposición de estados cuánticos. Tras esto, manipulamos los qubits de una manera especificada para luego poder medir, lo que colapsa el estado en nuestro resultado.
	- El entrelazamiento también juega rol importante.