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

>[!Note] Bit `valid` de TLB $\neq$ bit `valid` de Page Table
>
>El bit `valid` de la page table indica que esa PTE no fue todavía alojada por el proceso, por lo que no puede ser accedida por un programa que funcione correctamente. Esto envía una trap al procesador que termina el proceso.
>El bit `valid` de una entrada de la TLB refiere a que esa entrada no tiene una traducción válida en él. Es útil a la hora de hacer context switches, entre otras cosas, como notaré más adelante.

### Contenidos de una TLB

Una TLB típica suele tener 32, 64 o 128 entradas que son **fully associative**, es decir las entradas pueden colocarse en cualquier lugar de la TLB, y que el hardware busca en toda la TLB en paralelo hasta encontrar la traducción deseada. Una entrada puede verse algo así:

![[Pasted image 20260531152033.png]]

El VPN y PFN están en cada entrada, pues la traducción puede llegar a cualquiera de estas ubicaciones.
Lo interesante viene en esos *other bits*. Comúnmente tiene un bit de **validez**, que nos dice si hay una traducción válida en esa entrada o no. También suele haber bits de **protección**, que nos dicen de qué manera se puede acceder a una página (Read and Execute, Read and Write, ...). También hay otros campos, como un **identificador de address-space** o un bit **dirty**.

### Context Switches

Con la llegada de la TLB, tenemos nuevos problemas al hacer context switches. Específicamente, la TLB tiene traducciones virtual->física que solo son válidas para el proceso que está corriendo, pero no para el resto de procesos.
Entonces cuando hacemos un switch de un proceso a otro, el hardware y el SO deben tener garantizar que el proceso que está por correr no acceda accidentalmente a las traducciones de un proceso que corrió antes.

Para administrar los contenidos en un context switch, tenemos un gran número de approaches. Uno es simplemente hacer un flush de la TLB en los cambios, limpiándola completamente antes de correr el próximo proceso. Esto trae un costo: cada vez que un proceso corre, debe incurrir en TLB misses cuando entre en páginas de código y datos. Si el SO cambia entre procesos frecuentemente, el costo es muy alto.

Para reducir este overhead, algunos sistemas agregan soporte al hardware para compartir la TLB entre context switches. Algunos sistemas de hardware proveen un **address space identifier (ASID)** como campo en la TLB. Este ASID es como un PID, pero con menos bits.

Se usa una política de reemplazo **LRU** o **Random**, cada una con sus pros y sus contras.

### Una entrada real

Veamos esta entrada de un MIPS R4000, que usa una TLB administrada por software. En la imagen se puede observar la entrada

![[Pasted image 20260531160803.png]]

Este procesador soporta un address space de 32 bits con páginas de 4 KB. Por ende, debería tener un VPN de 20 bits y un offset de 12 bits en nuestras direcciones. Pero tenemos solo 19 bits para la VPN, porque las direcciones de usuario vendrán solo de la mitad de las direcciones, el resto está reservado para el kernel, dejándonos en `20 >> 2 = 19` bits para el VPN. Este VPN se traduce a 24 bits de PFN, y entonces soporta sistemas de hasta 64 GB de memoria física principal ($2^{24}$ páginas de 4KB).

Tenemos también un bit **global** (G), que es utilizado para páginas compartidas globalmente entre procesos. Si `G=1` se ignora el `ASID`. El ASID es de 8 bits, soportando hasta 256 procesos. ¿Qué hacemos si tenemos más de 256 procesos corriendo a la vez? Pues, tenemos un 3 bits de **coherencia** (C), que determinan cómo una página se inserta en el cache por el hardware. El bit **dirty** marca que se modificó la página. El bit de **validez** nos dice si está presente la traducción en la entrada.

## Tablas más pequeñas

Las page tables son muy grandes, y por ende consumen mucha memoria. Sigamos con un address space de 32 bits ($2^{32}$ bytes), con páginas de 4KB ($2^{12}$ bytes) y una page entry de 4 bytes.

Un address space tiene como un millón de páginas virtuales, que multiplicado por el tamaño de la entrada de la page table vemos que nuestra page table es de 4MB de tamaño, y sumemos a esto que tenemos una page table por cada proceso del sistema. Con cientos de procesos activos, algo común en SOs modernos, estaríamos alojando cientos de MBs de memoria SOLO para las page tables. ¿Cómo solucionamos este problema?

### **Solución simple:** Páginas más grandes

Podríamos reducir el tamaño de la page table aumentando el tamaño de las páginas. En nuestro address space de 32 bits, tomemos páginas de 16KB. Entonces tendríamos VPNs de 18 bits y un offset de 14 bits. Teniendo en cuenta que no aumentamos el tamaño de las entradas de la page table, tenemos ahora $2^{18}$ entradas en nuestra page table, usando un total de 1MB por page table.

Esto trae como problema que desperdiciemos espacio *dentro* de cada página (**fragmentación interna**). Así las aplicaciones terminan alojando páginas pero solo usando pocos bytes de cada uno y se llena la memoria de páginas enormes. Por eso la mayoría de sistemas usan tamaños de página pequeños: 4KB (como en x86) u 8KB (como en SPARCv9).

### **Solución híbrida:** Paginación y Segmentos

Podemos combinar la paginación con la segmentación para así reducir el overhead de memoria de las page tables. Asumamos que tenemos un address space en el que las porciones del heap y el stack usadas son pequeñas. Entonces la page table si fuese lineal estaría llena de entradas inválidas...

Metamos ahora la segmentación de por medio. En vez de tener una sola page table por todo el address space del proceso, por qué no tener una por segmento lógico. Tenemos un registro **base** y un registro **bound** que nos marcan el inicio del address space y el tamaño respectivamente. En el híbrido aún tenemos estas estructuras en nuestra MMU, así que usamos al **base** para apuntar a la address física de la page table del segmento. El **bound** entonces nos indica el fin de esa page table.

Esto nos trae un gran ahorro de memoria comparado con la page table lineal. Las páginas sin alojar entre el stack y el heap ya no ocupan espacio de la page table.

Pero aún tenemos problemas. Seguimos **usando segmentación**, lo que no es flexible; si tenemos un heap disperso, podemos aún así tener mucho desperdicio en page tables. Además el híbrido vuelve a insertar la **fragmentación externa** en la mesa. Mientras que toda la memoria se maneja en unidades de tamaño en base a páginas, las page tables ahora pueden ser de tamaño arbitrario. Encontrar espacio libre entre ellas es más complicado. Entonces seguimos buscando cómo.

## Page Tables Multinivel

