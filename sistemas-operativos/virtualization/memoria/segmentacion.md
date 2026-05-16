# Segmentación

Usar **base & bounds** gasta espacio de memoria física sin hacer nada, entre el heap y el stack del proceso. Entonces acá nos surge la pregunta de cómo podemos hacer para manejar un address space enorme, con potencialmente mucho espacio libre entre el heap y el stack de cada proceso.

Para resolver esto, surgió la **Segmentación**. En vez de tener un solo par de registros **base-bounds** en la MMU, tengamos un par por cada segmento **lógico** del address space. Un **segmento** es una porción contigua del address space de una longitud particular. En nuestro address space canónico, tenemos tres segmentos distinguidos: el *stack*, el *heap* y el *código* del programa.

Lo que permite la segmentación al SO es poner estos segmentos en distintas partes de la memoria física, y así evitar llenar la memoria física con espacio virtual sin usar.

El hardware de nuestra MMU es sencillo, tres pares de base and bounds, uno para cada segmento. Por ejemplo

| Segment | Base | Size |
| ------- | ---- | ---- |
| Code    | 32K  | 2K   |
| Heap    | 34K  | 3K   |
| Stack   | 28K  | 2K   |

Según los datos de la tabla:
- el *segmento de código* está ubicado en la dirección física **32KB**, y tiene un tamaño de **2KB**
- el *segmento del heap* está ubicado en la dirección física **34KB** y tiene un tamaño de **3KB**
- el *segmento del stack* está ubicado en la dirección física **28KB** y tiene tamaño de **2KB**.

El campo Size es exactamente el bounds register de antes. Le dice cuántos bytes del segmento son válidos para acceder al hardware.

El hardware es quién puede determinar si estamos haciendo accesos ilegales a memoria o no. En caso de accesos ilegales, el hardware detecta el out of bounds, salta una trap del procesador, y el SO atiende esta trap en la mayor parte de los casos con la terminación del programa. Acá es donde se origina el término **segmentation violation** o **segmentation fault**, aún en los procesadores que no soportan segmentación!

Para saber a qué segmento nos estamos refiriendo, usamos un approach **explícito**, dividimos el address space en segmentos basados en los bits más altos de la dirección virtual. Por ejemplo, si tengo 3 segmentos, usamos dos bits más altos de la dirección para marcar los segmentos. Si tengo una dirección virtual de 14 bits, nuestra dirección virtual se ve así:

![[Pasted image 20260510123724.png]]

Entonces si nuestros bits más altos son 00, la CPU sabe que nuestra dirección virtual se refiere al segmento de código, 01 al heap y 10 al stack, usando sus respectivos base and bounds según corresponda.

Básicamente lo que hace el hardware es:

```c
// obtengo segmento
uint64_t segment = (virt_addr & SEG_MASK) >> SEG_SHIFT;
// obtengo offset
uint64_t offset = virt_addr & OFFSET_MASK;

// si estoy out of bounds, lanza excepción:
if (offset >= bounds[segment]) {
	RaiseException(PROTECTION_FAULT);
} else {
	// consigo la dirección física
	paddr_t phy_addr = base[segment] + offset;
	// accedo a memoria
	char reg = access_mem(phy_addr);
}
```

En el ejemplo anterior, los campos serían `SEG_MASK = 0x3000` y `SEG_SHIFT = 12`.

Notar que un segmento no se usa para nada (11). Muchas veces lo que se hace es usar el segmento de código como segmento de código/heap, haciendo que se utilice un solo bit para esto.

Otro problema que podemos notar es que usar explícitamente qué segmento se usa nos limita el máximo número de tamaño de segmento. En el ejemplo de arriba, nos limita a 4KB. Si un programa quiere agrandar su segmento más allá de este tamaño, no puede.

La forma **implícita** de elegir el segmento tiene que ver con cómo generamos el address: si es generado por el PC, entonces es del segmento de código, si lo genera a partir del stack pointer o base pointer, entonces es del segmento del stack, en otro caso es del heap.

Como el stack crece en forma negativa, necesitamos decir no solo la base y el límite del segmento, sino que también debemos decir la orientación en la que crece. Lo que cambia el algoritmo del hardware a algo más así:

```c
// obtengo segmento
uint64_t segment = (virt_addr & SEG_MASK) >> SEG_SHIFT;
// obtengo offset
uint64_t offset = (virt_addr & OFFSET_MASK) >> OFFSET_SHIFT;
uint64_t access_type = virt_addr & ACCESS_TYPE_MASK;

if (!has_permissions(segment, access_type)) {
	RaiseException(PROTECTION_FAULT);
}

paddr_t phy_addr;

// Si estoy en el segmento del stack (crecimiento negativo)
if (!grows_positive[segment]) {
	uint64_t negative_offset = offset - MAX_SEG_SIZE;
	
	// si estoy out of bounds, lanza excepción:
	if (abs(negative_offset) > bounds[segment]) {
		RaiseException(PROTECTION_FAULT);
	}
	
	// consigo la dirección física
	phy_addr = base[segment] + negative_offset;
} else {

	if (offset >= bounds[segment]) {
		RaiseException(PROTECTION_FAULT);
	}
	// consigo la dirección física
	phy_addr = base[segment] + offset;
	
}

// accedo a memoria
char reg = access_mem(phy_addr);
```

Además hay que agregar soporte para compartir código. Para permitir esto, agregamos bits de protección. Estos indican si el segmento es de **Lectura/Escritura**, **Solo Lectura**, **Lectura/Ejecución**.
