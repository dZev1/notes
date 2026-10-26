# File System

## Ejercicio 10

Tenemos un disco con FAT.

Queremos implementar `cargar_archivos(const char** directorios)`.

Tenemos
- `FAT_entry(block_address)`
- `raw_data = read_blocks(block_address1, block_address2, ...)`. 
- `parse_directory_entries(raw_data)`
- `raw_data = root_table()`

```c++
char* cargar_archivo(const std::vector<std::string>& directorios) {
	char* raw_data = root_table();
	
	for (unsigned long i = 0; i < directorios.size; i++) {
		struct entrada_directorio entries[] = parse_directory_entries(raw_data);
		
		struct entrada_directorio entrada_destino;
		bool found = 0;
		
		for (unsigned long j = 0; j < entries.size; j++) {
			struct entrada_directorio entrada = entries[j];
			if (strcmp(entrada.nombre, directorios[i]) == 0) {
				entrada_destino = entrada;
				found = 1
			}
		}
		
		if (!found) return NULL;
		
		unsigned long bloque_actual = entrada_destino.primer_bloque;
		std::vector<unsigned long> lista_bloques(); 
		
		while (bloque_actual != DELIMITADOR_EOF) {
			lista_bloques.push_back(bloque_actual);
			bloque_actual = FAT_ENTRY(bloque_actual);
		}
		 
		raw_data = read_blocks(lista_bloques);
	}
	
	return raw_data;
}
```