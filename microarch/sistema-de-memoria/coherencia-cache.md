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

## Protocolo MSI

Protocolo de tres estados: Modified, Shared, Invalid (**M, S, I**). Es el protocolo más simple que se puede implementar.

- **Invalid**: la línea es inválida. No se informa al sistema que se invalida una línea.
- **Shared**: la línea puede **potencialmente** estar compartida. Entonces puede estar compartida o el core que marca Shared puede ser el único que la tenga.
- **Modified**: la línea fue modificada en el cache **local**, la línea está DIRTY. Como este protocolo es **write-invalidate**, este estado es además **EXCLUSIVO**. Este estado implica que ese core es el dueño de la línea. Este estado permite aplicar políticas **Write Back** en el cache local.

Estos protocolos tienden a beneficiar no solo la performance del core local, sino que también la del conjunto.

### Transiciones requerimientos CPU

![[Pasted image 20260111161819.png]]

#### INVALID  --CPU Read--> SHARED

Da un *Read Miss* en el bus, sale al bus del sistema a buscar el dato. Una vez que se haga con el dato, lo almacena en el cache y lo coloca en estado **SHARED**. No sabemos si la línea está en otra cache o no. Asume por default SHARED.

#### INVALID --CPU Write--> MODIFIED

Da un *Write Miss* en el bus, propaga el write miss a los demás cores. Los cache controllers de cada core van a ver si tienen la línea. En caso de tenerla, la van a invalidar. Una vez hecha la escritura, la línea se marca como **MODIFIED**. Ahora la política de escritura es *Write-Back* para esta línea.

#### MODIFIED --CPU Read/Write HIT--> MODIFIED

Cualquier operación de escritura o lectura que se efectúe sobre esta línea, mantendrá su estado MODIFIED.

#### MODIFIED --CPU Write MISS--> MODIFIED

Se produce un Write Back de la línea actual para salvarla, luego propagamos Write Miss al bus. Luego hay que hacer un Replacement de la línea.

La línea se mantiene modified pero tiene otra dirección de la memoria principal y valor. **REPLACEMENT**.

#### MODIFIED --CPU Read MISS--> SHARED

Se hace el Write Back de la línea, luego se propaga un Read Miss al bus y luego hacer un reemplazo a la línea. Se coloca el estado SHARED.

#### SHARED --CPU Read HIT--> SHARED

Siempre que se pida leer la dirección se lee sin problema, sin modificar nada.

#### SHARED --CPU Read MISS--> SHARED

Puede ocurrir un Conflict Miss sobre esa línea. Puede buscar la lectura de una línea que mapee a ese set del cache, por lo tanto se propaga Read Miss en el Bus y se reemplaza la línea.

#### SHARED --CPU Write--> MODIFIED

Si se escribe la línea en la misma dirección de memoria, simplemente pasamos de estado la línea a MODIFIED previamente invalidadas todas las copias en los demás cores.

#### SHARED --CPU Write MISS--> MODIFIED

La CPU quiere escribir en el dato que mapea a esta línea, pero no la tengo. Como la cache está llena, se produce Miss y Conflict Miss.

Entonces se reemplaza la línea, la antigua siendo descartada y se vuelve exclusiva la línea (hence MODIFIED).

### Transiciones requerimientos BUS

![[Pasted image 20260111162634.png]]

Cuando se detecta un Write Miss o un Invalidate en el bus, todos los caches que poseen esa línea deben invalidarla.

#### Mod --Write Miss de la línea--> Invalid

Se hace un Write Back de la línea y se aborta el acceso a memoria.

#### Shared --Write Miss de la línea o Invalidate--> Invalid

No se hace nada, solo se invalida.

#### Mod --Read Miss de la línea--> Invalid

Se aborta el acceso a memoria y se hace un Write Back de la línea.

#### Shared --Read Miss--> Shared

### No es óptimo

Al ser Shared tan impreciso, se hacen muchas transacciones en el bus innecesarias por si llega a ocurrir que otro core tenga el dato.

Cuando se tienen dos cores, puede estar bien, pero cuando aumentamos los cores, ya no tanto.

## Protocolo MESI

Agrega el estado Exclusive para disminuir la actividad en el bus. Una línea en estado **E** puede escribirse sin generar transacciones en el bus. Aplica a cache L1 de datos y L2/L3, L1 de código es MSI.

- **Modified**: Línea presente solamente en este cache que varió respecto de su valor en memoria del sistema. Requiere Write-Back hacia la memoria principal antes que otro procesador lea desde allí el dato, que ya no es válido.
- **Exclusive**: Línea presente solo en este cache, que coincide con la copia en memoria principal. La línea está LIMPIA, la podemos modificar sin informar a nadie.
- **Shared**: Línea del cache presente y *puede* estar almacenada en los caches de otros cores. Este estado asegura la coherencia. Estas seguro que la copia de esta línea es buena, está compartida y coherente con el resto del sistema.
- **Invalid**: Línea no válida.

### Transiciones Reqs CPU

Todas las lecturas de líneas enviadas por la CPU se resuelven desde el cache, a excepción de aquellas líneas en estado INVALID. Las líneas inválidas se buscan en la jerarquía inferior inmediata. Una línea Shared o Exclusive se puede descartar e invalidarse en cualquier momento.


![[Pasted image 20260111191359.png]]

#### INVALID --CPU Read--> EXCLUSIVE

Se coloca un Read Miss en el bus y cambia el estado de la línea a EXCLUSIVE.

#### INVALID --CPU Write--> MODIFIED

Se propaga un Write Miss en el bus y se cambia el estado de la línea a MODIFIED.

#### MOD --CPU Read/Write HIT--> MOD

No cambia el estado y todas las lecturas y escrituras se hacen solamente sobre el cache local a máxima velocidad. Se utiliza Write Back

#### MOD --CPU Conflict Read Miss--> EXCLUSIVE

Se hace un Write Back de la línea, luego se propaga un read miss al bus y se hace un replacement con la línea nueva. Luego se coloca la línea en modo EXCLUSIVE.

#### MOD --CPU Conflict Read Miss--> Shared

Es una transacción iniciada por la CPU local, pero que además recibe del bus una orden de que está compartida la línea. Se hace un Write Back de la línea, se propaga un Read Miss con un Shared en el bus y luego se hace el replacement de la línea.
#### EXCLUSIVE --CPU Read HIT--> EXCLUSIVE

Todas las lecturas se llevan a cabo a máxima velocidad en el cache local. Se mantiene el modo Exclusive.

#### EXCLUSIVE --CPU Write--> MOD

Se hacen a gran velocidad en el cache local, pero como cambiamos la copia que está en la memoria principal, tenemos que cambiar a MODIFIED, sin invalidar en el bus nada.

#### SHARED --CPU Write Miss--> EXCLUSIVE

Se propaga un Write Miss en el bus haciendo previamente un Write Back de la línea.

#### SHARED --CPU Conflict Read Miss--> SHARED

Esto ocurre si se lee un Read Miss junto a un Shared en el bus. Solo se hace un Replacement y queda en estado shared.

#### SHARED --CPU Read HIT--> SHARED

Si se lee una línea en Shared, no hay ningún tipo de problema. Queda en Shared.

#### SHARED --CPU Conflict Read MISS--> Exclusive

Esto ocurre si hay un conflict read miss que y no se recibe un Shared del bus. Se reemplaza la línea y se tiene el ownership exclusive.

### Señales

- **Read For Ownership (RFO)**: línea que va de controlador en controlador bidireccional que permite a un controlador avisar a los demás un requerimiento de propiedad de la línea.
	- Es enviada por el cache que necesita escribir una línea en estado **Shared** o **Invalid**. El resto de los caches revisan si tienen la línea. En caso de tenerla la invalidan al recibir esta transacción.
	- Desde el punto de vista del Cache Controller, es una lectura de la línea cacheada con toma del bus para escribir contenido de la línea en el nivel jerárquico inferior, es decir, un **Write Back**.
	- Fuerza al resto de los controladores a invalidarla.
- **Shared**: línea bidireccional al controlador. Con esta línea los cores se informan si una línea es shared o no. Si un core tiene una línea exclusive y recibe del Snoop Bus que se va a leer esa línea, activa la línea Shared para que el otro core no la ponga exclusive, sino Shared.

### Transiciones Reqs del Bus

![[Pasted image 20260111194350.png]]

#### Mod --Read Miss--> Exclusive

Si por el Snoop Bus se detecta un Read Miss para una línea Modified en el cache local, el controlador asume que tiene la única copia válida, pero que está incoherente al resto de los niveles de la jerarquía. Entonces se activa la línea RFO para indicar al lector que ese dato incoherente y previamente realizando un Write back de la línea.

#### Shared --Read Miss--> Shared

Si se ejecuta un Read Miss, es otro más que no tenía, pero quedamos en shared. La leemos nada más.

#### Mod --Write Miss--> Invalid

Se hace un Write Back de la línea previo a invalidarla.

#### Exclusive --Write Miss/Invalidate--> Invalid

Se invalida la línea sin requerir hacer write backs.

### Limitaciones y Fortalezas

Es eficiente para mantener la coherencia en un sistema multiprocesador, con diferentes niveles de memoria; optimiza el uso del bus del sistema, habilitando política Write Back de escritura siempre que se pueda; invalida copias cada vez que se escribe un dato y que una escritura sea Write Through.

Un efecto de Write Invalidate es que probablemente aquellos cores que invalidaron su copia por requerimiento de escritura vuelvan a necesitarla. Solo la obtienen de los niveles inferiores de la jerarquía, con un costo de performance evidente. Esto es aún más evidente a medida que aumentemos el número de cores.

## Protocolos para Multicore

### MESIF

Se busca optimizar el acceso al bus en sistemas con memoria distribuida. Intel agrega un estado adicional a MESI, el estado **Forward** (**F**) como una forma especial del estado **S**.

El protocolo debe asegurar que entre los caches que tienen una línea en estado S, una de ellas tenga estado F. Cuando se invalida una línea F y S, no se informan. Si no queda ninguna línea en estado F, el próximo **Read Miss** se resuelve desde el nivel jerárquico inferior, con su penalización de performance consecuente.

Para minimizar este costo, se asigna el estado F mediante un criterio **LRU**. El cache que resuelve Forward, cede el estado F a la línea del cache que termina de leerse. Siempre el último que lee esta línea, tiene el estado F, mientras que la copia que tenía antes el estado F, pasa a estado S.

>[!Note] ¿Cómo funciona F?
> La línea F es una línea Shared forwardeable. Cuando otro core genera un Read Miss al querer leer esta línea, en lugar de hacer perder tiempo al core, el core que tiene la línea F, habilita su salida y le provee la línea.

### MOESI

Se agrega un quinto estado: **Owned**. *Modified*, *Invalid*
y *Exclusive*, se comportan como en MESI. Al estado *Shared* se le introduce una diferencia por consecuencia del nuevo estado.

**Shared** indica que una copia de la línea puede estar en otros caches, y es válida. Hasta ahí, igual que MESI. Pero ahora, la copia en memoria principal puede no ser válida.

Si ningún cache tiene una copia de esa línea en estado **OWNED**, la copia de memoria principal es válida. Si no hay ninguna, es inválida.

Solo **una** copia de una línea puede tomar estado Owned en un cache. Indica que se trata de una de varias copias válidas de la línea, pero con la diferencia de que esta es la única que permite escribirla sin efectuar Write Back. El resto de las copias permanecen S y válidas.

Cada vez que se escribe una línea **Owned**, se activa la transacción por el bus local, y el resto de los caches con copias Shared, la actualizan. Así, se disminuyen los **write back** por intentar leer una línea DIRTY, y las invalidaciones al escribir una línea S. Además,el cache que tiene la línea Owned, se encarga de proveerla a los *"nuevos lectores"*, aquellos que le asignarán al leer S.