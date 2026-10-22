# Sincronización entre procesos

- Contención y concurrencia = pilares de la programación distribuida y paralela.
- También a nivel SO, se tienen que manejar estas propiedades *correctamente* y con *buen rendimiento*.
- Toda ejecución debería dar un resultado equivalente a alguna ejecución secuencial de los mismos procesos.
- Hay problemas de **condición de carrera**.

## Secciones críticas

- Sectores de exclusión mutua.
	- Es un fragmento de código en el que corre solo un proceso a la vez.
	- Todo proceso esperando a entrar a esa sección crítica, tiene que ser asegurado de que va a ingresar.
	- Ningún proceso fuera de la misma puede bloquear a otro.
	- **Sólo para datos compartidos**
	- Se implementaría con dos llamados: uno para entrar y otro para salir.
	- Permiten que varios procesos puedan compartir datos sin molestarse.
- Formas:
	- **Suspender interrupciones**
		- Elimina la multiprogramación, aunque garantiza la actualización de los datos compartidos.
		- Baja performance.
	- **Locks**
		- Variables booleanas compartidas.
		- Si entras a la sección se pone en 1, cuando salís en 0.
		- El problema es que pasa si antes del store del acceso a una sección crítica de un proceso, se cambia a otro proceso que también quiere entrar, están dos procesos a la vez.
	- **Ayuda del Hardware!!**
		- Instrucciones atómicas, es decir, no se pueden dividir en la mitad de su ejecución, ni con más CPUs.
		- Se llama **TestAndSet**.
		- Pone el valor en 1 y devuelve el valor anterior. De manera atómica.
		- Todo en una sola instrucción.

```c
bool lock;

int main() {
	while (TestAndSet(&lock)); // Si no hay nadie en la sección crítica, TAS da 0, sino da 1.
	
	/*
	 * En sección crítica, hago cualquier cosa
	 *
	 */
	 
	 lock = 0; // Salgo de sección crítica
}
```

- Esto consume mucha CPU, por hacer **busy waiting**.
- Podríamos poner un `sleep()`, pero no sabemos por cuánto tiempo.
	- Si es mucho, se pierde tiempo.
	- Si es poco, se desperdicia CPU, aunque menos que antes.
- Podemos hacer que el SO le avise al proceso que la sección crítica está disponible.

## Semáforos

- Variable entera con características:
	- Se puede inicializar en cualquier valor.
	- Solo se puede manipular cos dos operaciones:
		- `wait()` (o `P()` o `down()`).
		- `signal()` (o `V()` o `up()`).
	- `wait(s)` : `while (s <= 0) dormir(); s--;` atómico.
	- `signal(s)` : `s++; if (alguien espera s) despierto alguno;` atómico.
- Un tipo especial de semáforo con dominio binario se llama **mutex**, por *mutual exclusion*.
- Pueden fallar y llegar a **deadlocks**, si están mal usados.

- Hoy hay otros mecanismos para implementar secciones críticas.
	- Tipos de datos atómicos (bools, ints, ...).
		- Traen TAS con objetos (`getAndSet(), testAndSet()`).
		- Podemos implementar **Spin Locks**, a partir de bools atómicos con testAndSet().
			- Hace busy waiting pero no es tan costoso.