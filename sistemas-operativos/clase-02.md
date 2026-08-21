# Comunicación entre procesos (IPC)

- Hay procesos que necesitan compartir info, mejorar la velocidad de procesamiento o modularizarse con otros procesos.
- Para esto esta la IPC.
- Formas:
	- Memoria compartida.
	- Recursos compartidos (BBDD, archivos, otra computadora, ...)
	- Mediante mensajes, entre procesos en la misma máquina o en la red.

## File descriptors

- Son índices de una tabla que indica los archivos abiertos por el proceso.
- Hay file descriptors preconfigurados:
	- `stdin` $\rightarrow$ 0
	- `stdout` $\rightarrow$ 1
	- `stderr` $\rightarrow$ 2
- Syscalls para leer/escribir:
	- `ssize_t read(int fd, void *buf, size_t count)`
	- `ssize_t write(int fd, void *buf, size_t count)`
	- `dup2(int oldfd, int newfd)`
## Pipes

- *pseudo archivo*.
- Definen file descriptors que redirigen la salida de un proceso a la entrada de otro.
	- Es decir, redirige el `stdout` de un proceso al `stdin` de otro proceso.
- Es un archivo temporal y anónimo.
- Es un buffer para leer y escribir en forma **secuencial**.
- Creados por la syscall `int pipe(int pipefd[2])`
- Despues de ejecutar tenemos
	- En ``pipefd[0]`` tenemos la lectura (entrada).
	- En `pipefd[1]` tenemos la escritura (salida).
- Si usamos fork, se heredan los extremos de los pipes.

## Sockets

- Interfaz de comunicación que permite el intercambio de datos.
- Dos tipos en UNIX:
	- Sockets Unix
		- Locales
		- Es un archivo con permisos.
	- Sockets de red
		- Remotos
- Estándar POSIX sockets para las syscalls para operar con ellos.
- 