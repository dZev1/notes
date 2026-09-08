# Scheduling

- Solo un proceso corre a la vez (1 core) (ACA)
- Usar procesador = ráfaga de CPU y esperar un dispositivo/archivo/.... = ráfaga de I/O.
- ráfagas de CPU cortas y de I/O largas = Proceso intensivo en I/O

- **Scheduler**
	- *Preemptive*
		- La CPU puede desalojar el proceso que obtiene la CPU.
		- RR.
	- *Nonpreemptive*
		- Una vez iniciado el proceso, no puede desalojarse hasta que libere la CPU de forma voluntaria.
		- FCFS.
- Queremos maximizar
	- Uso de CPU
	- Throughput
- Queremos minimizar
	- Turnaround $\longrightarrow$ lo que tarda un proceso en terminar
	- Waiting time
	- Response time

- Criterios para decidir cuál algoritmo es mejor
	- Avg. Turnaround time
	- Avg. Waiting time

## Algoritmos

### **FCFS**
- Nonpreemptive.
- Ejecuta los procesos en orden de llegada.
- No sirve para programas interactivos.
	- Mucho I/O

### **RR**
- Preemptive
- Da quantums de CPU de manera equitativa a cada proceso.
- Si el quantum dura mucho $\longrightarrow$ parecido a FCFS
- Si el quantum es muy corto $\longrightarrow$ Performance completamente tomada por el Context Switch

### **SJF**
- Nonpreemptive
- Se eligen los procesos más cortos entre los encolados para correr a continuación.

### **SRTF**
- Version preemptive de SJF 
- El scheduler desaloja al proceso y le da la CPU al nuevo.
- Compara el tiempo restante del proceso con los demás procesos.
	- Si falta más que el nuevo proceso, lo desaloja y corre el nuevo.

### **Prioridades**
- Asociamos prioridades por procesos.
- Si hay varios con la misma prioridad, ejecutamos FCFS.
- Puede ser preemptive o nonpreemptive.
- Mayor prioridad = 0
- Posibilidad de **starvation** si siguen llegando procesos de menor prioridad.
- Para evitar esto:
	- *Aging*: incrementamos gradualmente la prioridad de los procesos que están esperando hace mucho.

### **Multilevel Queue**
- Colas separadas por prioridad.
- Se ejecutan los de la cola de mayor prioridad.
- Prioridad estática.
- RR dentro de la misma cola.

### **Multilevel Feedback Queue (MLFQ)**
- Permitimos a los procesos cambiamos de cola.
- Esto lo hacemos agregando aging a la multilevel queue.

## Real time scheduling
- Dos tipos de sistemas RT
	- Soft RT
	- Hard RT
		- Usan deadlines en los que se tiene que ejecutar un proceso
