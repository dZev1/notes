# Memoria

## API de memoria de la libc

- Memoria de stack
	- Manera implícita.
	- Manejada por el compilador.
	- Se libera automáticamente
	- Tiene tamaño límite.
- Memoria de heap.
	- Manera explícita
	- Memoria dinámica
	- La maneja el usuario.
		- `malloc()` para reservar.
			- Por debajo, usa la syscall `brk()`.
		- `free()` para liberar
	- Hay que liberarla.
