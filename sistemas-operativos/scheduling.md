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