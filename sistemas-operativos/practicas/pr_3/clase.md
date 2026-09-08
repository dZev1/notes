# Sincronización

## Ejercicio 1

### Mem Compartida

```c++
sem(prod);
sem(sC1);
sem(sC2);
```

### Productor

```c
for(;;) {
	prod.wait();
	producir();
	producir();
	sC1.signal();
}
```

### C1

```c++
for(;;) {
	sC1.wait();
	consumir1();
	sC2.signal();
}
```

### C2

```c++
for(;;) {
	sC2.wait();
	consumir2();
	wait.signal();
}
```

## Ejercicio 2

### Mem compartida

```c
sem(s);
```

### A

```c
for(;;) {
	A1();
	s.signal();
	A2();
}
```

### B

```c
for(;;) {
	B1();
	s.wait();
	B2();
}
```