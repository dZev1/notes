# Microarquitectura

Preparación para el final de miroarquitectura, para sacarme un excelente 10 creo...

## Índice
- [[microarch/README#Conceptitos|Conceptos]]
	- [[microarch/README#Alta Impedancia (Z)|Hi-Z]]
	- [[microarch/README#Transistor MOS|Transistor CMOS]]
- [[historia|Historia]]
- [[sistema-de-memoria|Sistema de Memoria]]
- [[tecnologías-de-memoria|Tecnologías de Memoria]]
## Conceptitos

### Alta Impedancia (Z)

El símbolo Z indica que un nodo no está ni alto ni bajo. No es un 0 lógico, pues un nodo en Z puede ser un 1, un 0, o cualquier valor entre medio. Es un nodo **flotante**.

Que haya alta impedancia no significa que hay un error en el circuito, siempre y cuando se pueda poner en un estado que no sea de alta impedancia.

Una manera de originar alta impedancia es no conectando el nodo a una corriente.

Existen buffers tri-state, como el que se muestra en la imagen, donde el output está en alta impedancia si el ENABLE del buffer está bajo, pero se habilita el output si el ENABLE está alto.
![[Pasted image 20260103225403.png]]

Estos buffers se utilizan normalmente en _busses compartidos_ que conectan múltiples chips, como se muestra en la imagen. No se puede escribir en el buffer de dos lugares a la vez, pues esto rompería el sistema, entonces se utilizan estos buffers para habilitar la corriente de chip en chip. También tienen un cable de lectura que siempre está habilitado para leer lo que viene del bus.
![[Pasted image 20260103225735.png]]
### Transistor MOS

#### Semiconductor _n_-type

Un semiconductor _n_-type se consigue al utilizar dopantes donantes (de **Grupo V** de la tabla periódica) para aumentar la conductividad del silicio, como puede ser el _Arsénico_. De esta manera se forma un exceso de electrones libres que son portadores de carga mayoritarios.

#### Semiconductor _p_-type

Un semiconductor _p_-type se consigue al utilizar dopantes del **Grupo III** de la tabla periódica el cual pide un electrón de manera tal que a alguno de los átomos de silicio que rodean al dopante siempre le faltará un electrón. De esta manera se forma un **agujero** que se propaga por el exterior.

### Diodo

![[Pasted image 20260103232359.png]]

Es la intersección de un silicio de tipo p con uno de tipo n, donde el voltaje del semiconductor de tipo p se llama **ánodo** y del tipo n es **cátodo**.
Cuando el voltaje de un ánodo aumenta por encima del cátodo, se el diodo es **polarizado hacia adelante**. Cuando el voltaje del ánodo es menor o igual que el del cátodo, entonces es **polarizado hacia atrás**.

#### Estructura MOS

Una estructura MOS se crea superponiendo varias capas de materiales conductores y aislantes formando una estructura simil a un sanguche. La tecnología CMOS provee dos tipos de transistores: nMOS y pMOS, se pueden utilizar ambos transistores de manera intercambiable, asi que usaremos CMOS para referirnos a cualquiera de ellos.
![[Pasted image 20260103232854.png]]

---
$$V_{DD} \iff POWER \iff LOGIC \ 1 \iff Voltaje+$$
$$V_{SS} \iff GROUND \iff LOGIC \ 0$$

Se puede ver a los transistores MOS como switches ON/OFF (1/0 para los nMOS y 0/1 para los pMOS)