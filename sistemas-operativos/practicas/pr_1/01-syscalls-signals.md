# Syscalls y Señales

## Syscalls

- Interacción con el SO
	- **Usuario** $\rightarrow$ programas o utilidades.
	- **Programadores** $\rightarrow$ *syscalls*
- Proveen una interfaz a los servicios que brinda el SO: es la API del SO.
- Se usa una interrupción que pasa a modo **kernel**.
	- Los parámetros se pasan usando registros o una tabla en memoria.
- Se suelen usar a través de **wrappers en C**.
	- Por un tema de portabilidad y utilidad.

## Creación de procesos

### `fork()`
- Crea un proceso, que es clon del primero.
- Este clon se llama proceso **hijo** y el original es el proceso **padre**.
- Devuelve al proceso que la llamó el `pid` del proceso hijo.
- El proceso hijo corre desde la instrucción que devuelve la llamada a `fork()`.
- Ambos procesos corren en espacios de memoria separados. Es decir, no comparten memoria.
- El valor que recibe el hijo tras la llamada a `fork()` es `0`, el padre recibe el `pid` del hijo.
- Entonces podemos separar el código:
	- si `pid == 0`, entonces estamos en el proceso **hijo**.
	- si `pid != 0`, entonces estamos en el proceso **padre**.
	- si `pid < 0`, entonces hubo errores.
- No tenemos control sobre el orden en que se ejecutan los procesos.
 
### `getpid()` y `getppid()`

- Devuelven el valor del PID del proceso en ejecución y el del proceso padre, respectivamente.

### `wait()` y `waitpid()`

- Syscalls que bloquean un proceso hasta que se reciba una actualización del estado de uno de sus hijos (`wait`) o un hijo específico (`waitpid`).

### `pause()`

- Bloquea un proceso hasta que se reciba **cualquier** señal.
- Más propenso a **condiciones de carrera!**.

### `exec()`

- Familia de syscalls que reemplaza el código actual del programa y toda su memoria por la de un programa nuevo.

## Comunicación entre procesos mediante envío de mensajes

### Señales

- Header `signal.h`.
- Cada señal es un número, pero usamos un macro para llamarlas comúnmente.
- Un comando puede mandar desde la terminal una señal a un proceso con `kill`. Entre procesos se pueden enviar señales con la syscall `kill()`.
- Se pueden redefinir en cuanto a comportamiento se refiere, usando **handlers** sin parámetros.
- Para ello, se usa la función en C `signal(signal, handler)`.
- **No se pueden redefinir todas las señales.**
	- Las señales `SIGKILL` y  `SIGSTOP` son ejemplos de esto.
- Las redefiniciones se heredan.
- Si haces `exec`, se pierden las redefiniciones.
- Hay distinción entre procesos `root` y `no-root`. Aquellos procesos no-root no pueden enviar señales a procesos `root`

### `SIGCHLD`

- Cuando un proceso hijo termina su ejecución, envía a su proceso padre la señal SIGCHLD.
- También se envía cuando cambia el estado del proceso hijo.