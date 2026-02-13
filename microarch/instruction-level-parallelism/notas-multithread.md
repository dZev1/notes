# Procesadores Multithread

- Estado Arquitectural = registros.
- En procesadores convencionales, metés un thread al estado arquitectural, y largás el resultado.
- Un procesador multithread tiene dos estados arquitecturales, son dos vistas de un procesador.
	- Se duplica la ISA.
	- Los caches y el resto de las unidades están compartidas por los threads, son únicas.
- De esta manera, metemos dos threads y podemos conseguir dos resultados en paralelo.
	- Se ocupan la mayor cantidad de unidades de ejecución posibles, cosa que en procesadores convencionales no ocurría, quedaban libres muchas.
- Se duplican los $\mu$-op Queues, los IPs/PCs, las unidades de paginación y los RAT y Reorder Buffer (una por thread).

## Varios Cores = performance y menor consumo.