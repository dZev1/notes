# Sistemas Operativos

## Table of contents

| Semana |                                                    Tema                                                    |
| :----: | :--------------------------------------------------------------------------------------------------------: |
|   01   |                          [[sistemas-operativos#Sistema Operativo\|Introducción]]                           |
|   02   |                                      [[procesos#Procesos\|Procesos]]                                       |
|   03   |              [[procesos#InterProcess Communication (IPC)\|Comunicación entre procesos (IPC)]]              |
|   04   |                                   [[sistemas-operativos/virtualization/procesos/scheduling#Scheduling\|Scheduling]]                                    |
|   05   | [[threads#Threads\|Threads]], [[sincronizacion#Sincronización de Procesos\|Sincronización entre procesos]] |
|   06   |                               [[memoria#Memoria\|Administración de memoria]]                               |
|        |                                                                                                            |
|        |                                                                                                            |
|        |                                                                                                            |
|        |                                                                                                            |

## Sistema Operativo

Es un intermediario entre el software y el hardware. Hace que el software específico no se preocupe con los detalles de bajo nivel del hardware (visión de usuario), y que el usuario use correctamente el hardware (visión del propietario del hardware).

En un momento de la historia, con los primeros SO, nació el concepto de **multiprogramación**. Si se tienen dos trabajos, $j_1$ y $j_2$, ocurre que $j_1$ toma el mismo tiempo que sin multiprogramación, pero $j_1 + j_2$ tarda menos.

Además, a partir del concepto de multiprogramación nace también el de **contención**, que refiere a la capacidad de varios programas de acceder a un mismo recurso a la vez.

**Timesharing** es una variación de la multiprogramación. Consiste en conectar muchas terminales a una misma computadora, y darles un poco de tiempo de procesador a aquellas que están siendo usadas.

El pionero en usar *timesharing* fue MULTICS, del cual luego nacería UNIX, de la mano de **Ken Thompson** y **Dennis Ritchie**, que luego llevaría a la creación de Linux, de mano de **Linus Torvalds**.

Retomando, un SO es una pieza de software que media entre el HW y los programas del usuario. Tiene que manejar la contención y la concurrencia de manera tal que logre hacerlo con buen rendimiento y correctamente. Para ello, corre en nivel de privilegio 0, es decir de **máximo privilegio**.

## Elementos de un SO

### Drivers

Programas que son parte del sistema operativo que manejan los detalles de bajo nivel relacionados a cómo operan los distintos dispositivos.

### Kernel

Es el sistema operativo propiamente dicho. Es el cerebro. se encarga de las tareas fundamentales y contiene diversos subsistemas.

### Shell

Un programa, que muchas veces es ejecutado automáticamente cuando se inicia el sistema operativo, el cual permite interactuar con el sistema operativo.

### Proceso

Un programa en ejecución con su espacio de memoria asociado y otros atributos.

### Archivo

Secuencia de bits que tiene un nombre y una serie de atributos que dan, por ejemplo, permisos.

### Directorio

Colección de archivos y directorios que contiene un nombre y está organizado jerárquicamente.

### Dispositivo virtual

Abstracción de un dispositivo físico bajo la forma de un archivo, que permite que este se pueda abrir, leer, escribir, entre otras cosas.

### Sistema de archivos

Es el cómo se organizan los datos en el disco para gestionar su acceso, permisos, etc.

### Directorios del sistema

Son directorios donde el SO guarda archivos que necesita para su almacenamiento, por ejemplo `/boot`, `/devices`, `C:\Windows\System32`.

### Binarios del sistema

Son archivos que viven en los directorios del sistema. No forman parte del kernel pero suelen llevar a cabo tareas muy importantes o proveen utilidades básicas del sistema, como por ejemplo `/usr/bin/bash`, `/usr/sbin/syslogd`, `/usr/bin/who`

### Archivo de configuración

Es un archivo del cual el sistema operativo saca información que necesita para funcionar, por ejemplo `/etc/passwd` o `C:\Windows\System32\user.dat`.

### Usuario

Representación de las personas o entidades, dentro del SO, que van a utilizarlo. Sirve para aislar información entre sí y establece limitaciones.

### Grupo

Colección de usuarios.

### Estructura de un sistema UNIX

![[Pasted image 20260315173317.png]]