# Seguridad de la información

- Es la conservación de estas características:
	- **Confidencialidad**: Acceso sólo para los sujetos autorizados.
	- **Integridad**: Prevenir o detectar cambios no autorizados.
	- **Disponibilidad**: Acceso oportuno a información y servicios.
- Seguridad *de la información* → Incluye personas, procesos y soportes físicos.
- Seguridad *informática* → Protege los sistemas que la procesan.
- Tres componentes principales:
	- Sujetos
	- Objetos.
	- Acciones.
- **Sujetos** realizan **acciones** sobre **objetos**.
- Los roles de sujeto y objeto no son excluyentes.

## Usuario

- Abstracción más común
- Un usuario es un sujeto del SO, que puede ejecutar acciones y que es hasta dueño de:
	- archivos
	- procesos
	- memoria
	- conexiones
	- ...
- El usuario puede sobre estos objetos:
	- leer
	- escribir
	- abrir
	- ejecutar
	- copiar
	- ...
- Se pueden agrupar en colecciones de usuarios.
- Los grupos son también sujetos del sistema de permisos.
- Los roles también son otra abstracción que se utiliza para agrupar.

## AAA
- **Autenticación:** verifica que somos quienes decimos que somos. 
	- Contraseñas, medios biométricos, etcétera.
	- Usan criptografía.
- **Autorización:** qué podemos hacer.
- **Auditoría:** dejamos registrados qué hicimos.
- Las funciones de Authentication y Authorization están claramente diferenciadas.

## Criptografía
- Ciencia que aporta mecanismos para confidencialidad, integridad y autenticación.
- Cifrado **simétrico**: misma clave para cifrar y descifrar.
	- **AES**
	- **ChaCha20**
- Cifrado **asimétrico**: par de claves pública y privada. Una es la inversa de la otra.
	- **RSA**
- Los **hashes criptográficos** son resúmenes de tamaño fijo. No son cifrado ni tienen claves de descifrado.
- Cifrar por sí solo no da integridad.

### Funciones de hash
- Tienen que cumplir 2 de estas propiedades:
	- **Resistencia a preimagen**
		- Dado $h$, es difícil encontrar $m$ con $H(m) = h$.
		- Complejidad en reversibilidad de hash.
	- **Resistencia a segunda preimagen**
		- Dado $m_1$, es difícil encontrar $m_2 \neq m_1$ con $H(m_1)=H(m_2)$.
		- Es casi imposible reemplazar o falsificar el mensaje preexistente.
	- **Resistencia a colisiones**
		- Es difícil encontrar *cualquier* par $m_1 \neq m_2$ con el mismo hash.
		- Es computacionalmente difícil encontrar dos mensajes con el mismo hash.
- SALT: valor aleatorio que elige el sistema, diferente para la contraseña de cada usuario.
	- Agrega entropía a la clave del usuario.
	- Si hay dos usuarios con la misma contraseña, la clave almacenada de manera segura es distinta.
- Se está usando últimamente MFA, que combina factores de categorías distintas.
	- Algo que tenemos y algo que somos.
	- Dos contraseñas no es MFA.

## RSA

- Se generan dos primos $p,q$ muy grandes y secretos, y un módulo $n=p*q$.
- Clave pública $(n,e)$.
- Clave privada $d$ y habitualmente, parámetros derivados de $p,q$.
- Se cifra con la clave pública del receptor y se descifra con su clave privada.
- Se **encripta** cifrando cada letra $m$ calculando el resto de dividir $m^e$ por $n$.
- Se **desencripta** calculando, para cada letra encriptada $c$, el resto de dividir $c^d$ por $n$.

### Firma digital
- Calcular un hash para un documento.
- Se encripta el hash usando la clave privada.
- Entregamos el documento con el hash encriptado.
	- Esto a fin de detectar modificaciones y vinculamos la firma con la clave privada correspondiente.
- El receptor desencripta con nuestra clave pública. Si lo puede desencriptar exitosamente, se asegura que nosotros seamos los autores.
- Luego verifica que el hash obtenido se corresponda con el documento.

### Certificados
- Vinculan una indentidad con una clave pública y esta firmado por una CA.
- Verificar cadena de confianza, identidad esperada, vigencia y usos autorizados.
- Considerar revocación según el protocolo.
- En TLS, el cert permite autenticar al servidor. Claves de sesión protegen la comunicación.
- Desactivar la verificación permite ataques MiTM, aun cuando el tráfico esté cifrado.

## Criptografía Post-Cuántica
- Algoritmos diseñados para resistir ataques clásicos y cuánticos conocidos.
- El algoritmo de Shor amenaza con RSA, DH y ECC con una computadora cuántica lo suficientemente grande.
- Shor no afecta a AES y hashes de la misma manera que a RSA o ECC.
- Hacen *Harvest Now, Decrypt Later*: capturan datos cifrados hoy para intentar descifrarlos cuando exista capacidad cuántica suficiente.
## Autenticación remota con hash

- Hay *replay-attacks* si el canal de comunicación no está cifrado.
- Métodos *Challenge-Response*
	- El servidor elige un número al azar, que comunica al cliente.
	- El cliente tiene que encriptar la contraseña utilizando ese número como seed.
	- El servidor hace lo mismo y se fija si coinciden.
	- **esto también puede ser atacado.**

## Autorización

- Monitor de referencias.
- Media las operaciones que intentan realizar los sujetos sobre los objetos en función de una política de acceso.
- Mediación completa, se controla cada acceso relevante.
- El monitor de referencias debe resistir modificaciones no autorizadas y ser tan pequeño como para analizarlo y verificarlo.

### DAC
- Control de Acceso Discrecional.
- El propietario concede o revoca permisos dentro de las reglas del sistema.

### MAC
- Control de Acceso Obligatorio (Mandatory).
- Una política central restringe los accesos.
- El propietario no puede eludirla modificando permisos DAC.

- MAC no siempre compara niveles.
- Pueden coexistir MAC y DAC.
	- Un permiso DAC no basta si MAC deniega.
- RBAC (Role Based Access Control) organiza permisos en roles y asigna roles a usuarios.

### DAC en UNIX

- Permisos básicos en UNIX:
![[Pasted image 20261021230602.png]]

### SETUID y SETGID

- `setuid` puede cambiar el UID **efectivo** al propietario.
- `setgid` el GID **efectivo** al grupo del archivo.
- No elevan necesariamente a `root`, depende del propietario o grupo.
- `setgid` hace heredar el grupo a objetos nuevos y el bit a subdirectorios.
## Problemas de seguridad

- **Arquitectura/diseño**
	- Cuando estamos pensando la aplicación.
- **Implementación**
	- Cuando estamos escribiendo el código de la aplicación.
- **Operación**
	- Cuando la aplicación se encuentra productiva.

### Implementación

- Son debilidades fáciles de entender y solucionar.
- Por ejemplo, hacer suposiciones sobre el ambiente del programa.
- La entrada puede venir de:
	- Environment variables
	- Entradas del programa
	- otras fuentes.
- Ejemplos
	- Buffer overflows por strcpy.

- **Mecanismos de prevención**
	- *ASLR* → Aleatoriza ubicaciones del espacio de direcciones.
	- *NX / DEP* → impide ejecutar en páginas marcadas no ejecutables.
	- *Stack Canaries* → Detectan corrupciones antes de retornar.