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
En sistemas SMP, el replicado y la migración se implementan por hardware, los llamamos **protocolos de coherencia**.

## Protocolos de coherencia

Son mecanismos para llevar el tracking de cada línea compartida entre cores. Para ello requerimos poder establecer estados para cada línea, tal como teníamos el estado DIRTY.

Dos clases:
- **Directory Based**
	- El estado de las líneas se mantiene en un directorio centralizado accesible de manera rápida por todos los cores.
	- Puede ser un área de cache externa a cada core, lo que es más complejo.
	- Esto tuvo un auge a partir de los sistemas DSM.
	- En sistemas Multicore modernos, con múltiples niveles de Cache también es una posibilidad integrar esto.
- **Snoopy
	- Cada core mantiene el estado de sus líneas y puede monitorear lo que hace el resto con un bus dedicado.
	- Esto es lo más común.

Dos maneras de implementar un protocolo de coherencia:
- **Write Invalidate**
	- Aseguramos acceso exclusivo al procesador que va a escribir el dato, antes que la escritura se lleve a cabo.
	- El resto tiene que poder detectar la escritura e invalidar la línea que contiene el dato en caso de tenerlo en su Cache.
	- Se garantiza que no existe otra copia del dato luego de escribirse.
- **Write Update**/**Write Broadcast**
	- Actualiza la línea en todos los caches que la tengan.
	- Consume bastante ancho de banda.
	- Asegura menos Miss Rate.

Write Invalidate es lo más aplicado. pues al crecer el número de procesadores, el ahorro de ancho de banda en el bus es masivo bro.

### Snoop Bus

No se necesita implementar un bus específico. La información que necesita el Cache Controller es la Memory Address puesta en el bus, y si esta se va a leer o escribir, la cual circula por el bus del sistema.

El Snoop Bus es un conjunto de líneas entrantes a cada Cache Controller con el cual espía lo que hacen os demás cores con la memoria. Toma la línea de Address de todos los procesadores y dos líneas de control de cada uno: MemR/W.

Uno de los cores efectúa una operación y pone en su línea Address una dirección donde efectúa una escritura o una lectura. Estas tres señales pasan por el Bus del Sistema y llega por los Snoop Buses a todos los demás procesadores.

Si un procesador necesita un dato y no está en su cache, genera el acceso hacia la jerarquía inferior. Si fue un HIT, lo resuelve desde su cache. Baja or la jerarquía SOLO por un MISS.

Los demás procesadores hacen un Line Identification para saber si tienen esa línea en su cache. En caso de tenerla, se activa el protocolo de coherencia y se invalida inmediatamente la línea. En caso contrario, se ignora.

En los multiprocesadores de chips múltiples, el Snoop Bus utilizado para la coherencia es el bus de acceso a la memoria compartida. En uno de un solo chip, como el Intel Core-i7, se puede conectar el Snoop Bus a la conexión entre las caches privadas y la cache compartida.

![[Pasted image 20260110154357.png]]

Cuando se produce una escritura en una línea compartida, el procesador que escribe debe tener acceso **exclusivo** al bus para colocar la dirección que los demás deben invalidar desde su Snoop Bus.

Si dos procesadores intentan escribir diferentes líneas compartidas al mismo tiempo, sus accesos al bus para invalidación se realizan mediante la arbitración del bus de cada sistema. Es decir, primero saldrá una línea y luego la otra.

Si dos procesadores intentan escribir la misma línea, la serialización impuesta por la arbitración del bus también serializará sus escrituras.

Como consecuencia, una escritura en un elemento de datos compartido, no puede completarse hasta que se obtenga acceso al bus.

Todos los esquemas requieren algún método de serialización de accesos a la misma línea de cache, ya sea serializando el acceso al bus o a otra estructura compartida, como el cache directory.

### Write Invalidate

#### Cache Search

Además de invalidar las copias pendientes de la línea que está siendo escrita en el Cache, hay que localizar un elemento de cuando se produce un Cache Miss.

En un Cache *write-through*, todas las líneas se envían a la memoria siempre. Siempre se puede obtener el valor más reciente de un elemento de datos en cualquier punto de la jerarquía.

Los buffers de escritura se tienen que tratar como entradas de cache adicionales, porque puede estar allí el dato modificado esperando para ser copiado al siguiente nivel. Este dato hay que descartarlo también del write buffer.

Para un Cache *write-back*, el valor del dato más reciente puede estar en un cache privado, en vez del compartido, o en la memoria principal. Por esta razón ellos usan el mismo esquema de **Snooping** tanto para *Cache MISS*, como para *escrituras*.

Si un Cache tiene una copia DIRTY de la línea solicitada, el Cache habilita la lectura de esta línea desde el Cache y cancela su acceso a la memoria principal o al Cache L3. Como los **Write-Back** usan el mínimo ancho de banda posible de memoria, pueden soportar una mayor cantidad de procesadores más rápidos.

Como resultado, todos los procesadores Multicore usar **WRITE-BACK** en los niveles más externos del cache. Por eso nos concentraremos en aquellos que sean de este tipo.

Los tags del Cache, sirven para el proceso de Snoop, y el bit de validez de cada línea hace trivial la invalidación. Los **Read Miss** también son simples, pues ya sea que se generen por una invalidación u otro evento, se basan en el Snoop.

#### Cache Write

Para escrituras necesitamos saber si hay otras copias de la línea almacenadas en cache pues, si no las hay, en un Write-Back la escritura no necesita cursarse en el bus. Así se reduce tanto el tiempo de escritura para el procesador, como el ancho de banda solicitado al bus.

Para saber si una línea de Cache se comparte o no, se puede agregar un bit de estado adicional a cada línea. Con este bit el controlador cache puede decidir si una escritura debe generar una invalidación.

Cuando se produce una escritura en una línea cuyo estado es compartida, el Cache Controller genera una invalidación en el bus, genera el **write-back** y marca la línea como exclusiva. Ese core no envía ninguna invalidación más luego de escribir esa línea.

El cache que tiene la única copia de una línea es el **owner** de la línea. Si otro procesador más adelante solicita esta línea, el estado vuelve a compartida. Esta solicitud por el bus responde a un Cache MISS en el procesador remoto, y se detecta por el Snoop Bus el acceso a memoria.

Cada transacción de bus obliga a comparar los tags en el cache, y podría interferir con accesos del procesador a su propia memoria cache. Para reducir esta concurrencia, se pueden duplicar los Tags y dirigir los accesos de Snoop a los duplicados, lo cual requiere mayor costo de hardware.

Otro enfoque es usar en la L3 un directorio que indique para cada línea si se comparte y qué núcleos tienen la copia, pasando a ser **directory based**. Requiere que el L3 siempre tenga una copia de cualquier elemento de datos de L1 o L2.

## Implementación de protocolos de coherencia

El Snoop Bus es útil para obtener información que permita tomar acciones para mantener la coherencia de acuerdo con la actividad que este registra en el bus del sistema.

Para actuar de acuerdo con las políticas de escritura que adoptemos, se requiere un conjunto de acciones a tomar para cada caso posible de actividad en el bus (**un protocolo**) y hardware adicional para implementarlas.

**Copy Back** es el método menos demandante para el sistema, optimiza la utilización del bus, pero parece inapropiado cuando se trata de mantener coherentes los datos entre dos o más caches.

Para poder utilizar este método de escritura siempre que sea posible y reemplazarlo solo cuando la misma dirección física está presente en más de un cache, se desarrollaron protocolos de coherencia.

Hay que implementar una máquina de estados finita en el Cache Controller de cada core que cambia el estado de cada línea de acuerdo con los requerimientos del procesador local del core o requerimientos provenientes del bus.

Podemos pensar en un controlador individual asociado con cada línea, que pueda proceder independientemente con las operaciones de Snoop o con las solicitudes de Cache para diferentes líneas. En implementaciones reales, un controlados soporta múltiples operaciones entrelazadas a líneas diferentes.

Una operación puede iniciarse antes de que se complete otra, aunque solo se permite un acceso a la cache o al bus a la vez.

Para cualquier protocolo de coherencia se deben definir dos atributos por línea de cache: su **ownership** y su **coherencia**. Estos atributos definen las acciones que el protocolo debe realizar para cada actividad posible en el bus del sistema detectable por el Snoop Bus.

Un cache es propietario de la línea cuando es el único que la tiene. Sino, ownership es **SHARED**. Una línea esta coherente con el resto de las copias disponibles cuando todas las copias son iguales; y es incoherente si su copia está **DIRTY** en algún cache producto de alguna escritura. Esto puede tolerarse siempre que el cache que tiene la copia **DIRTY** tenga el ownership de la línea.