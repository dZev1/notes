# Sistema de archivos

## Proceso de booteo

- Secuencia de eventos que inicia un SO desde que se enciende el hardware hasta que el sistema está listo para el uso.
- **Etapas**:
  - BIOS/UEFI: inicializa el hardware básico.
  - Bootloader (ej. GRUB): selecciona y carga el kernel.
  - Kernel: inicializa el SO.
  - init/systemd: arranca los servicios del sistema.
  - Login: inicio de sesión del usuario.

### MBR (BIOS)

- El MBR (Master Boot Record) es un esquema tradicional de particionado.
- El primer sector contiene código de arranque y una tabla de particiones.
- Hasta 4 particiones primarias; una puede ser extendida para contener particiones lógicas.
- Sectores lógicos de hasta 512 bytes.
- Direcciona hasta 2TB de disco.

### GPT (UEFI)

- GPT (GUID Partition Table) es el esquema moderno de particionado.
- Permite discos y particiones de mayor tamaño que MBR
- Admite muchas particiones, siendo 128 una configuración habitual.

### GRUB

- GRUB (GRand Unified Bootloader) permite elegir entre distintos SSOO.
- En linux, carga el kernel y le pasa el control.
- Para cargar otro SO, pasa el control a otro bootloader.

## Sistemas de Archivos

- Un *archivo* es una **Secuencia de bytes, sin estructura**.
- Tienen un nombre que los identifica.
- Este nombre puede incluir una extensión para ayudar a distinguir el contenido.

- Un *sistema de archivos* (o *file system* (*fs* si acortamos)), es un módulo dentro del kernel que se encarga de organizar la información en disco.
	- Algunos SO soportan solo uno.
	- Otros más (Windows soporta **FAT**, **FAT32**, **NTFS**, ...).
	- Otros, como el caso de UNIX modernos, vienen con soporte para algunos pero mediante módulos dinámicos de kernel se puede soportar casi cualquier fs.
	- Algunos fs importantes: UFS, FFS, `ext2`, `ext3`, `ext4`, XFS, ZFS, ISO-9660, BTRFS.
	- Hay hasta fs distribuidos, como por ejemplo NFS, DFS, SMBFS, AFS, ...

- Tienen como responsabilidad elemental ver cómo se organizan de manera lógica los archivos.
	- **Internamente**: cómo se estructura la información dentro del archivo.
	- **Externa**: cómo se ordenan los archivos. Hoy en día casi todos soportan directorios/carpetas, con una organización jerárquica en forma de árbol.
- Casi todos soportan un *link*, un alias para el mismo archivo.
	- Teniendo links, la estructura deja de ser arbórea y se vuelve un grafo dirigido, con ciclos y todo.
- Además, determina cómo nombrar a los archivos.
	- Cómo separo los directorios? (`\` en Windows `/` en UNIX).
	- Hay extensiones?
	- Hay longitud restringida?
	- Hay caracteres que no se permiten?
	- Diferencio mayúsculas de minúsculas?
	- Cuál es el **punto de montaje**.
- Qué coño e el punto de montaje?
	- Si tenemos más de un disco, al montarla debemos indicar cómo referirnos a ella, y qué punto del grafo de nombres de archivos iba a ser la raíz de este almacenamiento.
	- Entonces podemos tener cosas como `/mnt/discos/datos/Fernandez/planilla.xlsx`.
- Tenemos que ver tres cosas:
	- **¿Cómo se representa un archivo?**
	- **¿Cómo gestiono el espacio libre?**
	- **¿Qué hago con los metadatos?**.
- Estas tres preguntas detereminan el rendimiento y confiabilidad del FS.

## Representación de archivos

- Para el FS, un archivo es sinónimo de **bloques de bytes + metadata**.
- Primera opción: pongamos todos los bloques contiguos en disco.
- Lecturas rápidas pero:
	- ¿Qué pasa si el archivo crece y no hay más espacio?
	- ¿Qué hago con la Fragmentación?
- Bueno entonces hagamos una Linked List de bloques.
	- Pero ahora, las lecturas consecutivas son rápidas, pero las aleatorias son lentas.
	- Desperdiciamos además espacio de cada bloque diciendo dónde está el siguiente.
- Agreguemos una tabla que por cada bloque, te dice dónde está el siguiente en la lista.
	- Es muy grande la tabla, no se sostiene en casos grandes.
- ¿ENTONCES?

- Sigamos el approach UNIX, con los *inodos*.
- Cada archivo tiene un inodo.

- En las primeras entradas hay atributos (tamaño, permisos, etc).
- Después están las direcciones de algunos bloques.
	- Permite acceder rápidamente a archivos pequeños, de 8KB (tamaño mínimo de página) a 96KB.
- Sigue una entrada que apunta a un bloque llamado **single indirect block**.
	- Tiene punteros a bloques de datos, para archivos de hasta 32GB.
- Sigue un **triple indirect block**
	-  Apuntan a bloques de **double indirect blocks**, cubriendo hasta 70TB.

- Permiten los inodos tener en memoria solo las tablas correspondientes a los archivos abiertos.
- Una tabla por archivo implica menos contención.
- Es consistente, pues solo están en memoria los archivos abiertos.

## Directorios 

- Para implementar directorios, usamos un inodo raíz que sirve como entrada al directorio root.
- Por cada archivo o directorio dentro del directorio hay una entrada.
- Cuando los directorios son grandes, conviene pensarlos como una hash table, que como una lista de nombres, a modo de facilitar búsquedas.

## Atributos

- Cuando hablamos de metadata, se incluyen los inodos, pero además otra información.
	- Permisos
	- Tamaños
	- Owner
	- Etc

## Espacio libre

- ¿Cómo administramos el espacio libre?
- Una técnica puede ser tener un mapa de bits, donde 1 sea libre.
- Requiere tener un vector en memoria.
- Podemos usar lista enlazada de bloques libres.
- En general clusterizamos.
	- Si tenemos un bloque de disco que puede tener $n$ punteros a otros bloques, los primeros $n-1$ indican bloques libres y el último apunta al próximo nodo de la lista.
	- Si además agregamos que cada nodo de la lista indique cuántos bloques consecutivos hay a partir de él, god.

## Rendimiento

- Podemos usar un **cache**.
- Se maneja de manera muy similar a las páginas.
- Los SO modernos manejan un cache unificad para ambas, si no, si mapeamos archivos en memoria, tenemos dos copias del mismo.
- El cache puede grabar las páginas de manera ordenada, entonces el administrados I/O puede planificar más eficientemente la escritura.

## Journaling

- Algunos FS, llevan un log para registrar los cambios que habrían que hacer.
- Cuando se baja el cache a disco, se actualiza una marca indicando qué cambios ya se reflejaron.
- Si se llena el buffer, se baja el cache a disco.
- Tiene un impacto bajo en la performance
	- Se escribe en bloques consecutivos, lo que es más rápido que una aleatoria.
	- Si no se hace journaling, se escriben a disco inmediatamente los cambios en la metadata para evitar daños en los archivos.
- Cuando levantamos el sistema, se aplican los cambios aún no aplicados.

## NFS

- Network File System es un protocolo que permite acceder a FS remotos como si fuesen locales, usando RPC.
- Se monta en algún punto del sistema local, y las aplicaciones acceden a archivos de ahí, sin saber que son remotos.
- Para soportarlo, los SO incorporan una capa que se llama **Virtual File System**.
- Esta capa tiene vnodes por cada archivo abierto.
- Así, los pedidos de IO que llegan al VFS, son despacados al FS real, o al cliente de NFS que maneja el protocolo necesario.
- Si bien del lado del cliente es necesario un módulo de kernel, del lado del server aclanza con un programa común y corriente.

## LVM

- Es un sistema de administración de volúmenes lógicos, que proporciona mayor flexibilidad en la gestión del almacenamiento.
- Permite
	- Redimensionar particiones fácilmente
	- Crear snapshots
	- Mejor uso del espacio en disco.
	- Realizar operaciones de ampliación de un volumen y su FS en caliente, sin downtime.
- Componentes
	- Volumen Físico
	- Grupo de Volumenes
	- Volumen Lógico.