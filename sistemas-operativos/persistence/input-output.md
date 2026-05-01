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