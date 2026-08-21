# IPC

## Funciones

- `int pipe(int fd[2])`
	- Crea un pipe unidireccional, el cual tiene extremo de lectura (salida), y de escritura (entrada).
	- Genera dos descriptores, uno para cada extremo.
- `int dup2(int oldfd, int newfd)`
	- Elimina la referencia al objeto apuntado por `newfd`, y apunta `newfd` al objeto que apunta `oldfd`.
	- Reemplaza el file descriptor.
- `int open(char *path, int flags, ...)`: Abre el archivo indicado, retornando un descriptor que apunta a dicho archivo.
- `int close(int d)`: Cierra para el proceso actual el descriptor pasado por parámetro.
- `int read(int d, void* b, size_t s)`: lee ``s`` bytes del descriptor y los escribe en el buffer. **es bloqueante**
- `int write(int d, void* b, size_t s)`: lee ``s`` bytes del buffer y los escribe en el archivo al que apunta el descriptor.

## Ejercicio 1

```c
// enums para ayuda
enum { READ, WRITE };
enum { LESTER, ELIZA };

// vars globales
int pipe_lester[2];
int pipe_eliza[2];

void lester() {
	close(pipe_lester[READ]);
	close(pipe_eliza[READ]);
	close(pipe_eliza[WRITE]);
	
	int dado = tirar_dado();
	write(pipe_lester[WRITE], &dado_lester, sizeof(dado));
	
	exit(EXIT_SUCCESS);
}

void eliza() {
	close(pipe_eliza[READ]);
	close(pipe_lester[READ]);
	close(pipe_lester[WRITE]);
	
	int dado = tirar_dado();
	write(pipe_eliza[WRITE], &dado_lester, sizeof(dado));
	
	exit(EXIT_SUCCESS);
}

int main(void) {
	
	// creamos pipes
	pipe(pipe_lester);
	pipe(pipe_eliza);
	
	// creo a lester y lo ejecuto
	pid_t pid_lester = fork();
	
	if (lester == 0) {
		lester();
	}
	
	// creo a eliza y la ejecuto
	pid_t pid_eliza = fork();
	
	if (pid_eliza == 0) {
		eliza();
	}
	
	// cierro pipes para padre
	close(pipe_lester[WRITE]);
	close(pipe_eliza[WRITE]);
	
	// leo resultados
	int resultado_lester;
	read(pipe_lester[READ], &resultado_lester, sizeof(resultado_lester));
	
	int resultado_eliza;
	read(pipe_eliza[READ], &resultado_eliza, sizeof(resultado_eliza));
    
	if (resultado_eliza > resultado_lester)
		printf("gano Eliza con %d!\n", resultado_eliza);
	else if (resultado_lester > resultado_eliza)
		printf("gano Lester con %d!\n", resultado_lester);
	else
		printf("Empate\n");
    
	return 0;
}
```

## Ejercicio 2

>[!Important] !!
>- Cerra los pipes.
>- Usar ``dup2`` para cambiar el fd de escritura al stdout y de lectura de wc para los extremos correspondientes del pipe.

```c
enum { READ, WRITE };

int pipe_fd[2];

void ls() {
	close(pipe_fd[READ]);
	
	dup2(pipe_fd[WRITE], STDOUT_FILENO);
	
	execXXXXXXXX("ls", "-al");
}

void wc() {
	close(pipe_fd[WRITE]);
	
	dup2(pipe_fd[READ], STDIN_FILENO);
	
	execXXXXX("wc", "-l");
}

int main() {
	pipe(pipe_fd);
	
	if (fork() == 0) {
		ls();
	}
	if (fork() == 0) {
		wc();
	}
	
	close(pipe_fd[WRITE]);
	wait(NULL);
	wait(NULL);
}
```

## Sockets

- Pueden trabajar en != maquina.
- Extremo de una conexión, con una **dirección IP** y un **puerto**.
- `socket()` para crear el socket.
- `listen()` para recibir conexiones entrantes.
- `accept()` para aceptar la conexion.