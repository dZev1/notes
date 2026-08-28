# Notas Branch Prediction Unit

## ¿Es necesario?

- Si, pues cuanto más profundo es un pipeline, más grande es el branch penalty en el caso de saltos condicionales.
	- Hay que ver si se toma o no el salto.
- En saltos incondicionales podemos determinarlos en la etapa de DECODE.
- Necesitamos predecir los saltos, es decir, el valor del PC/IP, para minimizar o evitar el branch penalty.
	- La manera en que vuelve el branch penalty es si predecimos mal, pero vamos a tratar de minimizar los errores de predicción.
- Hay casos que no se pueden predecir, como las interrupciones. En estos casos tenemos que vaciar el pipeline si o si.
## Predicted-Non-Taken

- El procesador asume por default que el salto no se toma.
	- El procesador continúa realizando el fetch de las operaciones siguientes al salto, tratando al mismo como una instrucción más.
- Funciona para estructuras del tipo
```x86_asm
	INSTRUCCION `BRANCH`
	Sucesora secuencial
	INSTRUCCION DESTINO `BRANCH_TAKEN`
```
- Funciona entonces cuando saltamos *hacia adelante*, como puede ser un `if-else`. Es decir, la dirección destino es mayor (numéricamente) a la dirección que contiene al salto.

## Predicted Taken

- El procesador asume por default que el salto se toma.
	- Se continúa la búsqueda de la instrucción (OPCODE FETCH) a partir de la dirección target.
- Funciona con estructuras del tipo
```
	Instruccion TARGET para BRANCH TAKEN
	Instrucciones a ejecutar secuencialmente
	Instruccion BRANCH
```
- Funciona entonces muy bien para estructuras de iteración, es decir, *loops*.
- Como ventaja, siempre se acierta, a excepción de los casos donde el loop termina.
- Es más eficiente que el Predicted Non-Taken.

## Delayed Branch

-  Si tenemos una estructura como el `if-else`, se envía la instrucción sucesora secuencial a un slot de delayed-branch.
- Entonces se ejecutan independientemente ambas ramas. El JMP taken y el non-taken.
- Luego, no se aplica el resultado hasta no ver la evaluación de la condición.
- Entonces no se generan demoras.
	- Si se toma el branch, entonces se descarta la ejecución del slot delayed, y se tiene tiene un ciclo de clock para que salga el resultado de la instrucción destino.

## Loop Unrolling

- Se desenrollan los branches.
- Es necesario que los datos dentro del loop sean paralelizables.
	- Ejemplo:	
```C
	for (i = 0; i < 256; i++) {
		suma = 0.0f;
		for (j = 0; (j <= i && j < 256) ; j++)
			suma += v0[i - j] * v1[j];
		fAux[i] = suma;
	}
```
- *Ventaja*
	- Se eliminan los branches y desaparece la penalización.

## Predicción dinámica

- Es con hardware.
- El procesador realiza análisis del flujo de instrucciones.
- Toma decisiones en función de lo que encuentra se tiene por delante de la dirección de la predicción de saltos.
- Corresponden a microarquitecturas avanzadas, con mayor ILP.

### Branch Prediction Buffer

- Es una pequeña tabla, que tiene un índice que es la dirección de memoria de la instrucción de salto, y un bit que indica el resultado reciente del salto, es decir si se tomó o no (`0 = TAKEN`, `1 = NON-TAKEN`.
- El bit del último resultado es una pista para la próxima vez que se llegó al branch.
- No reviste seguridad.
- **PROBLEMA**
	- Tener un solo bit limita la eficiencia.
	- Cuando un salto siempre es `TAKEN`, y falla una vez, produce dos predicciones fallidas seguidas, pues el bit se invierte.
- Para mejorar el rendimiento, se implementó un modelo de 2 bits, que implementa una máquina de estados.
![[Pasted image 20260121120957.png]]
- No cambiamos la predicción al primer desacierto, la cambiamos al segundo.
- `0X` predice Non-TAKEN, siendo 00 fuerte y 01 débil.
- `1X` predice TAKEN, siendo 11 fuerte y 10 débil.
- Este tipo de Branch Predictor se implementa en la etapa del FETCH de instrucciones. Se implementa
	- Como un cache de direcciones de salto.
	- Agregando un par de bits a cada bloque de líneas en el ICache, que se usan solo si ese bloque tiene instrucciones de branch condicional.
		- Si es Taken, el PC se establece con la dirección del salto y se continúa buscando instrucciones a partir de allí.
		- En otro caso, busca instrucciones en la secuencia.
- *Conclusiones*
	- El método tiene eficiencia superior al 82%.
	- Eficiencia superior en programas de punto flotante frente a los de cálculo entero.
	- Más allá de un buffer de 4Kb, no hay aumento significativo de eficiencia. Lo mismo sucede con los 2 bits de predicción.

### Branch Target Buffer

- Cache de instrucciones de salto.
- Contiene para cada entrada, el par dirección de la instrucción de salto y dirección del target resuelta.
- No tiene los bits **taken/non-taken**.
- Memoria de acceso por contenido. Accedida por el valor completo del PC/IP.
- Si el valor no está, se asume **taken**.
	- Si el resultado es non-taken, se acepta el delay en el pipeline y no almacena nada en el BTB.
	- Si es taken, se ingresa el valor al BTB.
- Si el valor está en el BTB, se aplica la dirección target almacenada.
	- Si es taken, no hay penalización y no se guarda en el BTB ningún nuevo valor, pues el almacenado nos sirve.
	- Si es non-taken, se guarda el nuevo valor en el BTB, y surge la penalización correspondiente en el pipeline.

## Superscalar

- Se busca ejecutar más de una instrucción por ciclo de clock.
- Esto se lograría con más paralelismo.
- Agregar un pipeline más...
	- Esto lo hizo el Pentium de Intel, ensanchando el Data Bus a 64 bits y los buffers del pipeline, agregando una ALU más.
	- Pasó a ser arquitectura Harvard.
- **¿Problemas?**
	- Los obstáculos estructurales quedan más expuestos.
	- Teniendo en cuenta el obstáculo del acceso simultáneo a memoria, ahora no solo tenemos que lidiar con las etapas propias, sino que tienen que lidiar con las del otro pipeline.
	- Los accesos concurrentes aumentan con la cantidad de vías del Superscalar.
	- Se pueden ejecutar en dos ALUs dos instrucciones distintas. Pero si una depende del resultado de la otra, ya no se puede.
	- Si falla el branch prediction, tenemos que limpiar todos los pipelines.
