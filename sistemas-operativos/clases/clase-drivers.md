# Administración de E/S

- Focus en almacenamiento
	- Discos rígidos, extraibles, cintas.
	- Discos virtuales: NAS
	- Otros: SAN, almacenamiento en red, pero especial.

-  E/S = Dispositivo físico + Controlador de dispositivo (el cual interactúa con el SO por un bus o registro).
	- Ej: PCI
- **Driver**: código del fabricante del dispositivo para que el usuario pueda comunicarse con el dispositivo.
	- Son clave, corren a máximo privilegio y de ellos depende el rendimiento E/S, fundamental para el rendimiento del SO.
- API
	- `open() / close()`
	- `read() / write()`
	- `seek()` <- específica de almacenamiento. Busca algo.
- Tipos de transferencia de dispositivos.
	- **Char device:** mandan byte a byte. (por ejemplo un teclado)
	- **Block device:** mandan de a bloques de bytes. (por ejemplo un disco rígido)
- En UNIX, todo es un archivo, entonces hay funciones de alto nivel para acceder a archivos:
	- todas las que sean `fxxxxx()`.
- Queremos minimizar las operaciones de E/S, porque son lentas
	- Por ejemplo en los casos de los discos, traer datos de a bloques, no solo un dato concreto.
- Scheduling en discos, buscamos el algoritmo más óptimo para mejorar los tiempos de respuesta y el mover la cabeza de un lado a otro en busca de datos.
	- Un FCFS, haría que el disco vaya de un lado a otro.
	- SSTF, es decir, ir al más cercano, puede provocar inanición.
	- Atender pedidos yendo en un solo sentido, etc etc.

## RAID

- **R**edundant **A**rray of **I**nexpensive **D**isks.
- Copiamosn los datos en muchos discos.
- Hay varios standards.

### RAID 0 (stripping)

- No da redundancia.
- Mejora rendimiento.
- Se distribuyen los bloques de un archivo en los discos.
- Mejora el bandwith, permite escrituras en paralelo.

### RAID 1 (mirroring)

- Duplica la información en ambos discos.
- Mejora el rendimiento de las lecturas.
- Las escrituras tardan lo mismo o el doble.
- Es muy caro, pues reducimos el almacenamiento en $\frac{1}{\#discos}$
- Si se te corrompe un bit, podemos detectar errores, pero **no sabemos dónde**.

### RAID 5
