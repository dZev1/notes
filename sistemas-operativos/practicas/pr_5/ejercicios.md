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

## Ejercicio 7

Antes de escribir un sector, el driver tiene que asegurarse:
- Motor encendido
- Si no lo está, encenderlo y esperar 50ms antes de hacer cualquier operación.
- Una vez que termine una operación, apagar el motor. Tarda 200ms, no puede atender operaciones antes de ese tiempo.

### 7.A

```c
int driver_write(int sector, void *data) {
	if (sector < 0) {
		return IO_ERROR;
	}
	
	int motor_status = IN(DOR_STATUS);
	if (motor_status == 0) {
		OUT(DOR_IO, 1);
		sleep(50);
	}
	
	int pista = sector / cantidad_de_sectores_por_pista();
	int int_sector = sector % cantidad_sectores_por_pista();
	
	OUT(ARM, pista);
	while (IN(ARM_STATUS) == 0);
	
	OUT(SEEK_SECTOR, sector_interno);
	
	escribir_datos(data);
	while (IN(DATA_READY) == 0);
	
	OUT(DOR_IO, 0);
	sleep(200);
	
	return IO_OK;
}
```

### 7.B

```c
semaphore timer;
semaphore arm;
semaphore data_sem;
void handler_irq6(int irq) {
	if (IN(ARM_STATUS) == 1) {
		sema_signal(&arm);
	}
	if (IN(DATA_READY) == 1) {
		sema_signal(&data);
	}
}

void handler_irq7(int irq) {
	sema_signal(&timer);
}

int driver_init() {
	sema_init(&timer, 0);
	sema_init(&arm, 0);
	sema_init(&data_sem, 0);
	
	if (request_irq(6, handler_irq6) == IRQ_ERROR)
		return IO_ERROR;
	if (request_irq(7, handler_irq7) == IRQ_ERROR)
		return IO_ERROR;
	return IO_OK;
}

int driver_write(int sector, void *data) {
	if (sector < 0) {
		return IO_ERROR;
	}
	
	int motor_status = IN(DOR_STATUS);
	if (motor_status == 0) {
		OUT(DOR_IO, 1);
		sema_wait(&timer);
	}
	
	int pista = sector / cantidad_de_sectores_por_pista();
	int int_sector = sector % cantidad_de_sectores_por_pista();
	
	OUT(ARM, pista);
	sema_wait(&arm);
	
	OUT(SEEK_SECTOR, int_sector);
	
	escribir_datos(data);
	sema_wait(&data_sem);
	
	OUT(DOR_IO, 0);
	sema_wait(&timer);
	sema_wait(&timer);
	sema_wait(&timer);
	sema_wait(&timer);
	
	return IO_OK;
}
```