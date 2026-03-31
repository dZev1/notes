# Scheduling

Queremos que la CPU sea compartida y *virtualizada* por todos los procesos. Queremos a su vez correr los procesos de manera tal que en un quantum corra el proceso A y al acabarlo, cambiemos de contexto al proceso B. A esto se le llama **Time Sharing**

## Mecanismo de Time Sharing: Limited Direct Execution

Es una técnica que se basa en correr directamente el programa en la CPU. Entonces, cuando la CPU quiere correr un nuevo programa, crea una nueva entrada en la lista de procesos, asigna memoria al proceso, lo carga en memoria, encuentra su punto de entrada (`main()` o lo que sea), salta al mismo y corre el código del usuario. Una vez que se retorna del `main()`, se libera la memoria del proceso y se elimina de la lista de procesos.

Todo muy simple, pero puede traer problemas a la hora de virtualizar la CPU:
- Si solo corremos el programa, ¿cómo hace el SO para asegurarse de no hacer nada que no queremos que haga, pero manteniendo la performance?
- Cuando corremos un proceso, ¿cómo hace el SO para que deje de correr el proceso actual y cambie a otro proceso, implementando así el *time sharing* para virtualizar la CPU?

### Problema 1: Operaciones Restringidas

Solución: **Syscalls**. Cuando un proceso necesita ejecutar una operación restringida, como efectuar E/S, el programa efectúa una syscall, la cual está implementada como una rutina en el espacio de **traps** del sistema operativo. Este espacio está a nivel de privilegio de **kernel**, y no de usuario. Una vez que se efectúa la syscall pedida, se utiliza una instrucción de retorno especial, que baja el nivel de privilegio nuevamente a modo usuario.

El hardware debe recordar de preservar el estado anterior a efectuar la syscall, pues no queremos alterar el contexto de ejecución anterior a la syscall, a excepción de aquello que se haya pedido, claro está.

El Kernel cuando bootea hace un setup de una tabla de traps, donde guarda todas las direcciones de los handlers de las syscalls. A cada entrada se le asigna un número, que es el número que luego debe pasarse para acceder a la syscall.

Hay dos fases en el protocolo **LDE**. En la primera, el kernel inicializa la trap table, y la CPU recuerda su localización para usos subsecuentes. En la segunda, el kernel hace el setup de algunas cosas, como crear un nodo en la lista de procesos, antes de usar la instrucción de retorno de una trap, permitiendo ahora correr el proceso en modo usuario.

Cuando el proceso quiere realizar una syscall, cambia a modo kernel nuevamente, la ejecuta y vuelve a modo usuario tras correr el handler de la misma. Una vez que termina el proceso y retorna el status code de `main()`, se ejecuta la finalización del programa (por ejemplo llamando a `exit()`), y el SO limpia todo lo que haya creado el proceso (archivos temporales, PCB, descriptores, memoria alojada, etc).

### Problema 2: Cambio de procesos

Debería ser algo simple, dejar de correr un proceso e iniciar el próximo, pero... Si estoy corriendo un proceso en la CPU, no está corriendo el SO, ¿no? Sip, y no puede hacer nada si no está corriendo. Entonces, ¿Cómo retomo el control de la CPU?

#### Approach **cooperativo**: esperar una syscall

Es un approach que tomó Apple en su momento con los SO de las primeras Macintosh. El SO *confía* en que los procesos del sistema van a devolver el control de la CPU al mismo. Se asume que los procesos que corren por mucho tiempo le dan periódicamente el control de la CPU de nuevo al SO para que este decida si corre otra tarea.

Los procesos dan control de la CPU al SO cuando ejecutan **syscalls**, ya sea para abrir archivos, enviar mensajes a otro proceso o para ejecutar un nuevo proceso. Estos procesos suelen tener una instrucción `yield` que lo único que hace es darle el control de la CPU al SO.

Los procesos también dan el control de nuevo al SO cuando hacen algo ilegal, como dividir por cero, o acceder a memoria que no puede acceder, salta el **trap** asociado a esa excepción. Esto da el control nuevamente al SO, que seguramente termine el proceso.

Ahora, ¿qué pasa si un programa termina en un loop infinito, y nunca hace una syscall? ¿Qué puede hacer la CPU?

#### Approach **no-cooperativo**: la CPU toma el control

Sin otras ayudas, el SO se queda colgado si un proceso no quiere hacer una syscall o cometer errores, pues no cede nunca el control al SO de la CPU. ¿Cómo vuelvo a obtener el control (como SO) si el proceso no coopera? Usemos una interrupción por tiempo.

Podemos programar un timer que lance una interrupción cada cierto tiempo. Esto hace un *halt* al proceso que esté corriendo actualmente y el handler de interrupción del SO corre en ese momento, recuperando el control sobre la CPU.

El hardware tiene la responsabilidad de guardar el contexto de ejecución del programa que estaba corriendo previo a la interrupción.

### Context Switch

El **scheduler** se encarga de decidir si seguimos corriendo un proceso o cambiamos a otro. En caso de querer cambiar el proceso, debe ejecutar lo que se conoce como **Context Switch**. Debe guardar el valor de unos registros del proceso que se está corriendo en algún lugar (su Kernel Stack, por ejemplo). De esta manera, al hacer el *return-from-trap*, en vez de continuar la ejecución del proceso anterior, estamos corriendo otro, teniendo el contexto de ejecución del anterior proceso guardado, para poder retomarlo.

Se guardan los registros de propósito general, el kernel stack pointer y el program counter del proceso.