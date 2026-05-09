# Sistema de Archivos

## Proceso de booteo

Es la secuencia de eventos desde que se enciende el hardware hasta que el sistema operativo está listo para su uso.

### Etapas

- **BIOS**/**UEFI**: Inicializa el hardware básico
- **Cargador de arranque**: Selecciona y carga el kernel (por ejemplo GRUB).
- **Kernel**: Inicializa el sistema operativo.
- **init**/**systemd**: Arranca los servicios del sistema.
- **Login**: Inicio de sesión del usuario.

### GRUB (GRand Unified Bootloader)

GRUB permite elegir entre múltiples SO. En Linux, carga el kernel y le pasa el control. Para cargar otro SO, se pasa el control a otro bootloader. Es configurable.

## Archivos

Son secuencias de bytes sin estructura, a las cuales se las identifica con un nombre. Este nombre puede incluir una extensión para así ayudar a distinguir el contenido. Por ejemplo:
- `file.txt` es un archivo con contenido de texto
- `file.c` es un archivo con código fuente en C
- `file.docx` es un archivo con fuente en Microsoft Word.

## Sistemas de archivos

Hay un módulo dentro del kernel que se encarga de organizar la información en el disco: el *sistema de archivos* o *file system*. Algunos SO soportan solo uno (DOS soporta solo `FAT`), otros más de uno (Windows soporta `FAT`, `FAT32`, `NTFS`, ...). Otros, como los UNIX modernos, vienen con soporte para algunos, pero mediante módulos dinámicos del kernel, se puede hacer que soporten cualquiera.

Hay incluso file systems distribuidos, donde los datos están distribuidos en varias máquinas en la red, como por ejemplo NFS, DFS, ... .

Una de las responsabilidades elementales del file system es ver cómo se organizan, de manera lógica, los archivos.
- **Interna**: ¿Cómo se estructura la información dentro del archivo?
- **Externa**: ¿Cómo se ordenan los archivos? Hoy en día todos los file systems soportan el concepto de directorios, lo que trae organización jerárquica en forma de árbol.

Casi todos los file systems, soportan alguna noción de *link*. Este es un alias para un mismo archivo. Teniendo links, la estructura deja de ser arbórea y se vuelve un grafo dirigido, con ciclos y todo.

Además, el file system determina cómo se nombran a los archivos.
- Caracteres de separación de directorio
- Si hay extensiones o no
- Restricciones de longitud y caracteres permitidos
- Distinciones entre mayúsculas y minúsculas
- Punto de montaje
- etcétera

Como por ejemplo:
- `/usr/local/etc/apache.conf`
- `C:\Program Files\Valve\Steam\steam.exe`
- `\\SERVER3\Parciales\parcial1.doc`
- `server4:/ejercicios/practica3.pdf`

## ¿Cómo se representa un archivo?

y otras preguntas:
- ¿Cómo gestiono el espacio libre?
- ¿Qué hago con los metadatos?
Las respuestas a estas preguntas dan las características del file system, especialmente a su rendimiento y confiabilidad.

Para un file system, un archivo es una **lista de bloques** + **metadata**. La forma más sencilla para representarlos es poner los bloques contiguos en el disco.

Las lecturas son rápidas pero, ¿qué ocurre en caso de que el archivo crezca y no tengamos más espacio? o ¿qué hago con la fragmentación?. Es por estos problemas que nadie usa este esquema de file systems R/W.