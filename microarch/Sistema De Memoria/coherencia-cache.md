# Coherencia Cache

## Sistema SMP (Multiprocesamiento simétrico)

Dos o más CPUs indénticas entre si con igual prioridad que comparten un sistema. Todo este conjunto está administrado por un único sistema operativo. Cada CPU tiene su propio Cache en un mismo Circuito Integrado (CI). A este par se le llama **Core**.

Si hay L3, este está fuera de los cores, pues es más grande que L1 y L2 y es compartido entre todos los cores. Si se tienen muchos cores en un único CI, entonces estamos ante un sistema multicore.

![[Pasted image 20260110102217.png]]

Los sistemas SMP se denominan multiprocesadores **UMA** (Uniform Memory Access). Todos los cores tienen el mismo latency para acceder a memoria, aún cuando se organiza en varios niveles y bancos.

El problema es que mientras se aumente la cantidad de cores aumenta, se requiere más ancho de banda por parte de la memoria DRAM, ya que sino el latency de acceso se vuelve inviable.

Una alternativa son **Distributed Shared Memory** (DSM) systems. Acá, la memoria se encuentra distribuida por grupos de cores. Cada grupo puede acceder tanto a su memoria local, como a la del resto.

Este esquema asegura un bajo latency y además un ancho de banda suficiente en la memoria local de cada core, a costo de que el acceso a memoria no es uniforme (**NUMA**), pues su latency depende de dónde está ubicado el elemento que se quiere acceder.

![[Pasted image 20260110103931.png]]

## Coherencia en un Cache

Cuando se escribe, siempre se trata de un dato (DCache). Cada variable en un nivel del cache, también está alojada en el resto de los niveles inferiores hasta la DRAM. Estas copias deben tener el mismo valor, cosa que no se cumple cuando se requiere escribirla.

La variable se modifica en el cache de ese procesador y queda distinta de las copias de su jerarquía de memoria, desde el nivel inmediato inferior hasta la DRAM.

La coherencia entre las distintas copias en la jerarquía depende del número de CPUs que haya en el sistema. Si el sistema es SMP y se ejecuta un programa paralelizado vía threads, estos pueden ejecutarse en varias CPU a la vez. Entonces pueden existir copias de una misma variable en uno o más caches de los cores restantes, las cuales se vuelven obsoletas por una modificación.

Se necesita una manera de comunicar cuanto antes a los demás cores que se modificó la línea y sus caches puedan, al menos, invalidar la línea obsoleta.

Se busca mantener la coherencia sin tener que hacer Write Through todo el tiempo.

### Esquema para mantener coherencia

Un sistema de Coherencia debe proporcionar **Migración** y **Replicado** para los elementos compartidos.
- **Replicado**: Propagar el elemento por la jerarquía, evitando así contenciones de Bus para acceso al dato.
- **Migración**: Propagar el elemento entre los caches, reduciendo el latency y demanda del ancho de banda al bus del sistema.
En sistemas SMP, el replicado y la migración se implementan por hardware, introduciendo **protocolos de coherencia**.

## Protocolos de coherencia

Son mecanismos para llevar el tracking de cada línea compartida entre cores.
