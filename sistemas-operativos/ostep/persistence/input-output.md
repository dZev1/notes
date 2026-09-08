# Administración de I/O

Hablamos varias veces de hacer I/O. A estos dispositivos se los suelen categorizar en dispositivos de almacenamiento, comunicaciones, interfaces de usuario, etcétera. Vamos a concentrarnos en aquellos de almacenamiento.

Los SO se tenían que preocupar de:
- **Discos rígidos**, que siguen siendo una preocupación primaria.
- **Unidades de Cinta**, que ya solo se usan para hacer copias de seguridad.
- **Discos removibles**, como CDs, Floppy Disks o DVDs.

También hay discos virtuales, que están en otro punto de la red, a los que se acceden mediante ella. A estos los llamamos **NAS** (**N**etwork **A**ttached **S**torage). Hay otras alternativas llamadas **SAN**, o **S**torage **A**rea **N**etwork, que son redes especiales donde los protocolos son específicos para los tipos de datos a almacenar. 

Para nosotros, un dispositivo I/O tiene dos partes:
- El **device** (dispositivo físico).
- Un **driver (controlador) del dispositivo**, el cual interactúa con el SO a través de algún bus o registro.

## Drivers

Son componentes de software muy específicos, pues conocen las particularidades del hardware contra el que hablan. Hasta distintos modelos de un mismo fabricante pueden necesitar drivers distintos.

Son clave, pues corren con máximo privilegio, por lo que pueden hacer que se cuelgue todo el sistema, y de ellos depende el rendimiento de I/O, que es fundamental para el rendimiento combinado del sistema.

### Interacción con los dispositivos

Tres formas:

#### Polling

El driver verifica periódicamente si el dispositivo se comunicó.
Esto tiene como ventajas que es sencillo, pues son cambios de contexto controlador, a costa de consumir CPU.

#### Interrupciones (o push)

El dispositivo avisa generando una interrupción.
Esto tiene como ventaja que son eventos asincrónicos poco frecuentes, pero genera cambios de contexto impredecibles.

#### DMA (Direct Memory Access)

Se usa para transferir grandes volúmenes, la CPU no interviene. Requiere de un componente de hardware, el *controlador DMA*. Cuando este controlador termina, se genera una interrupción a la CPU.

### Subsistema I/O

Se ocupa de proveerle al programador una API sencilla:
- `open()` / `close()`
- `read()` / `write()`
- `seek()`

Hay cosas que no se pueden, o no se deben, ocultar. La misión del SO es hacer esto de manera correcta y eficiente. Esa responsabilidad está compartida entre el manejador de I/O y los drivers.

Hay dos grupos de dispositivos:

#### Char Device

Dispositivos en los cuales se transmite la información byte a byte. Por ejemplo, mouse, teclado, terminales. Debido a su acceso secuencial, no soportan acceso aleatorio y no utilizan *cache*.

#### Block Device

Dispositivos en los que la información se transmite en bloques. Por ejemplo, disco rígido, memoria flash o CDs. Permiten el acceso aleatorio y usan por lo general un buffer (*cache*).

![[Pasted image 20260429182014.png]]

---

El diálogo con estos dispositivos tiene las siguientes características:
- Son de `R`, `W`, o `RW`.
- Brindan acceso `secuencial` o `aleatorio`.
- Son `compartidos` o `dedicados`.
- Permiten comunicación de a `chars` o de a `bloques`.
- La comunicación con ellos es `sincrónica` o `asincrónica`.
- Tienen distinta velocidad de respuesta.

Una de las funciones del SO, en tanto API de programación, es dar un acceso consistente a todas las variaciones de dispositivos, ocultando las particularidades de cada uno de ellos tanto como sea posible.

### API del Subsistema I/O

>[!Important] ¡Todo es un archivo, incluso dispositivos!

Se proveen *funciones de alto nivel* para acceder a los archivos de los dispositivos:
- `fopen`, `fclose`
- `fread`, `fwrite`: Leer/escribir archivos en modo bloque.
- `fgetc`, `fputc`: Leer/escribir archivos en modo char.
- `fgets`, `fputs`: Leer/escribir archivos en modo stream.
- `fscanf`, `fprintf`: Leer/escribir archivos en modo char con formato.

## Planificación de I/O

Queremos minimizar los movimientos que hace la cabeza para llegar a la posición donde leer/escribir. Los pedidos de I/O llegan al disco constantemente, incluso antes de que terminemos uno de ellos, razón por la cual debemos planificar cómo manejar la cola de pedidos de I/O para lograr el mejor rendimiento posible.

No solo hay que manejar el *ancho de banda*, que es la cantidad de bytes que es posible transferir por vez, y la *latencia rotacional*, que es el tiempo que toma el disco en rotar y que la cabeza quede donde queramos, sino que también hay que manejar el tiempo de búsqueda o *seeek time*, que es el tiempo necesario para que la cabeza se ubique sobre el cilindro que tiene el sector buscado.

## Políticas de scheduling I/O a disco

Esquema más simple: **FIFO**, el tema es que si tenemos pedidos en distintos cilindros, tenemos que mover la cabeza de un lado a otro sin sentido.

Otro esquema posible es **SSTF** (Shortest Seek Time First). La idea de este esquema es atender como próximo pedido al más cercano de donde está la cabeza en ese momento. Si bien mejora los tiempos de respuesta, puede producir starvation. Es un algoritmo greedy, pero no óptimo.

Otra posibilidad es el algoritmo *scan*: ir primero en un sentido, atendiendo los pedidos que se encuentran en el camino, y luego ir en el otro sentido. Podría suceder que llegue una solicitud para el cilindro inmediato anterior, pero no la vamos a resolver hasta que volvamos a pasar por ese cilindro en sentido contrario. El tiempo de espera no es uniforme.

En la práctica, no se usa ninguno de estos algoritmos de manera pura, sino que hay prioridades (como bajar páginas de caches, swapping de procesos), etcétera.

## SSDs

- Los discos de estado sólido mejoraron sus prestaciones y disminuido su precio.
- Son más livianos, resistentes, silenciosos y consumen menos energía.
- Tienen mejor performance en la lectura que los discos duros, pues no tienen componentes mecánicos, pero tienen una escritura más compleja.
- Presentan problemas de durabilidad y *write amplification*.

## Gestión del disco

### Formateo

Se trata de poner en cada sector unos códigos que luego sirven a la controladora de disco para efectuar detección y corrección de errores. Funcionan como un prefijo y un postfijo a la parte donde efectivamente van los datos en cada sector. Si al leer un sector, el prefijo y postfijo no tienen el valor que deberían, el sector está dañado.

### Booteo

Las computadoras tienen un programa en la ROM que carga a memoria ciertos sectores del comienzo del disco y los comienza a ejecutar. El programa es tan pequeño que no llega a ser un SO, sino un cargador de SO.

### Bloques dañados

A veces se manejan por software, y el sistema de archivo es responsable de anotar los inválidos. Los discos vienen con sectores extra para reemplazar los defectuosos.

Cuando la controladora detecta un bloque dañado, se actualiza una tabla interna de remapeo y usa otro sector. Para no interferir con optimizaciones del scheduler I/O, los discos llevan sectores extra en todos los cilindros.

## Spooling

Es una forma de manejar dispositivos que requieren acceso dedicado en sistemas multiprogramados. Un ejemplo de ello es la impresora. No queremos, como usuario, que la impresora se bloquee hasta que se termine de imprimir todo.

La idea entonces es poner el trabajo en una cola, y designar un proceso que los desencole a medida que el dispositivo se libere.

El kernel no se entera que se está haciendo spooling, el usuario si.

## Protección de la información

### Copias de seguridad

Resguardo de todo lo importante en otro lado. Se suele hacer en cintas, por lo que toman tiempo y se suelen programar para hacerse por la noche.

Otra forma es copiar los datos a otro disco, en lo posible removible.

Copiar todos los datos puede ser costoso. Una estrategia consiste en:
- Una vez al mes/semana/... hacer una copia total.
- Todas las noches hacer una copia *incremental*, solo los archivos modificados desde la última copia incremental.
- Sino se puede hacer una copia *diferencial*, solo los archivos modificados desde la última copia total.

Para restaurar:
- Si hago solo copias totales, tomo la del día correspondiente y listo.
- Si hago copias diferenciales, necesito la última copia total + la copia diferencial.
- Si lo que tengo son incrementales, se necesita la última copia total, todas las incrementales entre esa copia total y la fecha requerida.

Entonces:
- Hoy = $Ultimo \ total + ultimo \ diferencial$
- Hoy = $Ultimo \ total + \sum_i{incremental_i}$

### Redundancia

A veces una copia de seguridad no nos alcanza, pues el costo de que el sistema salga de línea es muy alto. Entonces, conviene implementar *redundancia*.

Un método común para implementarla es **RAID**: *Redundant Array of Inexpensive Disks*. La idea es usar dos discos: cada escritura se hace en los dos, cosa que si uno de los dos se llega a romper, tenemos el otro. Esta alternativa es muy costosa, pero a su vez nos permite hacer dos lecturas a la vez, una en cada disco.

Hay varios niveles de RAID, que tienen distintas ventajas y desventajas en cuanto a rendimiento y redundancia.

#### RAID 0 (*stripping*)

No aporta redundancia, pero mejora el rendimiento. Los bloques de un mismo archivo se distribuyen en dos o más discos. Esto mejora el ancho de banda y permite escrituras en paralelo, si los discos están en diferentes controladoras.

#### RAID 1 (*mirroring*)

Se espejan los discos, lo que trae un mejor rendimiento de las lecturas. Las escrituras, en mejor caso, tardan lo mismo pero en peor el doble. Es muy caro.

#### RAID 0+1

Se combinan los dos niveles anteriores, el *mirroring* y el *stripping*. En otras palabras, cada archivo está espejado, pero al leerlo se lee un bloque de cada disco. Se leen más rápido que en mirroring simple, es como en stripping. Pero al escribir, tenemos que escribir cada bloque en ambos, como en mirroring.

#### RAID 2 y 3

La idea es tener, por cada bloque, guardada información adicional que permita determinar si se dañó o no. Cierto tipo de errores se pueden corregir automáticamente, recomputando el bloque dañado a partir de la información redundante.

Adicionalmente, cada bloque lógico se distribuye entre todos los discos participantes. RAID 2 requiere 3 discos de paridad por cada 4 de datos, mientras RAID 3 solo 1. Sin embargo, todos los discos participan de todas las I/O, lo cual lo hace más lento que RAID 1.

Se puede requerir mucho procesamiento para computar las redundancias, por lo que se suele implementar por hardware en una controladora dedicada, como todos los niveles siguientes.

#### RAID 4

Como RAID 3, pero hace el stripping a nivel del bloque. El disco dedicado a paridad sigue siendo un cuello de botella para el rendimiento, porque todas las escrituras lo necesitan.

#### RAID 5

Junto a 0, 1 y 1+0, es de los más usados en la práctica. Usa datos redundantes, pero los distribuye en N + 1 discos. No hay un disco que sólo contenga redundancia.

Cada bloque de cada archivo va a un disco distinto, y por cada bloque, uno de los discos tiene los datos y otro tiene la información de paridad. No hay más cuello de botellas, pero hay que mantener la paridad distribuida para las escrituras.

Se puede soportar la pérdida de un disco cualquiera. Cuando se reemplaza y comienza la reconstrucción, el rendimiento se degrada notablemente.

#### RAID 6

Es como RAID 5, pero agrega un segundo bloque de paridad, distribuido entre todos los discos. distribuido entre todos los discos. Las implementaciones varían, pero el objetivo es soportar la rotura de hasta dos discos.

#### RAID no protege contra borrar un archivo accidentalmente

Por eso se combina con copias de seguridad. Si la aplicación corrompe datos, ningún mecanismo sirve. Si se corrompe la estructura interna de los archivos, RAID tampoco sirve, por lo que es por eso que hay sistemas de archivos que brindan algo de protección.