#  Intro

- Un SO es un *intermediario* entre HW y SW.
	- El SW no se tiene que preocupar por qué HW se esté usando.
	- El usuario pueda usar el HW correctamente, usando abstracciones.
		- Para hacer esto, el SO tiene que correr en **nivel de privilegio 0** (máximo privilegio).

>[!Important] Sistema Operativo
>Sistema Operativo es el Kernel con los Drivers

- **Elementos**
	- *kernel* $\rightarrow$ se encarga de las tareas fundamentales y orquesta los subsistemas que componen.
	- *drivers* $\rightarrow$ manejan los detalles de bajo nivel relacionados con la operación de los dispositivos.
	- *shell* $\rightarrow$ es un programa que permite la interacción entre el usuario y el SO.
	- *procesos* $\rightarrow$ son programas en ejecución con su espacio de memoria asociado.
	- *archivos* $\rightarrow$ que son secuencias de bits con nombre y una serie de atributos para permisos.
	- *directorios* $\rightarrow$ colecciones de archivos y directorios que se organiza jerárquicamente.
	- *dispositivo virtual* $\rightarrow$ abstracción de un dispositivo físico bajo la forma de un archivo.
	- *sistema de archivos* $\rightarrow$ forma de organizar los datos en el disco para gestionar su acceso, permisos, etc.
	- *binarios del sistema* $\rightarrow$ archivos que están en directorios del sistema, que cumplen tareas importantes o básicas.
	- *usuarios* $\rightarrow$ representación, dentro del SO, las personas que pueden usarlo. Sirven para aislar información entre ellos.
	- *grupos* $\rightarrow$ una colección de usuarios

# Procesos

- Son programas en ejecución.
- Cada proceso tiene un identificador único (`pid`)
- Composición desde memoria:
	- área de **texto**: código máquina del programa.
	- área de **datos**: donde se almacena el heap.
	- **stack** del proceso
- Pueden:
	- terminar
	- lanzar un proceso hijo
	- ejecutar en la CPU
	- hacer una syscall
	- realizar E/S.

- Se organizan en un árbol de procesos.
- Cuando el SO inicia, lanza un proceso que se llama *init* o *systemd*.
- Syscalls:
	- `fork()` llamada que crea un proceso idéntico al actual.
		- El resultado de `fork()` es el `pid` del proceso hijo, copia exacta del padre.
	- `wait()` llamada que permite al padre decidir suspenderse hasta que termine el proceso hijo.
		- Cuando el hijo termina, el padre obtiene el status code del hijo.
	- `exec()` llamada que permite a un proceso hijo reemplazar su código binario por otro. 

- **Ejecutar en CPU**
	- Operaciones:
		- Hacer operaciones entre registros y direcciones de memoria
		- I/O
		- System calls
	- ¿Cuánto tiempo?
		- Hasta que termina. Mejor para el proceso, pero no para el sistema (imaginemos que no termina).
		- Un *quantum*.
		- Los SO modernos hacen preemption, una vez que se acaba el quantum, cambiamos a otro proceso.
		- Virtualizamos la CPU de manera tal que cada proceso tenga su propia CPU.
		- Entra en acción el **scheduler**.
	- **Scheduler**
		- Componente ESENCIAL del SO.
		- Decide a qué proceso le corresponde ejecutar en un determinado momento.
		- Es uno de los componentes que mayor impacto en la performance tienen sobre el SO.
	- Cambios de Contexto $\rightarrow$ Guardamos el estado del programa que se está ejecutando a fin de cambiar al próximo.
		- Guardamos registros
		- Guardamos PC
		- Si es un programa nuevo, cargarlo en memoria
		- Cargar los registros del nuevo
		- Poner el valor del PC del nuevo
		- ...
		- **Este estado se guarda en una estructura llamada PCB (Process Control Block)**.
		- El tiempo usado en el cambio de contexto es tiempo muerto.
		- Se usa la *interrupción del clock* para realizarlos.

- **Hacer syscalls**
	- En todas se debe llamar al kernel.
	- Requieren un cambio de privilegio, un cambio de contexto, e incluso algunas **una interrupción**.
	- Proveen una interfaz a los servicios brindados por el SO: la **API** del SO.
	- En Linux se usa la interrupción **0x80**. El número de syscall va por el registro **EAX**.
	- Se usan _wrapper functions_ en C.
		- Mayor portabilidad y sencillez
	- Cumplen el standard POSIX
		- IEEE 1003.1
		- Incluye
			- pipes
			- creacion de procesos
			- señales
			- libc
			- excepciones
			- E/S
			- operaciones de archivos y dirs
			- errores del bus

- **E/S**
	- Es lenta
	- Introduce **busy waiting**. Gasta ciclos de CPU. Hay otras alternativas:
		- Polling
		- Interrupciones
		- ...
	- Busy waiting
		- El proceso no libera la CPU. Un único proceso en ejecución por vez.
	- Polling:
		- El proceso libera la CPU, pero recibe quantums que desperdicia hasta que esté lista la E/S
	- Interrupciones:
		- El SO no da más quanta al proceso hasta que se termine la E/S.
		- El HW comunica al SO mediante una interrupción que terminó la E/S.
		- La interrupción es atendida y despierta el proceso.

- **Multiprogramación**
	- Aumento de rendimiento.
	- Correr un trabajo solo tarda lo mismo, pero sucesiones de trabajos tardan menos.
	- Capacidad de un SO de tener varios procesos en ejecución.
	- Desde el código, hay dos formas de implementarla:
		- Bloqueante: hago la syscall, para cuando el proceso recibe el control la E/S ya terminó. Mientras se bloquea el proceso
		- No Bloqueante: hago la syscall, que retorna enseguida. Se pueden seguir haciendo otras cosas, pero hay que implementar un mecanismo de aviso que terminó la E/S.
	- Introduce el **ESTADO de un proceso**
		- **Running**: Está usando la CPU
		- **Blocked**: No puede correr hasta que algo suceda
		- **Ready**: El proceso no está bloqueado, pero no tiene CPU para correr.
	- Tenemos una lista de PCBs, que se llama **Tabla de procesos**, que además del PCB, guarda su estado, su prioridad y los recursos que está esperando a que finalicen.
	- **SEÑALES**
		- Mecanismo que incorporan los sistemas POSIX, el cual permite notificar a un proceso la ocurrencia de un evento.