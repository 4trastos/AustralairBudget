# AustralairBudget
Desarrollo de software de presupuestos en C++ multiplataforma, con una interfaz amigable y maneja una base de datos local.

## Software de Presupuestos para Australair

Para cumplir con todos los requisitos, se propone la siguiente arquitectura y selección de tecnologías en **C++**:

### 1. Interfaz Gráfica y Multiplataforma (Mac y Windows)

* **Framework Elegido:** **Qt Framework** 🖼️
    * **Motivo:** Qt es el estándar de oro para aplicaciones C++ multiplataforma (Windows, macOS, Linux, etc.). Permite desarrollar una única base de código que se compila para ambos sistemas operativos, garantizando una **interfaz gráfica amigable e intuitiva**. Ofrece herramientas como **Qt Designer** para crear la interfaz visualmente, lo que acelera el desarrollo.

### 2. Gestión de Base de Datos Local

* **Base de Datos Elegida:** **SQLite** 💾
    * **Motivo:** SQLite es una base de datos **embebida** muy ligera y popular para aplicaciones de escritorio. Al ser un motor de base de datos *sin servidor*, toda la información se guarda en un solo archivo dentro del sistema del usuario, lo que simplifica la instalación y el uso de la aplicación, siendo perfecta para poder **guardar cada presupuesto, modificarlo y acceder a él**. Qt tiene módulos específicos (Qt SQL) que facilitan la interacción con SQLite.

### 3. Proceso de Desarrollo y Despliegue

| Fase | Método | Descripción |
| :--- | :--- | :--- |
| **Desarrollo** | **Compilación Directa** | Se utilizará un IDE (como Qt Creator o Visual Studio con el *plugin* de Qt) que compila el código C++ con el *framework* Qt. Esto permite la **iteración y prueba rápida** durante el desarrollo. |
| **Finalización** | **Despliegue** | Una vez terminado el programa, se utilizarán las herramientas de despliegue de Qt (como `windeployqt` en Windows y `macdeployqt` en macOS) para crear los **paquetes de aplicaciones de escritorio** (`.exe`, `.app`) que incluyen todas las dependencias necesarias. |

***

## 📋 Estructura de Datos (Campos del Presupuesto)

La aplicación gestionará los siguientes campos:

### Cliente y Contacto
* **Nombre Completo/Empresa:** Texto.
* **Persona de Contacto:** Texto.
* **Teléfono:** Texto/Numérico.
* **Email:** Texto.
* **Dirección Completa:** Texto.

### Datos del Presupuesto (Australair)
| Campo | Tipo | Notas |
| :--- | :--- | :--- |
| **Metros Cuadrados a Climatizar** | Numérico | Para el cálculo de la potencia o número de equipos. |
| **Tipo de Local** | Lista Desplegable | Valores fijos: Nave industrial, local comercial, vivienda, pista de pádel *indoor*. |
| **Localidad** | Texto. |
| **Tipo de Cubierta** | Texto. |
| **KM (Desplazamiento)** | Numérico. |
| **Peones/Empleados** | Numérico. |
| **Días de Trabajo** | Numérico. |
| **Horas Estimadas** | Numérico. |
| **Materiales** | Sub-Tabla/Lista | **Esencial:** Permite añadir Nº de máquinas, tuberías, tornillos, arandelas, pintura, remaches, etc., cada uno con su cantidad y precio. |

### Cálculo de Precios y Total
| Campo | Cálculo | Notas |
| :--- | :--- | :--- |
| **Subtotal** | Suma de Materiales, Costo KM, Costo Peones, etc. | Costos fijos + Costos variables. |
| **IVA (21%)** | $Subtotal \times 0.21$ | Se mostrará como campo separado. |
| **Precio Total (con IVA)** | $Subtotal + IVA$ | Campo principal a mostrar. |
| **Precio Total (sin IVA)** | $Subtotal$ | Se mostrará para referencia. |

***

## 💰 Definición Inicial de Precios (Orientativos y Modificables)

Según sus indicaciones, se establecerán precios base que el usuario final podrá **modificar en el panel de configuración de la aplicación**.

Se define un **Precio Base** de **$10€$** y un incremento de **$5€$** por cada apartado principal:

| Apartado de Coste | Base | Incremento | **Precio Inicial (por unidad)** |
| :--- | :--- | :--- | :--- |
| **Coste por KM** | $10€$ | $+5€$ | **$15€$/KM** |
| **Coste por Peón/Empleado (por día)** | $10€$ | $+5€$ | **$20€$/Día** |
| **Coste por Hora de Trabajo** | $10€$ | $+5€$ | **$25€$/Hora** |
| **Coste por Día de Trabajo (General/Gestión)** | $10€$ | $+5€$ | **$30€$/Día** |
| **Materiales (P. Base)** | $10€$ | $0€$ | **$10€$/Unidad** (Precio inicial para cualquier material nuevo añadido) |

El sistema permitirá al usuario **acceder a una tabla de configuración** para ajustar estos precios.

**Ejemplo de Cálculo Básico:**
* **KM:** $10$ KM $\times 15€/KM = 150€$
* **Peones:** $2$ Peones $\times 3$ Días $\times 20€/$Día $= 120€$
* **Horas:** $40$ Horas $\times 25€/$Hora $= 1000€$
* **Materiales:** $5$ Máquinas $\times 1000€/$Máquina (ejemplo de precio modificado) $= 5000€$
* **Subtotal:** $150 + 120 + 1000 + 5000 = 6270€$
* **IVA (21%):** $1316.70€$
* **Total (con IVA):** $7586.70€$

Este desarrollo le proporcionará a Australair una herramienta robusta, eficiente y fácil de usar en los principales sistemas operativos de escritorio.
