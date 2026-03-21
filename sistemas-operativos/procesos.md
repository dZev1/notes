# Procesos

## Programas y Procesos

Un programa es una secuencia de pasos escrita en algún lenguaje, el cual se compila eventualmente en código objeto, el cual es un programa escrito en lenguaje máquina.

Una vez que ejecutamos este programa, tenemos un **proceso**. A cada proceso se le da un identificador único, el *pid* o *process identifier* o *process id*.

## Composición

- **ÁREA DE TEXTO:** Aquí se encuentra el código de máquina del programa.
- **ÁREA DE DATOS**: Aquí se almacena el heap.
- **STACK**: Es un stack propio del proceso, en él se almacenan las variables locales y el contexto de ejecución completo.

## ¿Qué puede hacer un proceso?

### **Terminar**

- El proceso indica al SO que puede liberar todos sus recursos (`exit()`).
- Se indica a su vez un estado de terminación, con un código numérico.
- Se reporta este estado al padre.

### **Lanzar un proceso hijo**, mediante syscalls

Los procesos se organizan jerárquicamente en un **árbol de procesos**. Cuando el SO comienza, este lanza un proceso que se llama `init` o `systemd`.

Por eso es importante la capacidad de lanzar procesos hijos:
- `fork()` es una syscall que crea un proceso idéntico al actual, el resultado es el `pid` del proceso hijo.
- El proceso padre suele suspenderse hasta que el proceso hijo termine con la syscall `wait()`. Cuando el hijo termina, el padre obtiene el status code del hijo.
- El proceso hijo puede hacer lo mismo que el proceso padre, o algo distinto, cambiando el binario de ejecución con la syscall `exec()`.

Cuando lanzamos un programa desde nuestro `shell`, el shell hace un `fork()`, y el hijo hace un `exec()` al programa.

*¡Para visualizar el árbol de procesos de tu sistema, podés usar `pstree`!*

### **Ejecutar en la CPU**

Hacer operaciones entre registros y direcciones de memoria. Esto podríamos hacerlo:
- **Hasta que se termina** $\rightarrow$ lo mejor para el proceso, pero no para el SO. Pues podría hasta no terminar.
- Un **quantum**, un lapso pequeño de tiempo.
En general los SO hacen *preemption*: cuando se acaba el quantum, se ejecuta el siguiente proceso. Entonces, ¿Quién decide a qué proceso le toca ejecutarse en un determinado momento? ¿Cómo lo decide? Presentamos el *Scheduler*.

#### *Scheduler*

Es parte fundamental del kernel y es quien decide a qué proceso le corresponde ejecutar en cada momento. Hay varias formas de decidir esto, y es muy importante elegir una buena política de scheduling, pues es una de las cosas que tiene el mayor impacto en el rendimiento de un SO.

Abundamos más en **[[scheduling|SCHEDULING]]**

#### Context Switch

Para cambiar el programa que se ejecuta en la CPU, el SO debe:
- Guardar los **registros**.
- Guardar el **Program Counter/Instruction Pointer**.
- **Cargar el nuevo programa** en memoria.
- **Cargar los registros** del nuevo.
- Poner el **valor del PC/IP del nuevo**.
- Otras cosas.

A esto se lo llama **Context Switch**. El PC/IP y los demás registros se guardan en una estructura de datos llamada *Process Control Block*, o **PCB**. El tiempo que se utiliza para los context switch es tiempo muerto, no se hace nada productivo. Entonces surgen dos consecuencias:
- A nivel de arquitectura del hardware: procesadores **RISC**.
- Determinar un quantum apropiado para minimizar los context switch.
Para implementar los context switch nos arraigamos a la **interrupción del clock**.

### Hacer una **system call** (syscall).

Un proceso puede hacer llamadas al sistema, como las ya vistas y detalladas más abajo en **[[procesos#Process API (UNIX/GNU Linux)|API DE PROCESOS]]** `fork()`, `exec()` y `wait()`. Hay syscalls más comunes como la que imprime en pantalla `write()`.

En todas ellas se debe llamar al kernel. A diferencia de una subrutina común, las syscalls requieren cambiar el nivel de privilegio, un cambio de contexto o hasta una **interrupción**, entre otras.

Las syscalls proveen una **interfaz** a los servicios brindados por el SO: **la API del SO**. La mayoría de los programas hacen uso intensivo de ellas.

Para implementarlas, se utiliza una interrupción para pasar a *modo kernel*, y los parámetros se pasan utilizando registros o una tabla en memoria. En Linux, esta interrupción es `0x80` y el número de syscall se pasa en el registro `EAX` o `RAX`.

Normalmente se usan a través de wrapper functions en C, pues esto le da más portabilidad, pues no nos arraigamos a una sola arquitectura, y no requerimos programar en lenguaje ensamblador. Las wrapper functions permiten la interacción con el sistema con mayor *portabilidad* y *sencillez*.


### Realizar E/S a los dispositivos.

La E/S es muy lenta. Quedarse bloqueado es desperdiciar tiempo porque involucra hacer *busy waiting*, donde el proceso no libera la CPU, gastando ciclos en stall. Hay alternativas:
- Polling $\longrightarrow$ El proceso libera la CPU, pero todavía recibe un quantum que desperdicia hasta que la E/S se complete. 
- Interrupciones $\longrightarrow$ Permite la multiprogramación.
	- El SO no le otorga más quantums al proceso hasta que su E/S esté lista.
	- El hardware comunica que la E/S terminó mediante una interrupción, la cual es atendida por el SO, que en ese momento *despierta* al proceso.
- Otras.

## Recap de conceptos

- **Multiprocesador**: un equipo con varios cores/procesadores.
- **Multiprogramación**: la capacidad de un SO de tener varios procesos en ejecución.
- **Multiprocesamiento**: tipo de procesamiento que sucede en los multiprocesadores en la multiprogramación.
- **Multitarea**: Una forma especial de la multiprogramación, donde la conmutación entre procesos se hace tan rápido que da la sensación que tenemos varios programas corriendo en simultáneo.
- **Multithread**: Procesos en los cuales hay varios "mini procesos" corriendo en paralelo, ya sea real o ficticiamente, como sucede en la multiprogramación.

## Multiprogramación desde el código

Hay dos formas de conseguir multiprogramación desde el código:
- **Bloqueante** $\longrightarrow$ hago la syscall, para cuando recibo el control terminó la E/S. Mientras me bloqueo.
- **No bloqueante** $\longrightarrow$ hago la syscall, la cual retorna en seguida. Puedo seguir haciendo otras cosas. Necesitamos una manera de saber que mi E/S terminó.

Tenemos la syscall `select()` la cual se utiliza `select(..., *lectura, *escritura, *excepción, timeout)`, donde lectura, escritura y excepción son conjuntos de E/S *pendientes*. Vuelve al pasar el timeout o cuando alguna E/S está lista o dio error.

Esto da origen al concepto de **estado del proceso**. Tenemos cuatro:
- **Running**: el proceso está siendo ejecutado en un procesador.
- **Ready**: El proceso puede ser ejecutado pero no está siendo ejecutado porque no hay CPU disponible para correrlo.
- **Blocked**: El proceso hizo una operación que no permite su ejecución hasta que esta no termine (E/S por ejemplo).

$$Carga \ del \ sistema = \# (Procesos \ READY)$$
Es responsabilidad del scheduler elegir entre los procesos listos cuál es el próximo a correr. Esto está determinado por la política del scheduling. Sin embargo, necesitamos tener una lista de procesos.

En realidad tenemos una lista de PCBs, llamada *tabla de procesos*. En cada PCB, se guarda la prioridad del proceso, su estado y los recursos que se están esperando. Suelen formar también una lista enlazada que comienza en cada recurso por el que están esperando.

## Señales

Las señales son un mecanismo que incorporan SOs POSIX. Permiten notificar a un proceso la ocurrencia de un evento. Cuando un proceso recibe una señal, su ejecución se interrumpe y se ejecuta un handler. Cada tipo de señal tiene asociado un handler por defecto, que se puede modificar con la syscall `signal`.

Toda señal tiene asociado un número que identifica su tipo, los cuales están definidos como constantes en el header `<signal.h>`. Por ejemplo: `SIGINT` o `SIGKILL`. Las señales `SIGKILL` y `SIGSTOP` no pueden ser bloqueadas, ni es posible reemplazar sus handlers.

Si el usuario envía una señal a un proceso con el *comando* `kill`, este puede enviar una señal a otro con la *syscall* `kill()`.

## Process API (UNIX/GNU Linux)

**¿Cómo creo y controlo procesos? ¿Qué interfaces nos debería dar el sistema operativo para esto? ¿Cómo deben estar diseñadas para ser fáciles de usar, poderosas y con alta performance?**

La API de procesos de UNIX trae consigo tres llamadas de sistema para esta tarea: `fork()`, `exec()` y `wait()`.

### Syscall `fork()`

Esta syscall es la utilizada para crear procesos. Veamos que hace con el siguiente código:

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  printf("hello (pid:%d)\n", (int) getpid());
  int rc = fork();

  if (rc < 0) {
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  } else if (rc == 0) {
    printf("child (pid:%d)\n", (int) getpid());
  } else {
    printf("parent of %d (pid:%d)\n", rc, (int) getpid());
  }
  return EXIT_SUCCESS;
}
```

Cuando corremos este código, tenemos el siguiente output:

```bash
dz $ ./main
hello (pid:11147)
parent of 11148 (pid:11147)
child (pid:11148)
dz $ 
```

Es decir, cuando corremos el programa, este primero obtiene su PID, o Process Identifier mediante la función `getpid()`, este es el *nombre* que tiene el proceso en el sistema (para los sistemas UNIX/GNU Linux).

Luego, cuando llama a la syscall `fork()`, el SO provee una forma de crear un proceso nuevo. Este es una copia del proceso **llamador** o proceso **padre**, (el programa `main`). Esto significa que para el SO, tenemos dos procesos `main` corriendo, ambos a punto de retornar de la syscall `fork()`. El proceso creado (llamado proceso **hijo**), no comienza a correr desde el punto de entrada `main()`, como se puede ver en el prompt, no hizo su `hello (pid:11148)`, sino que corre desde luego de la llamada a `fork()`

Para el proceso padre, `fork()` devuelve el PID del proceso hijo, mientras que para el proceso hijo, `fork()` devuelve `0` o `NULL`.
El output del programa `main` no es determinístico, la CPU puede correr tanto el proceso hijo como el proceso padre luego de `fork()`. Si estuviésemos en una CPU de un solo core, el output podría ser:

```bash
dz $ ./main
hello (pid:11147)
child (pid:11148)
parent of 11148 (pid:11147)
dz $ 
```

Este no determinismo lleva a problemas interesantes, sobre todo para programas **multi-threaded**.

#### Aclaraciones

Cuando se llama a `fork()`, por debajo se llama a la syscall `clone`. Este es un mecanismo para crear procesos. Podemos determinar sobre qué contextos de ejecución comparten padre e hijo.

Se puede controlar si se quiere que compartan el espacio virtual, el stack, dónde se arranca la ejecución, entre otras cosas. Tanto procesos como threads usan esta syscall con sus parámetros correspondientes.

### Syscall `wait()`

A veces, es útil que un proceso padre espere a que termine la ejecución de su proceso hijo. Es por eso que se utiliza la syscall `wait()`.  Es decir, frena la ejecución del padre hasta que termine la ejecución del hijo. Una vez termine, `wait()` retorna al padre.

Si al programa `main` de antes, lo modificamos de manera tal que quede:

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  printf("hello (pid:%d)\n", (int) getpid());
  int rc = fork();

  if (rc < 0) {
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  } else if (rc == 0) {
    printf("child (pid:%d)\n", (int) getpid());
  } else {
	int rc_wait = wait(NULL);
	printf("parent of %d (rc_wait:%d) (pid:%d)\n", rc, rc_wait, (int) getpid());
  }
  return EXIT_SUCCESS;
}
```

Ahora el output del programa es determinístico, pues esperamos siempre a que termine el proceso hijo antes de hacer el print del padre. El output siempre es

```bash
dz $ ./main
hello (pid:11147)
child (pid:11148)
parent of 11148 (pid:11147)
dz $ 
```

### Syscall `exec()`

La llamada a esta syscall permite ejecutar programas distintos al que se está ejecutando. Contrario a solo hacer `fork()` donde se ejecuta el mismo código desde la llamada a la misma syscall.

Veamos con este fragmento de código

```C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  printf("hello (pid:%d)\n", (int) getpid());
  int rc = fork();

  if (rc < 0) {
    fprintf(stderr, "fork failed\n");
    exit(EXIT_FAILURE);
  } else if (rc == 0) {
    printf("child (pid:%d)\n", (int) getpid());
    
    char *my_args[3];
    my_args[0] = strdup("wc"); // el código que queremos ejecutar
    my_args[1] = strdup("p3.c"); // el archivo actual
    my_args[2] = NULL; // fin del array
    
    execvp(my_args[0], my_args);
    printf("This will not be printed!\n");
  } else {
	int rc_wait = wait(NULL);
	printf("parent of %d (rc_wait:%d) (pid:%d)\n", rc, rc_wait, (int) getpid());
  }
  return EXIT_SUCCESS;
}
```

Este código nos da el siguiente output:

```bash
dz $ ./p3
hello (pid:17883)
child (pid:17884)
 30  96 767 p3.c
parent of 17884 (rc_wait:17884) (pid:17883)
dz $ 
```

`exec()`, dado el nombre de un ejecutable y algunos argumentos, **carga** el código del ejecutable y sus datos estáticos y reemplaza el segmento de código con el del programa a ejecutar. Esto quiere decir que el heap, el stack y otros lugares de memoria del programa se reinician. Luego, el SO pasa los argumentos como `argv` y ejecuta el proceso.

Es decir, no crea un nuevo proceso, sino que transforma el proceso que corre actualmente por otro proceso. Luego de `exec()` en el proceso hijo, es como si `p3` no hubiese sido ejecutado, por lo que nunca retorna del `exec()`

### ¿Por qué complicarse y no ejecutar simplemente el proceso?

Pensemos que `fork()` permite hacer un setup para luego ejecutar el programa, de manera más sencilla o más asequible en medio de otro proceso o dentro del mismo código. Podemos estar haciendo logging dentro de un proceso, hacer `fork()` y hacer el setup de este mismo, pues recordemos que el proceso padre recibe el número del child, el child recibe 0 y cualquier falla recibe un numero <0,  podemos separar en casos más sencillamente. Luego, al padre le ordenamos que espere a que su proceso hijo termine de realizar lo que deba hacer, y seguir con nuestra ejecución del programa. Es MUY util.

### Otras syscalls importantes

`kill()` envía señales al proceso, ya sea un **SIGINT** (interrupt), **SIGSTP** (stop), para interrumpir (en muchos casos terminándolo) o pararlo en medio de la ejecución.

`signal()` captura las señales enviadas por el usuario.