# Sincronización de Procesos

## Introducción

Vamos a buscar que los procesos puedan cooperar sin estorbarse. La contención y la concurrencia son dos problemas fundamentales con la creciente ola de la programación distribuida y paralela.

Este problema también aparece en los SO. Tenemos que manejar la contención y la concurrencia a fin de lograr hacerlo correctamente y con buen rendimiento.

## Ejecuciones paralelas de un mismo proceso

Toda ejecución debería dar un resultado equivalente a alguna ejecución secuencial de los mismos procesos. Si esto no ocurre porque un proceso pisa las variables de memoria del otro o viceversa, estamos frente a *race conditions*, pues el resultado que se obtiene varía dependiendo de en qué momento se ejecuten las cosas (o en qué orden se ejecutan).

### ¿Cómo solucionamos las race conditions?

Una forma es lograr una exclusión mutua usando **secciones críticas** (o `CRIT`). Podemos pensar a `CRIT` como un cartel de *no molestar* en la puerta.

Una sección crítica es un cacho de código tal que sólo hay un proceso a la vez en `Crit`, Todo proceso que esté esperando entrar a `CRIT` va a entrar y ningún proceso fuera de `CRIT` puede bloquear a otro.

A nivel de implementación, se haría con dos llamados: uno para entrar a `CRIT`, otro para salir de `CRIT`. Si las logramos implementar correctamente, contamos con herramientas para que los procesos puedan compartir datos sin estorbarse.

## Implementación de secciones críticas

Una alternativa es suspender todo tipo de interrupciones una vez que estemos dentro de la sección crítica. Esto borra temporalmente la multiprogramación. Aunque garantiza la correcta actualización de los datos compartidos, trae muchos problemas.

Otra alternativa es "revolear booleanos", de la pinta

```pseint
entrar():
	reintentar:
		if (!CRIT)
			CRIT = true;
		else
			sleep(1);
			goto reintentar;

salir():
	seccion_critica = false;
```

Si hacemos un sleep muy largo, capaz desperdiciamos tiempo en el que ya estaba lista la sección crítica. Además, sigue ocasionando problemas de race conditions. Irónicamente hay race conditions en la solución a la race conditions... cambiemos a otro.

La solución más general requiere un poco de ayuda del hardware. Esta solución se llama **Test And Set** (TAS). El HW provee una instrucción que permite establecer atómicamente el valor de una variable entera en 1. La idea es que pone 1 y devuelve el valor anterior, pero de manera atómica. Esto significa que es una instrucción indivisible, aunque tengamos varias CPUs. Funcionaría algo así (pero en ASM, en una sola instrucción ininterrumpida):

```c
bool TestAndSet(bool *destino) {
	bool resultado = *destino;
	*destino = TRUE;
	return resultado;
}
```

Entonces, veamos como usarlo para hacer locks:

```c
bool lock;

int main(void) {
	while (true) {
		while (TestAndSet(&lock)){
			// Estoy esperando a que se libere la sección critica
			// TestAndSet está devolviendo 'true'
		};
		
		
		// Ahora estoy en la sección crítica.
		// Ejecuto el código crítico.
		
		// Salgo de la sección crítica. 
		lock = false;
		
		// Si hay algo no crítico, lo hago aca.
	}
}
```

Como el while del TAS está vacío, el código intenta obtener un lock constantemente, consumiendo muchísima CPU en el intento. A este tipo de whiles vacíos se los llama **Busy Waiting** y hay que ser cuidadosos a la hora de usarlos, pues es una forma agresiva y costosa de intentar obtener recursos. Perjudica otros procesos sin razón en muchos casos, pues podemos verlo como que esos procesos no reciben muchos mensajes el día del amigo, o sea, se enojan.

Para solucionar esto, podemos poner un `sleep()`, pero lo dicho antes, si es mucho tiempo de sleep, perdemos tiempo, si es poco seguimos consumiendo CPU de más.

Otra solución, la propuso Dijkstra. Plantea roles de **productor y consumidor**. Ambos roles comparten un buffer de tamaño limitado, más algunos índices para saber dónde se colocó el último elemento, si hay alguno, etcétera. Esto es un *buffer acotado*.

El productor pone elementos en el buffer, y el consumidor los saca. Esto trae nuevamente problemas de concurrencia, pues tanto el productor como el consumidor quieren actualizar las mismas variables.

Además de esto, tenemos un problema adicional. El buffer es acotado, entonces qué pasa si el buffer está lleno y el productor quiere agregar un item más, o qué pasa si el consumidor quiere sacar algo y el buffer está vacío. La solución es simple, debe esperar. ¿Cuánto? Podemos hacer busy waiting, pero podemos perder tiempo. Podemos usar también `sleep()`-`wakeup()`.

Del lado del consumidor podemos pensar algo de este tipo:

`if (cant == 0) sleep();`

Y el productor haría algo así:

```C
item_t buffer[255] = {0};
cant = 0;
void agregar(item_t item, item_t* buffer)
void productor() {
	agregar(item, buffer);
	cant++;
	wakeup();
}
```

Esto no soluciona nada. Se agrega un problema, pues se pierden los `wakeup()`, lo que provoca trabas en el sistema. A esto se lo conoce como *lost wakeup problem*.

Vamos a complejizar esto, usando **semáforos**.

## Semáforos

Un semáforo es una variable entera que cumple:
- Puede ser inicializada en cualquier valor.
- Solo se puede manipular con dos operaciones:
	- `wait()` (o `P()` o `down()`)
	- `signal()` (o `V()` o `up()`)
- `wait(s)`: `while (s <= 0) sleep(); s--;`
- `signal(s)`: `s++; if (x espera por s) wakeup(x)`
- Ambas se implementan de manera tal que se ejecuten sin interrupciones.
- Semáforo especial que tiene dominio binario: `mutex`, mutual exclusion.

### Deadlocks

Son situaciones en las que un proceso espera algo que solo otro proceso puede producir y este otro proceso está esperando algo que lo puede producir del primer proceso:

```
A --necesita--> B ## A bloqueado
B --necesita--> A ## B Bloqueado
```

Es uno de los problemas más difíciles de la concurrencia.

## Actualidad

Los lenguajes de alto nivel proveen distintas alternativas para implementar secciones críticas:
- `bool` atómico
- `int` atómico
- `cola` atómica

### TASLock

Mutex basado en bool atómico con `testAndSet()`, también llamado *Spin Lock*.

```c++
atomic<bool> reg;

void create() { reg.set(false); }

void lock() { while (reg.testAndSet()) {} }

void unlock() { reg.set(false); }
```

#### Inconvenientes
- `lock()` no es atómico.
- Se hace busy waiting, aunque su overhead es menor que usar semáforos.
- Para minimizar el impacto, se puede testear antes de hacer testAndSet (TTASLock).

```C++
void create() { mtx.set(false); }
void lock() {
	while (true) {
		while (mtx.get()) {}
		if (!mtx.testAndSet()) return;
	}
}
void unlock() { mtx.set(false); }
```

Esto se conoce también como *local spinning*. Es más eficiente, pues hace `get()` en vez de `testAndSet()`, se lee directamente la memoria cache mientras sea verdadero. Cuando un proceso hace `unlock()` se produce un cache miss.

## Problema: garantizar exclusión mutua

Si la sección crítica es toda la función, entonces va a haber menos concurrencia.
Si la sección crítica es un bloque, entonces hay mayor concurrencia.

### Condiciones de Coffman

Estas son condiciones que son necesarias, según Coffman, para que se produzca un deadlock:
- **Exclusión mutua**: Un recurso no puede estar asignado a más de un proceso. Si esto ocurre, no hay deadlock.
- **Hold and Wait**: Los procesos que ya tienen algún recurso pueden solicitar otro.
- **No Preemption**: Si no hay mecanismo compulsivo para quitar recursos a un proceso.
- **Espera Circular**: Tiene que haber un ciclo de N $\geq$ 2 procesos, tal que $P_i$ espera un recurso que tiene $P_{i+1}$.
Tiene algunas falencias el postulado.

## Problemas de sincronización

### Problemas

- Race condition
- Deadlock
- Starvation

### Prevención

- Patrones de diseño
- Reglas de programación
- Prioridades
- Protocolos

### Detección

- Análisis de programas
	- Estático: sin correr el programa, sobre el código del programa
	- Dinámico: corriendo el programa. No solo el código, sino también los monitores.