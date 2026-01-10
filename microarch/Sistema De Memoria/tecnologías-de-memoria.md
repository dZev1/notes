# Tecnologías de memoria

## Clasificación

### Memorias No Volátiles

Dispositivos capaces de retener información almacenada aún cuando se les desconecta la alimentación.

En la actualidad son denominadas **Flash**, por su tecnología de origen. Pueden ser grabadas con algoritmos de escritura específicos. Incluye a los discos de estado sólido, tarjetas SD, flash drives, y las memorias que componen al **firmware** de cualquier equipo.



### Memorias Volátiles
Una vez que se interrumpe la alimentación eléctrica, la infromación almacenada se pierde.
Tienen mayor capacidad que las no volátiles, y pueden modificarse en tiempo real a gran velocidad.

#### Memoria volátil dinámica (DRAM)

![[Pasted image 20260106143113.png]]

![[Pasted image 20260106143620 1.png]]

Almacena información en forma de estado de carga en un capacitor y la sostiene durante un breve lapso con ayuda de un transistor. Un bit se implementa con un solo transistor, el cual está en estado de corte, por lo cual consume mínima energía.

Necesita que se refresquen los estados de carga, pues pierden la información con el tiempo. 

Cuando leemos la información guardada, nos llevamos la carga, por lo cual se destruye la información guardada. Es por ello que necesitamos realimentarla mediante buffers.

#### Memoria volátil estática (SRAM)

![[Pasted image 20260106143842 1.png]]

Seis transistores por cada celda, entonces hay mayor consumo. Ganamos el no tener que refrescarla, la información se mantiene ahí, no se pierde al leerla. Es más rápida que la DRAM, pero más costosa.

## Uso de las memorias

La memoria no volátil es aquella que almacena el programa de arranque de cualquier sistema.

Se conectan en un espacio de direcciones determinado por el propio procesador de acuerda a la dirección donde se busca la primer instrucción luego del encendido.

El resto es memoria volátil y es allí donde el sistema copia hasta buena parte del código de arranque que reside en la memoria no volátil, para así ejecutarlo más rápido, pues tienen menor tiempo de acceso.

### Conexión básica

Sigue un modelo de Von Neumann. Es decir, tenemos un procesador, una memoria y la lógica de entrada/salida para conectar el computador al mundo exterior. Hay buffers desde el bus del sistema al bus del procesador, para poder manejar los dispositivos conectados al bus del sistema.

![[Pasted image 20260106145955 1.png]]

Desde los años 80, las velocidades de las CPU comienzan a superar año a año a los tiempos de acceso a memoria dinámica.

Se comenó a necesitar insertar **WAITING states** en cada ciclo de acceso a memoria, hasta que la memoria señale **READY** para el acceso con una línea de hardware.

Esto provocaba que el procesador podía llegar a estar no haciendo nada por varios ciclos de clock. Esto es un techo en la frecuencia de clock.

Hay un gap enorme entre la preformance de la DRAM y la de una CPU. Necesitamos una estrategia que minimice el efecto de este gap.

El problema consiste en decidir qué tipo de RAM utilizar en el sistema:

#### DRAM

- *Menor consumo*
- *Capacidad de almacenamiento alta*
- *Costo por bit bajo*
- **Tiempo de acceso alto, debio al circuito de regeneración de carga**

#### SRAM

- **Alto consumo**
- **Capacidad de almacenamiento baja**
- **Costo por bit alto**
- *Tiempo de acceso bajo*

>[!NOTE] Conclusión
>Si usamos solo SRAM, tenemos velocidad acorde con el procesador, pero con un consumo y costo inviable para la computadora.
>
>Si usamos solo DRAM, resolvemos el costo y la energía, pero con una velocidad inviable, es demasiado lenta.

## SOLUCIÓN: [[memoria-cache|Cache Memory]]

La memoria cache, es el primer nivel de la jerarquía. Es la primer memoria que se encuentra la dirección que la CPU coloca en el Address Bus.

En ella se guardan **una copia** de los datos y el código que el procesador más probablemente utilice (ver [[sistema-de-memoria#Principio de localidad|principio de localidad]]), para acceder a ellos más rápidamente.

Se implementa utilizando un bloque de SRAM de muy bajo tiempo de acceso.

Con esta solución, el procesador puede acceder a estos elementos sin insertar **WAITING states** en el ciclo de lectura o escritura de memoria.

En un nivel inferior de la estructura de la jerarquía de memoria, se mantiene una gran cantidad de DRAM donde se almacena el total de código y datos que se requieren en memoria física.

Esto requiere un hardware adicional para asegurar que la cache contenga los datos e instrucciones más frecuentemente usados por el procesador.