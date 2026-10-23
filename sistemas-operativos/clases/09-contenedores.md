# Virtualización, Contenedores, Cloud y Trusted Execution Environments

## Virtualización

- Posibilidad de que un conjunto de recursos físicos se vean como varias copias de recursos lógicos.
- **Objetivo**
	- Portabilidad
	- Simulación/Testing (la posiblidad de hacer snapshots por ejemplo sirve)
	- Aislamiento
	- Particionar el hardware
	- Agrupar funciones.
	- Protección ante fallas de HW

### Virtual Machine Monitor

- controla máquinas virtuales. Es el *hipervisor*.
- Propiedades:
	- **Fidelidad:** un programa tiene que comportarse igual en el ambiente virtual que como correría en el físico.
	- **Control de recursos:** conserva el control de los recursos físicos de la máquina.
	- **Eficiencia:** una parte muy grande de las instrucciones debe ejecutarse directamente en el procesador.
- 