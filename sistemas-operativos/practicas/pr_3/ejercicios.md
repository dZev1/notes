# Práctica 3

## Ejercicio 1

![[Pasted image 20260909101709.png]]

- En el primer caso, las posibles salidas son `2` y `1`.
- En el segundo, las posibles salidas son cualquier combinación de la secuencia numérica `0123`, con cualquier cantidad de  `a`s metidas en algún lugar después del 0 entre números.

## Ejercicio 2

- Los procesos no cumplen con lo pedido, pues el incrementar o decrementar la variable incluye una lectura de la misma. Por lo tanto, puede haber race condition a la hora de leer la variable compartida en la sección del if. Lo correcto sería hacer lo siguiente.

```c
while (1) {
	mutex.wait();
		if (x <= 5)
			x++
		else
			x--
	mutex.signal();
}
```


## Ejercicio 5

![[Pasted image 20260909103235.png]]

- Esta solución provoca starvation porque solamente estamos haciendo un solo signal, en vez de hacer un signal por cada proceso luego de llegar a que los n procesos hallan llegado a la barrera. 
- También la evaluación del if puede fallar, porque no es atómica, y puede llegar el n-ésimo proceso a pasar por el if con la variable count desactualizada y no hacer el primer signal de la cadena.
- Propongo arreglos:

```go
preparado()

mutex.wait()
count = count + 1

// operamos el if dentro del mutex,
// para asegurarnos de que no haya problemas
// con la lectura de la variable compartida count
if (count == n) 
	barrera.signal()

mutex.signal()

barrera.wait()
barrera.signal() // agregamos el signal para el efecto cadena

critica()
```

## Ejercicio 7

```c
// globales
sem_t mutex = semaphore(1);
int running;

void run_child(pid_t *procs, int N, pid_t current) {
	while (1) {
		mutex.wait();
		if (running == current) {
			mutex.signal();
			break;
		}
	}
	// bla bla lba
	mutex.wait();
	running++;
	if (running >= N) {
		running = 0;
	}
	mutex.signal();
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv) {
	int N = atoi(argv[1]);
	int i = atoi(argv[2]);
	if (argc > 3) {
		exit(EXIT_FAILURE);
	}
	pid_t procs[N];
	int running = i;
	
	for (int j = 0; j < N; j++) {
		pid_t proc_pid = fork();
		if (proc_pid == 0) {
			runChild(procs, j, N);
		} else {
			procs[j] = proc_pid;
		}
	}
	for (int j = 0); j < N; j++ {
		wait(NULL);
	}
	
}
```

**NO!**, así.

```c

void run_child(int id, int N, sem_t *S) {
	S[id].wait();
	
	printf("Ejecutando proceso P%d\n", id);
	
	int id_siguiente = (id_propio + 1) % N;
	S[id].signal();
	
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv) {
	if (argc < 3) exit(EXIT_FAILURE);
	
	int N = atoi(argv[1]);
	int i = atoi(argv[2]);
	// suponemos que inicializarlo aca lo pone en memoria compartida
	sem_t S[N];
	
	for (int j = 0; j < N; j++) {
		if (j == i) {
			S[j] = semaphore(1);
		} else {
			S[j] = semaphore(0);
		} 
	}
	
	for (int j = 0; j < N; j++) {
		if (fork() == 0) {
			run_child(j, N, S);
		}
	}
	
	for (int j = 00; j < N; j++) {
		wait(NULL);
	}
	
	return EXIT_SUCCESS;
}
```

## Ejercicio 8

### 8.1

Se tienen tres procesos (A, B y C). Se desea que el orden en que se ejecutan sea el orden alfabético, es decir que las secuencias normales deben ser: ABC, ABC, ABC, ...

```c
sem_t semA = semaphore(1);
sem_t semB = semaphore(0);
sem_t semC = semaphore(0);

void procA() {
	for(;;) {
		semA.wait();
		printf("Ejecutando proceso A.\n");
		semB.signal();
	}
}


void procB() {
	for(;;) {
		semB.wait();
		printf("Ejecutando proceso B.\n");
		semC.signal();
	}
}

void procC() {
	for(;;) {
		semC.wait();
		printf("Ejecutando proceso C.\n");
		semA.signal();
	}
}
```

 Evita la inanición, pues al iniciar el semaforo de A en 1, este no se bloquea cuando hace el primer `wait()`. Luego de esto, avisamos al semáforo de B que puede correr B. Y así en un ciclo infinito de A->B->C->A.

### 8.2

Idem anterior, pero se desea que la secuencia normal sea: BBCA, BBCA, BBCA, ...

```c
sem_t semA = semaphore(0);
sem_t semB = semaphore(1);
sem_t semC = semaphore(0);

int ejecuciones_b = 0;

void procA() {
	for(;;) {
		semA.wait();
		printf("Ejecutando proceso A.\n");
		semB.signal();
	}
}


void procB() {
	for(;;) {
		semB.wait();
		printf("Ejecutando proceso B.\n");
		ejecuciones_b++;
		
		if (ejecuciones_b == 2) {
			ejecuciones_b = 0;
			semC.signal();
		} else {
			semB.signal();
		}
	}
}

void procC() {
	for(;;) {
		semC.wait();
		printf("Ejecutando proceso C.\n");
		semA.signal();
	}
}
```

Misma explicación que el anterior para la inanición, ejecutamos B dos veces, automandándose signal, luego con la variable de veces ejecutado podemos hacer que una vez que llega a ejecutarse dos veces, podemos enviar signal al semáforo de C y asi sigue el ciclo.

### 8.3

Se tienen un productor (A) y dos consumidores (B y C) que actúan no determinísticamente. La información provista por el productor debe ser retirada siempre 2 veces, es decir que las secuencias normales son: ABB, ABC, ACB o ACC. Nota: ¡Ojo con la exclusión mutua!

```C
sem_t semProductor = semaphore(1);
sem_t semConsumidor = semaphore(0);
sem_t mutex = semaphore(1);

int consumos = 0;

void productor() {
	for(;;) {
		semProductor.wait();
		
		printf("[PRODUCTOR] Produciendo algo\n");
		
		semConsumidor.signal();
		semConsumidor.signal();
		
	}
}

void consummidor() {
	for (;;) {
		semConsumidor.wait();
		
		mutex.wait();
		
		printf("[CONSUMIDOR] Consumiendo algo del productor\n");
		consumos++;
		
		if (consumos == 2) {
			consumos = 0;
			semProductor.signal();
		}
		
		mutex.signal();
	}
}
```

No hay inanición porque el productor comienza produciendo lo suyo, y envía las dos señales correspondientes al semáforo de los consumidores. Luego, tenemos la variable compartida consumos que nos lleva una cuenta de cuántas veces se consumió lo producido en A, sin importar quienes fueren sus consumidores. Una vez que consumimos 2 veces lo producido, señalizamos al productor que produzca de nuevo lo suyo. Manejamos esta variable compartida en una sección crítica, a través de un Mutex, en pos de que no haya race conditions.
### 8.4

Se tienen un productor (A) y dos consumidores (B y C). Cuando C retira la información, la retira dos veces. Los receptores actúan en forma alternada. Secuencia normal: ABB, AC, ABB, AC, ABB, AC...

```c
sem_t semProductor = semaphore(1);
sem_t semConsumidor = semaphore(0);
sem_t semB = semaphore(0);
sem_t mutex = semaphore(1);

void productor() {
	for(;;) {
		semProductor.wait();
		
		printf("[PRODUCTOR] Produciendo algo\n");
		
		semConsumidor.signal();
	}
}

void consumidorC() {
	for (;;) {
		semConsumidor.wait();
		
		mutex.wait();
		
		for (int i = 0; i < 2; i++) {
			printf("[CONSUMIDOR] Consumiendo algo\n");
		}
		
		mutex.signal();
		semProductor.signal();
	}
}

void consumidorB() {
	for(;;) {
		semConsumidor.wait();
		semB.signal();
		for(;;) {
			semB.wait();
			mutex.wait();
		
			if (consumos == 2) {
				semProductor.signal();
				consumos = 0;
				break;
			}
			mutex.signal();
			semB.signal();
		}
	}
}
```
 
 >[!WARNING] No respeta el orden ABB, AC, ABB, AC, ...

Corrijo:

```c
sem_t semA = semaphore(1);
sem_t semB = semaphore(0);
sem_t semC = semaphore(0);

bool turnoB = true;
int consumosB = 0;

void A() {
	for (;;) {
		semA.wait();
		
		printf("[PRODUCTOR A] Produciendo cositas\n");
		
		if (turnoB) {
			semB.signal();
			semB.signal();
			turnoB = false;
		} else {
			semC.signal();
			turnoB = true;
		}
	}
}

void B() {
	for (;;) {
		semB.wait();
		
		printf("[CONSUMIDOR B] Consumiendo cositas\n");
		consumosB++;
		
		if (consumosB == 2) {
			consumosB = 0;
			semA.signal();
		}
	}
}

void C() {
	for (;;) {
		semC.wait();
		
		printf("[CONSUMIDOR C] consumiendo 2 cositas juntas\n");
		
		semA.signal();
	}
}
```

## Ejercicio 10

![[Pasted image 20260914102020.png]]

### 10.A

Si hay trazas que llevan a Deadlocks, por ejemplo:

| foo          | bar          | S   | R   |
| ------------ | ------------ | --- | --- |
| `semWait(S)` |              | 0   | 1   |
|              | `semWait(R)` | 0   | 0   |
|              | `semWait(S)` | 0   | 0   |
| `semWait(R)` |              | 0   | 0   |
| **DEADLOCK** | **DEADLOCK** | 0   | 0   |

### 10.B

Si puede haber inanición, supongmos la siguiente traza, donde cuando le toca ejecutar a bar, este se encuentra bloqueado por el semWait:

| foo            | bar          | S   | R   |
| -------------- | ------------ | --- | --- |
| `semWait(S)`   |              | 0   | 1   |
| `semWait(R)`   |              | 0   | 0   |
|                | `semWait(R)` | 0   | 0   |
| `x++`          |              | 0   | 0   |
| `semSignal(S)` |              | 1   | 0   |
| `semSignal(R)` |              | 1   | 1   |
| `semWait(S)`   |              | 0   | 1   |
| `semWait(R)`   |              | 0   | 0   |
| `x++`          |              | 0   | 0   |
| `semSignal(S)` |              | 1   | 0   |
| `semSignal(R)` |              | 1   | 1   |
| ...            |              |     |     |

## Ejercicio 11

```c
sem_t semOccupied = semaphore(0);
sem_t semFree = semaphore(N);
sem_t mutex = semaphore(1);

queue_t buffer(N);

void write(mensaje_t m) {
	semWait(&semFree);
	
	mutex.wait();
	push(&buffer, m);
	mutex.signal();
	
	semSignal(&semOccupied);
}

mensaje_t read() {
	semWait(semOccupied);
	mensaje_t m;
	
	mutex.wait();
	pop(&buffer, &m);
	mutex.signal;
	
	semSignal(&semFree);
	return m;
}
```

## Ejercicio 12

```c
// memoria compartida
barreraExp = semaphore(1);
barreraImp = semaphore(0);
mutex = semaphore(1);
cant = 0;

estudiante() {
	for(;;) {
		implementarTP();
		
		mutex.wait();
		
		cant++;
		if (cant == N) {
			barreraExp.wait();
			barreraImp.signal();
		}
		
		mutex.signal();
		
		barreraImp.wait();
		barreraImp.signal();
		
		experimentar();
		
		mutex.wait();
		
		cant--;
		if (cant == 0) {
			barreraImp.wait();
			barreraExp.signal();
		}
		
		mutex.signal();
		
		barreraExp.wait();
		barreraExp.signal();
}
```

## Ejercicio 13

```c
mutexRopa = semaphore(1);
semRopa = semaphore(1);
lavarropaListo = semaphore(0);
int prendasDentro;

void ropa() {
	semWait(&semRopa);
	
	semWait(&mutexRopa);
	entroAlLavarropas();
	prendasDentro++;
	if (prendasDentro == 10)
		semSignal(&lavarropaListo);
	semSignal(&mutexRopa);
	
	semWait(&semRopa);
	semSignal(&semRopa);
	
	saquenmeDeAqui();
}

void lavarropa() {
	for(;;) {
		semWait(&lavarropasListo);
		lavar();
		puedenDescargarme();
		semSignal(&semRopa);
	}
}
```

>[!WARNING]
>Ta mal. Ahora lo arreglamos.

```c
sem_t semCarga = sem(0);
sem_t semLleno = sem(0);
sem_t semDescarga = sem(0);
sem_t semVacio = sem(0);
sem_t mutex = sem(1);
int prendas = 0;

lavarropa() {
	for(;;) {
		estoyListo;
		
		for (int i = 0; i < 10; i++) {
			semSignal(&semCarga);
		}
		
		semWait(&semLleno);
		
		lavar();
		puedenDescargarme();
		
		for (int i = 0; i < 10; i++) {
			semSignal(&semDescarga);
		}
		
		semWait(&semVacio);
	}
}

ropa() {
	semWait(&semCarga);
	
	semWait(&mutex);
	entroAlLavarropas();
	prendas++
	if (prendas == 10)
		semSignal(&semLleno);
	semSignal(&mutex);
	
	semWait(&semDescarga);
	
	saquenmeDeAqui();
	
	semWait(&mutex);
	prendas--;
	if (prendas == 0) {
		semSignal(&semVacio);
	}
	semSignal(&mutex);
}
```