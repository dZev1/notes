# Notas Instruction Level Parallelism

## Pipeline de instrucciones

- Von Neumann definió la máquina de ejecución. en 5 etapas. Una etapa por ciclo de clock, algo solo teórico.
	- **Fetch**
	- **Decode**
	- **Search Operand**
	- **Execute**
	- **Write Result**
- Hasta que aparece Intel, la mayoría de los procesadores ejecutaban el ciclo **en serie**.
	- Una etapa por ciclo de clock y nada más se ejecutaba esa etapa.
	- Si detectamos el estado de decodificación, sabemos que el bus está desocupado. Entonces agregamos señales de estado, esto lo hacemos con $\lceil log_2(\#estados + 1) \rceil$.
- **Pipeline**
	- Arquitectura que permite crear el efecto de superponer en el tiempo la ejecución de varias instrucciones a la vez.
	- Se formaliza **ILP**.
	- Poco o ningún hardware adicional.
	- Necesita que los bloques que resuelven la ejecución de una instrucción opere en forma simultánea.
	- Se logra si todos los bloques trabajan en paralelo pero con una instrucción distinta.
	- Cada etapa se llama **stage**.
	- El pipeline ideal es teórico pues, por ejemplo, el fetch de instrucciones no toma un solo ciclo de clock. Ni hablar de si hay branching.
	- **Primeras Conclusiones**
		- El pipeline tarda en llegar al primer resultado tantos ciclos de clock como cantidad de etapas haya.
		- Caso ideal: solo se consuma el primer ciclo de clock, y luego sea constante spit de resultados.
		- Este escenario es teórico. No se cumple todo el tiempo.
	
- **Eficiencia:** ¿Agregado de etapas más simples = mejor performance?.
	- Se tarda más en arrojar el primer resultado, pero cada etapa puede hacerse en menor cantidad de ciclos de clock.
	- $TPI = \frac{Tiempo \ por  \ instrucción \ en \ CPU \ ^"No Pipeline^"}{\#Etapas \ del \ Pipeline}$. Pareciera que el Time Per Instruction es proporcional con la cantidad de etapas.
	- ¿Es cierto? Si, pero hasta cierto punto. La cuenta no es 100% cierta.
		- Existen overheads introducidos por el pipeline, que suman demoras, pero el tiempo se aproxima al ideal.
	- La reducción se aprecia como si se requiriesen menos ciclos de clock para completar una instrucción.
	- **Conclusiones**
		- El pipeline no reduce el tiempo de ejecución individual, sino que incrementa la cantidad de resultados por unidad de tiempo.
		- El overhead no es significativo, pero algo agrega.
		- El rendimiento del procesador mejora notablemente al utilizar un Pipeline.