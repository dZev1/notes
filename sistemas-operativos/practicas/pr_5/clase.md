# Drivers

## Ejercicio 1

- Registro de 32 bits `LOC_TARGET`
- Cte `START` en reg `LOC_CTRL` -> comienza operación y coloca `BUSY` en reg `LOC_STATUS`.
- Al encontrar la caja la deposita en la bandeja de salida y coloca `JOYA` en `LOC_CTRL` y `READY` en `LOC_STATUS.`
- Si no la puede encontrar pone `BAJON` en `LOC_CTRL` y `READY` en `LOC_STATUS`.

 ESCRIBIR `int driver_write(int *data)` del driver.

```c
int driver_write(int *data) {
	int codigo;
	copy_from_user(&codigo, data, sizeof(int));
	
	semWait(&mu);
	OUT(LOC_TARGET, codigo);
	OUT(LOC_CTRL, START);
	
	while (IN(LOC_CTRL) == START);
	
	int resultado = IN(LOC_CTRL);
	semSignal(&mu);
	
	if (resultado == JOYA)
		return 1;
	else if (resultado == BAJON)
		return 0;
	else
		return -1;
}
```

- `driver_init()` inicializa el semáforo que usa el código anterior.
- Estamos usando Polling, poco efectivo, si el hardware soporta INTS.
- Solución con interrupción para handlear el waiting.

```c
// Hacemos un Handler
void handler() {
	if (esperando && IN(LOC_START) == READY) {
		semSignal(&sem)
	}
}

// cambiamos el driver_init
int driver_init() {
	sem_t mu = semaphore(1);
	sem_t sesm = semaphore(1);
	atomic bool esperando = false;
	request_irq(CHINBOT_INT, handler);
}

int driver_write(int *data) {
	int codigo;
	copy_from_user(&codigo, data, sizeof(int));
	
	semWait(&mu);
	OUT(LOC_TARGET, codigo);
	OUT(LOC_CTRL, START);
	
	semWait(&sem);
	
	int resultado = IN(LOC_CTRL);
	semSignal(&mu);
	
	if (resultado == JOYA)
		return 1;
	else if (resultado == BAJON)
		return 0;
	else
		return -1;
}
```

## Ejercicio 2

- 3 drivers
	- Lector de código de barra
	- Brazo mecánico
	- Sistema de movimiento

- `struct paquete { int x ; int y ; int codigo }`
- 