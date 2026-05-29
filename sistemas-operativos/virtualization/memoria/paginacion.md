# Paginación

Dividir un espacio en distintos trozos de tamaño variable, trae problemas de fragmentación en el espacio, lo que hace al manejo de memoria más complejo a medida que ocupamos más memoria.

Podemos tomar un segundo approach, dividir al espacio de memoria en fragmentos de tamaño determinado. Esta idea se llama **paginación**. En vez de separar al espacio en varias unidades de tamaño fijo que llamamos *páginas*. Vemos a la memoria física como un arreglo de slots de tamaño fijo llamados *page frames*. Cada frame contiene una página de memoria virtual. 

## Ventajas

- *Flexibilidad*: con un approach de paginación, el sistema podrá soportar la abstracción de un address space de forma efectiva, sin importar cómo un proceso esté usando el address space.
- *Simplicidad* del manejo del espacio libre: Podemos usar page tables para guardar traducciones de direcciones virtuales para cada una de las páginas virtuales del address space, permitiéndonos saber dónde en la memoria física reside cada página.

## Traducción

Para traducir una dirección virtual que genera el proceso, primero separamos la dirección en dos componentes: el **virtual page number (VPN)** y el **offset** dentro de esa página. Luego,  se traduce el VPN a su **physical frame number (PFN)** correspondiente.

### ¿Dónde guardamos las Page Tables?

Las page tables pueden ser mucho más grandes que la segment table o base-bounds dado en [[segmentacion|segmentación]]. Por ejemplo en un address space de 32 bits con 4KB, el address space se divide en un VPN de 20 bits y un offset de 12.

>[!important]
>$$
> 10 bits = 1KB \Rightarrow 4KB = 1KB * 4 = 10 bits * 4 = 10 bits << 2 = 4
>$$

Un VPN de 20 bits implica $2^{20}$ traduccionnes que el SO debería manejar para cada proceso. Asumiendo que necesitamos 4 bytes por *page table entry* (PTE) para mantener la traducción física y otras cosas útiles (como permisos), necesitamos unos 4MB de memoria por cada page table. Ahora imaginemos esto por 100 procesos que pueden estar corriendo en simultáneo en nuestro SO, necesitaríamos 400MB de memoria solo para las traducciones...

Como las page tables son muy grandes, no las guardamos en el chip del hardware de la MMU, sino que las guardamos en algún lugar de la memoria.

### ¿Qué hay en las Page Tables?

Una PT no es más  que una estructura de datos utilizada para mappear direcciones virtuales a direcciones físicas (o VPNs a PFNs), por lo que cualquier estructura de datos funcionaría.

La más simple se llama *Page Table lineal*, que solo es un array. El SO indexa al array por la VPN y busca su PTE en ese índice para así encontrar su PFN correspondiente. Por ahora supongamos que es lineal, luego retomaremos esto

En cuanto a los contenidos, tenemos diferentes bits que valen la pena aprender para qué sirven. Tenemos un **bit de validez**, que indica si una traducción particular es válida. Por ejemplo, cuando corremos un programa, al comienzo tenemos el código y el heap en un extremo del address space y el stack en el otro; todo el espacio sin utilizar en el medio se marca como inválido y, si el proceso trata de acceder a ellas, salta una trap del procesador. De esta manera, nos ahorramos la necesidad de alojar frames físicos para esas páginas y así salvamos gran parte de la memoria.

También tenemos **bits de protección**, indicando si una página se puede leer, escribir o ejecutar. En cuyo caso hagamos una operación no válida, salta trap del procesador. Otros bits incluyen el **bit de presentismo**, que nos indica si una página está en memoria física o en el disco, es decir, que hubo un swap. Un **bit dirty** también es común, indicando si hubo una modificación en la página desde que fue traída desde memoria.

También se utiliza un **bit de referencia** o **acceso**, que se utiliza para llevar un registro de si la página fue accedida, lo que es útil saber a la hora de reemplazar las páginas ([[notas-cache#Organización Cache|LRU]] por ejemplo).

## Lentitud...

Con las page tables en memoria, sabemos que pueden llegar a ser muy grandes y hasta pueden relentizar tareas.

Tengamos el caso en que tenemos la siguiente instrucción:

```x86_asm
movl 21, %eax
```

Asumimos que el hardware se encarga de la traducción. Para hacer el fetch del dato, el sistema primero traduce la dirección virtual a su correcta dirección física. Entonces, antes de hacer fetch del dato, el sistema tiene que primero hacer un fetch de la PTE correspondiente de la PT del proceso, y luego cargar datos de memoria.

Es muy lento no? Bueno, que tal si implementamos de alguna manera algo que acelere esto, que ya tienen los procesadores...

## Translation Lookaside Buffer (TLB)

Como ya mencionamos, la paginación a secas es lenta, pues por cada instrucción tenemos que hacer un doble fetch para buscar la localización física de un dato en una cierta dirección lógica/virtual. ¿Cómo aceleramos esto? Usamos la vieja confiable: hardware.

Agregamos lo que se llama **Translation-Lookaside Buffer (TLB)**. Esta pieza es parte de la MMU del chip. Es simplemente un [[notas-cache|cache]] de traducciones virtuales a físicas muy recurridas, se puede pensar que un mejor nombre es **address-translation cache**.

Al hacer referencia a memoria virtual, el hardware primero revisa si está en la TLB la traducción a esa address. Si está, se brinda la traducción guardada en la cache, de manera rápida, sin tener que recurrir a consultar las page tables. Es la rapidez de la TLB lo que permite el uso de la paginación y hace que la memoria virtual sea posible.

### Algoritmo básico

#### Pseudocódigo

```python
VPN = (VirtualAddress & VPN_mask) >> SHIFT
(success, tlb_entry) = tlb_lookup(VPN)
if success: # TLB HIT
	if can_access(tlb_entry.protection_bits):
		offset = VirtualAddress & OFFSET_MASK
		phy_addr = (tlb_entry.PFN << SHIFT) | offset
		register = access_memory(phy_addr)
	else:
		raise PROTECTION_FAULT_EXCEPTION
else
	pte_addr = PTBR + (VPN * sizeof(PTE))
	pte = access_memory(pte_addr)
	if !pte.valid:
		raise SEGFAULT_EXCEPTION
	elif can_access(pte.protection_bits):
		raise PROTECTION_FAULT_EXCEPTION
	else:
		tlb_insert(VPN, pte.PFN, pte.protection_bits)
		RetryInstruction()
```

#### Explicación

Asumamos una page table lineal y una TLB administrada por el hardware.

El algoritmo primero extrae el VPN de la address virtual, y revisamos si la TLB tiene la traducción o no. Si la tiene, se produce un **TLB hit**, entonces extraemos el PFN de la entrada de la TLB, concatenamos eso al offset y formamos la dirección física deseada. y accedemos a memoria en caso de que no haya problemas de protección.

Si no la tiene en la TLB, se produce un **TLB miss**. En ese caso, tenemos que bajar y hacer todo el camino hasta traducir correctamente la dirección virtual a su física correspondiente, revisando si es válida la traducción y accesible con nuestro nivel de protección actual. Luego se inserta en la TLB la nueva traducción para futuras referencias a esa dirección, pues por principio de localidad volveremos a acceder a ella.

#### Tamaños de página

Necesitamos que los tamaño de página sean lo suficientemente grandes como para tener un gran hit-rate y reducir el miss-rate lo más posible. Un tamaño típico de página es 4 KB.

#### ¿Quién resuelve los TLB misses?

Dos actores: el hardware o el SO. En arquitecturas CISC, mucho se maneja por hardware, y la TLB no es menos, casi en su totalidad la maneja el hardware, y poco el SO. Para hacer esto, el hardware debe saber exactamente *dónde* están las page tables en memoria, usando un **page table register**. También debe saber su formato exacto.  Un ejemplo es la TLB manejada por hardware en la arquitectura **x86**, que usa una page table multinivel que tiene un puntero en el registro CR3.

Arquitecturas más modernas o aquellas que son RISC, tienen una TLB manejada por software. En un TLB miss, el hardware levanta una excepción que pausa la ejecución de instrucciones, eleva el nivel de privilegio a kernel y ejecuta un trap handler. Hay unos detallitos:

1. El **return-from-trap** tiene que ser distinto a las de las syscalls. Las syscalls retoman la ejecución desde la instrucción que viene *después* de la trap al SO. Al devolver desde un TLB miss handler, el hardware debe retomar la ejecución desde la instrucción que *causó* la trap, nada más que ahora en vez de causar un TLB miss, causa un TLB hit.
2. Cuando corremos el código del handler del TLB miss, debemos evitar que ocurra una cadena eterna de TLB misses. Esto se puede hacer, por ejemplo, manteniendo los handlers de TLB en memoria física, o reservando algunas entradas de la TLB para traducciones permanentemente válidas que se usan para el código del handler en sí mismo.

Hacer esto por software nos da gran flexibilidad. El SO puede usar cualquier estructura de datos que quiera para implementar la page table, sin requerir cambios en el hardware. Además nos da simplicidad. El hardware no hace mucho en el miss, solo eleva una excepción y deja que el handler del SO haga lo que tenga que hacer para resolverlo.