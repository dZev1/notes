# Práctica 5

## Ejercicio 1

Un driver es:
- Una pieza de software, pues es el software que comunica el dispositivo con el sistema operativo.
- Es parte del SO, es aquella parte del SO que, a través de su API de drivers, le brinda al usuario la capacidad de usar el dispositivo de forma transparente.

## Ejercicio 2

```c
int driver_write(int *data) {
	OUT(CHRONO_CTRL, CHRONO_RESET);
	return IO_OK;
};

int driver_read(int *data) {
	int timer = IN(CHRONO_CURRENT_TIME);
	copy_to_user((char *)data, (char *)&timer, sizeof(timer));
	return IO_OK; 
}
```

## Ejercicio 3

```c
int driver_read(int *data) {
	while ((IN(BTN_STATUS) & 1) == 0);
	int writ_stat = BTN_PRESSED;
	
	copy_to_user((char *) data, (char *) &writ_stat, sizeof(writ_stat));
	
	int wipe = IN(BTN_STATUS);
	wipe = wipe & ~2;
	OUT(BTN_STATUS, wipe);
	
	return IO_OK;
}
```

El resto de funciones de la API no hace falta implementarlas, solo podemos hacer que retornen `IO_OK`.

## Ejercicio 4

```c
void handle_status(int irq) {
	sema_signal(&key_pressed);
}

int driver_init() {
	semaphore key_pressed;
	sema_init(&key_pressed, 0);
	
	if (request_irq(7, handle_status) == IRQ_ERROR)
		return IO_ERROR;
		
	OUT(BTN_STATUS, BTN_INT);
	
	return IO_OK;
}

int driver_read(int *data) {
	sema_wait(&key_pressed);
	
	int writ_stat = BTN_PRESSED;
	copy_to_user((char *) data, (char *) &writ_stat, sizeof(writ_stat));
	
	int wipe = IN(BTN_STATUS);
	
	wipe = wipe & ~2;
	OUT(BTN_STATUS, wipe);
	
	OUT(BTN_STATUS, BTN_INT);
		
	return IO_OK;
}

int driver_remove() {
	free_irq(7);
	return IO_OK;
}
```