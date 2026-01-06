# Un poco de contexto...

>[!NOTE] Ley de Moore
>_\"El número de transistores incorporados en un chip se duplicará aproximadamente cada veinticuatro meses."_

Moore dice que la industria de los chips se aceleraría a un ritmo increiblemente rápido.  Tal es así que llegamos a chips con transistores de un espesor del cual no podemos disminuir, pues sino habría desastres físicos, pues habría que comenzar a romper átomos.

Los transistores tri-gate permitieron reducir el consumo energético de un procesador.

El escalado producido por reduir el tamaño de los transistores lleva a que estos puedan conmutar más rápido de 0 a 1.

A medida que se reduce el tamaño de los transistores, estos ganan linealmente en rendimiento: consumen menos enegía, aumentan su velocidad de conmutación, etc.

Los alambres de cobre que conforman los buses internos para la comunicación entre componentes, en las escalas y a las frecuencias de trabajo actuales, generan dos problemas:
- **Delays**
- **Consumo de energía**

## Potencia dinámica
Es la potencia que disipa un transistor CMOS en conmutación. Es proporcional a la capacidad de carga del dispositivo, al cuadrado de la tensión de alimentación y a la frecuencia de conmutación.

$$
P_d = \frac{1}{2} C_c \cdot V^2 \cdot f_c
$$
Entonces el scaling favorece decrementando tanto la capacidad de carga y la tensión de alimentación unitariamente, entonces logramos que la potencia disipada sea mucho menor. No queremos seguir aumentando la frecuencia del clock pues, esto aumentaría la potencia dinámica y se quemaría el circuito.

La energía dinámica es $C_c \cdot V^2$, entonces el scaling directamente nos sirve para que las baterías duren más.

## Leakage
A pesar de que un transistor esté al corte, siempre alguna corriente muy pequeña circula de todos modos.
Se calcula con la siguiente fórmula:
$$
E_e = I_e V^2
$$
donde $E_e$ es la potencia estátitica, $I_e$ es la corriente de leakage y $V$ la tensión de alimentación. Cada transistor tiene así una componente adicional de potencia cuando está en corte.
Mientras más transistores haya, más aumenta esta corriente.

## Arquitectura
Es el conjunto de recursos accesibles para el programador, como registros, ISA, estructuras de memoria.

## Microarquitectura
Implementación en el silicio de la arquitectura. Lo que está detrás del set de registros y del modelo de programación. Puede ser simple o robusta y poderosa.
Nos referimos a los detalles de la implementación de la ISA:
- Organización e interconexión de memoria.
- Diseño de los distintos bloques de la CPU para implementar el set de instrucciones.
- Implementación de paralelismo a nivel de instrucciones (ILP) y/o de datos.
