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

#### Approach **cooperativo**: esperar una syscall (Approach cooperative)

Es un approach que tomó Apple en su momento con los SO de las primeras Macintosh. El SO *confía* en que los procesos del sistema van a devolver el control de la CPU al mismo. Se asume que los procesos que corren por mucho tiempo le dan periódicamente el control de la CPU de nuevo al SO para que este decida si corre otra tarea.

Los procesos dan control de la CPU al SO cuando ejecutan **syscalls**, ya sea para abrir archivos, enviar mensajes a otro proceso o para ejecutar un nuevo proceso. Estos procesos suelen tener una instrucción `yield` que lo único que hace es darle el control de la CPU al SO.

Los procesos también dan el control de nuevo al SO cuando hacen algo ilegal, como dividir por cero, o acceder a memoria que no puede acceder, salta el **trap** asociado a esa excepción. Esto da el control nuevamente al SO, que seguramente termine el proceso.

Ahora, ¿qué pasa si un programa termina en un loop infinito, y nunca hace una syscall? ¿Qué puede hacer la CPU?

#### Approach **no-cooperativo**: la CPU toma el control (Approach preemptive)

Sin otras ayudas, el SO se queda colgado si un proceso no quiere hacer una syscall o cometer errores, pues no cede nunca el control al SO de la CPU. ¿Cómo vuelvo a obtener el control (como SO) si el proceso no coopera? Usemos una interrupción por tiempo.

Podemos programar un timer que lance una interrupción cada cierto tiempo. Esto hace un *halt* al proceso que esté corriendo actualmente y el handler de interrupción del SO corre en ese momento, recuperando el control sobre la CPU.

El hardware tiene la responsabilidad de guardar el contexto de ejecución del programa que estaba corriendo previo a la interrupción.

### Context Switch

El **scheduler** se encarga de decidir si seguimos corriendo un proceso o cambiamos a otro. En caso de querer cambiar el proceso, debe ejecutar lo que se conoce como **Context Switch**. Debe guardar el valor de unos registros del proceso que se está corriendo en algún lugar (su Kernel Stack, por ejemplo). De esta manera, al hacer el *return-from-trap*, en vez de continuar la ejecución del proceso anterior, estamos corriendo otro, teniendo el contexto de ejecución del anterior proceso guardado, para poder retomarlo.

Se guardan los registros de propósito general, el kernel stack pointer y el program counter del proceso.

## Política de Scheduling

Es una de las principales huellas de identidad de un SO y es gran parte del esfuerzo para optimizar el rendimiento del mismo. Es tan importante que los SO dan más de una.

Se necesita optimizar:

- **Fairness:** $\longrightarrow$ Cada proceso reciba una dosis de CPU *justa*.
- **Eficiencia:** $\longrightarrow$ Tratar que la CPU esté ocupada todo el tiempo.
- **Carga del sistema:** $\longrightarrow$ Minimizar la cantidad de procesos **READY** que estén esperando a la CPU.
- **Response Time:** $\longrightarrow$ Minimizar el tiempo de respuesta percibido por los usuarios.
- **Latencia:** $\longrightarrow$ Minimizar el tiempo requerido para que un proceso comience a dar resultados.
- **Execution Time:** $\longrightarrow$ Minimizar el tiempo total que le toma a un proceso ejecutar hasta terminarse.
- **Rendimiento:** $\longrightarrow$ Maximizar el número de procesos terminados por unidad de tiempo.
- **Liberación de recursos:** $\longrightarrow$ Hacer que terminen los procesos que tienen reservados más recursos primero.

No se pueden optimizar todos a la vez, pues unos contradicen a otros, como el **fairness** y el **execution time**. Cada política de scheduling va a buscar maximizar una función objetivo, que va a ser una combinación de estas metas evitando dar un gran impacto sobre el resto de políticas.

### Preemptive Scheduling (cont. de [[#Approach **no-cooperativo** la CPU toma el control (Approach preemptive|esto]])

Es el scheduling con desalojo. El scheduler se vale de la interrupción del clock para decidir si seguir ejecutando el proceso actual o cambiar a otro. Como se vale de la interrupción del clock, requiere que la misma exista, y además no da garantías de continuidad en procesos, lo cual puede ser problemático en un SO.

### Scheduling Cooperativo (cont. de [[#Approach **cooperativo** esperar una syscall (Approach cooperative)|esto]])

El scheduler  analiza la situación cuando el kernel toma el control (es decir, cuando ocurre una syscall), especialmente si el proceso hace I/O. A veces se proveen syscalls explícitas para permitir que se ejecuten otros procesos.

---

Los schedulers preemptive combinan ambos enfoques.

## Algunos enfoques

### FIFO, o FCFS (First Came, First Served)

Supone que todos los procesos son iguales, lo que hace que si llega un proceso que requiere mucha CPU, actúa de tapón para los procesos que se encuentran detrás.

Agreguemos entonces prioridades al modelo. Esto trae un nuevo problema, el que los procesos de mayor prioridad demoren infinitamente a los de menor prioridad, los cuales pueden no ejecutarse nunca. Esto se conoce como *starvation*.

Para mitigar esto, podríamos aumentar la prioridad de los procesos a medida que estén hace más tiempo. Concluimos entonces que cualquier esquema con prioridades **fijas** corre riesgo de el problema de *starvation* mencionado.

### RR (Round Robin)

La idea es darle un quantum a cada proceso e ir alternando entre ellos. Este quantum no debe ser *ni muy largo*, pues podría parecer que el SO no responde, *ni muy corto*, pues sino una gran proporción del quantum se la lleva el tiempo de scheduling y el context switch. Esto haría que el sistema esté más tiempo en mantenimiento en lugar de trabajando.

Se suele combinar el modelo Round Robin con prioridades. Estas pueden estar dadas por el tipo de usuario (root-super user-user) o decididas por el mismo proceso, lo cual no suele funcionar. Las prioridades decrecen a medida que los procesos reciben su quantum, para evitar la starvation de los otros procesos.

Los procesos que hacen I/O ganan un crédito extra por ser buenos compañeros de scheduling.

### Múltiples colas

Se tienen colas con 1, 2, 4, 8 quanta cada una. A la hora de elegir un proceso, la prioridad la tiene siempre la cola cono menos quanta. Si a un proceso no le alcanza su cuota de CPU, es pasado a la cola siguiente, disminuyendo su prioridad, pero asignándole más tiempo de CPU en el próximo turno.

Se puede hacer que cuando un proceso termina de hacer I/O, vuelva a la cola de máxima prioridad, pues se supone que va a volver a hacerse interactivo. La idea general es minimizar el tiempo de respuesta para los procesos interactivos, suponiendo que los cómputos largos son menos sensibles a demoras.

### SJF (Shortest Job First)

Está ideada para sistemas donde predominan los trabajos batch. Está orientada a maximizar el throughput. En estos casos se puede predecir la duración del trabajo o al menos clasificarlo. Si conozco las duraciones de antemano, es óptimo, en cuanto a latencia promedio.

Otra alternativa es no pensar en la duración total, sino en cuánto tiempo necesita hasta hacer I/O de nuevo. El problema real es cómo saber cuánta CPU va a necesitar un proceso. Podemos predecir usando información pasada, lo que puede salir mal en procesos con comportamiento irregular.

### Scheduling en SMP

Es un scheduling con un problema importante: el **cache**. Este es de vital importancia para rendimiento de los programas.

Si la política de scheduling hace pasar un proceso a otro procesador, este llega con el cache vacío, tardando más de lo que tardaría si se hubiese ejecutado en el mismo CPU que antes.

Por esto se trata de usar el mismo procesador, aunque se tarde un poco más en obtenerlo. Esto es conocido como *afinidad al procesador*, donde si se respeta a rajatabla es *afinidad dura*, y si es un intento, *afinidad blanda*.

A veces, se distribuye la carga entre todos los procesadores:
- **push migration**
- **pull migration**


## MLFQ (Multilevel Feedback Queue)

Se busca **optimizar el turnaround time**, corriendo trabajos más cortos primero, y hacer al sistema responsive para los usuarios, es decir, **minimizar el response time**.

¿Cómo hacemos que el scheduler funcione bien siendo este algo que no sabe realmente cómo son los trabajos?

### Reglas básicas

Usamos varias colas, cada una con una prioridad distinta {`Q0`,...,`Q8`}. A menor número mayor prioridad. Un trabajo está solo en una cola, y el scheduler prioriza decidir qué correr en un tiempo dado.

Así llegamos a las reglas de MLFQ:

1. Si $Prioridad(A) > Prioridad(B)$ entonces corremos A (pero no B) 
2. Si $Prioridad(A) = Prioridad(B)$ entonces corremos A y B en Round Robin 

La prioridad de los trabajos no es estática, sino que el Scheduler va aprendiendo de los trabajos y acomoda según su uso de CPU y de I/O.

### ¿Cómo cambiamos la prioridad?

Necesitamos ir aprendiendo cómo es nuestra carga de trabajo: una mezcla de trabajos interactivos que corren rápido y que normalmente dependen de la CPU, y trabajos largos que están *CPU bounded*, que requieren un CPU time mayor pero donde el response time no es importante.

Así nace entonces el concepto de **allotment**. Esta es la cantidad de tiempo que un trabajo puede usar en un nivel de prioridad dado antes que el scheduler reduzca su prioridad. Primero asumiremos que el allotment es igual a un solo lapso de tiempo.

Primera versión de las nuevas reglas:

3. Cuando un trabajo entra al sistema, se pone en prioridad más alta.
4. A) Si un trabajo usa mucho de su **allotment** cuando corre, se reduce su prioridad. B) Si un trabajo cede la CPU antes que el allotment termine, se mantiene en el mismo nivel, reseteando el allotment.

Esto trae problemas de *starvation* y se puede llegar a *engañar al scheduler*. Imaginemos un proceso que justo cuando se esté por terminar el allotment, cede la CPU por entrar a un I/O. Se mantiene en la misma prioridad. Esto hace que un trabajo pueda monopolizar la CPU. Veamos otro intento, agregando esta regla:

5. Después de un determinado tiempo $S$, movamos todos los trabajos nuevamente a la cola de más alta prioridad.

Lo que estamos haciendo acá es **boostear** la prioridad de todos los trabajos del sistema. Esto arregla dos problemas a la vez: arregla la starvation, la CPU se comparte entre todos los trabajos en RR, y entonces todos los trabajos van a recibirla.  En segundo lugar, si un trabajo CPU-bound  se vuelve interactivo, el scheduler lo trata como tal, dándole su priority boost.

La constante $S$ no debe ser muy grande, pues sino habrá starvation para los trabajos más largos. Pero tampoco puede ser muy pequeña, pues sino los trabajos interactivos no van a recibir su tiempo de CPU correspondiente.

Falta poder no engañar al scheduler. Para eso vamos a contar mejor el uso de CPU de cada nivel de MLFQ. En vez de olvidarnos cuánto allotment usó un proceso en un nivel dado cuando usa I/O, el scheduler debe seguir manteniendo la cuenta de cuánto allotment usó cuando retorne del I/O. Reescribimos entonces la regla 4 a

4. Cuando un trabajo use todo su allotment time en cualquier nivel, sin importar cuántas veces haya cedido la CPU, su prioridad se reduce.

## Proportional Share Scheduler

Son schedulers que dan ráfagas de CPU a los procesos en base a una *lotería* en la que participan todos los procesos. Cada proceso tiene una proporción de *tickets*, mientras mayor sea la proporción, más posibilidades tiene de recibir ráfagas de CPU.

No es un scheduler determinístico, sino probabilístico. Cada cierto tiempo se sortean los recursos haciendo que el scheduler elija un número aleatorio, y así el proceso que tiene el ticket con ese número es el elegido para correr. 

### Mecanismos de Tickets

Necesitamos un mecanismo para manipular los tickets, hay varias formas que vamos a detallar.

#### Ticket Currency

Un proceso con una cierta cantidad de tickets, puede dar tickets a sus procesos hijos, pero como si fuese otra moneda de cambio con respecto a los tickets que tiene el proceso. Básicamente, subdividimos los tickets y el sistema convierte esa cantidad de tickets en moneda *"extranjera"* a la moneda global.

#### Ticket Transfer

Un proceso puede ceder sus tickets, ya sea una proporción o todos, a otro a modo de cambiar su probabilidad de que sea sorteado, a modo de aumentar su prioridad. Útil en escenarios cliente/servidor.

#### Ticket Inflation

Los procesos pueden aumentar o disminuir su cantidad de tickets según necesiten y calculen su importancia. Esto debe implementarse solo en caso de que no haya procesos greedy, que tomen todos los tickets para si mismos, tomando control total del sistema.

### Implementación

Es una implementación sencilla, solo debemos tener un buen generador de números aleatorios para elegir el ticket ganador, una estructura de datos que guarde los procesos y su cantidad de tickets, y el total de tickets.

Entonces, para tomar decisiones de scheduling, solo tomamos un número aleatorio, vemos que proceso posee ese ticket y lo corremos. Algo asío:

```c
int counter = 0;

// 0 <= winner <= total_tickets - 1
int winner = rng(0, total_tickets);

node_t *current = head;

while (current) {
	counter = counter + current->tickets;
	if (counter > winner) break;
	current = current->next;
}
// schedule current si se provocó el 'break'.
```

### Asignación de tickets

Es un problema que se mantiene abierto. Veamos como hacen algunos schedulers

#### Stride Scheduling

Es un scheduler fair-share deteminístico. Cada trabajo tiene un *stride*, proporcionalmente inverso a la cantidad de tickets que tiene. Cada vez que un proceso corra, incrementamos un contador (llamado *pass*) para este en su stride para trackear su progreso global.

Luego, el scheduler utiliza este *stride* y este *pass* para determinar qué proceso correr luego. La política es correr el proceso con menor pass. Sería algo así:

```c
curr = remove_min(queue);
schedule(curr);
curr->pass += curr->stride;
insert(queue, curr);
```

Este tipo de scheduling trae un problema contra los probabilísticos en que qué ocurre si entra un proceso nuevo al scheduler: ¿le asignamos 0 stride? En ese caso tomaría el control de la CPU. Esto en los lottery schedulers no ocurre, solamente agregamos al total de tickets la cantidad asignada al proceso y listo.

### Linux Completely Fair Scheduler (CFS)

Implementa fair-share pero de manera eficiente y escalable. Trata de no tardar mucho en tomar decisiones de scheduling, a través de su diseño y los usos de estructuras de datos acordes a la tarea.

En vez de basarse en un time slice fijo, busca dividir la CPU de manera equitativa sobre todos los procesos en competencia.  Lo hace usando una técnica de conteo llamada *virtual runtime* (**vruntime**).

Mientras cada proceso corre, se acumula su `vruntime`. El `vruntime` de cada proceso aumenta a la misma tasa, en proporción al tiempo real. Cuando ocurre una decisión de scheduling, elegimos el proceso con menor `vruntime` para correrlo.

Usa Red Black Trees para almacenar los procesos

---

## Ráfagas de I/O y CPU

La ejecución de un proceso consiste en un ciclo entre ejecución de CPU y espera por I/O. La ejecución siempre comienza con una ráfaga de **CPU**. Luego le sigue una ráfaga de I/O, luego CPU, luego I/O, ... hasta terminar.

Un programa **intensivo I/O** tiene muchas ráfagas de CPU cortas. Un programa **intensivo en CPU** tiene pocas ráfagas de CPU largas. 

![[Pasted image 20260406101956.png]]
