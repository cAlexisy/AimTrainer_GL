# 📌 Instalación y Estructura del Proyecto

Este repositorio contiene el desarrollo del proyecto correspondiente al período académico **2024B** para el grupo **GR1CC_GR1**.

---

## 📂 Estructura del Proyecto

Asegúrate de que la estructura de carpetas esté organizada de la siguiente manera dentro del directorio principal del proyecto:

```bash
openGL/
├── shaders/    # Contiene los archivos de shaders
├── textures/   # Contiene las texturas del proyecto
├── sound/      # Nueva carpeta para archivos de sonido
├── models/     # Nueva carpeta para modelos 3D
└── GraficShotApp.cpp  # Archivo principal del proyecto
```

---

## 🚀 Pasos para Configurar el Proyecto

### 1️⃣ Clonar el Repositorio
```bash
git clone https://github.com/tu-usuario/2024B_GR1CC_GR1.git
cd 2024B_GR1CC_GR1
```

### 2️⃣ Crear las Carpetas Necesarias
Si las carpetas `sound` y `models` no existen dentro de `openGL`, créalas manualmente:
```bash
mkdir openGL/sound
mkdir openGL/models
```

### 3️⃣ Agregar Archivos a las Carpetas
- 📁 **`openGL/sound/`** → Coloca aquí los archivos de sonido.
- 📁 **`openGL/models/`** → Guarda los modelos 3D en este directorio.
- 📁 **`openGL/shaders/`** → Debe contener los archivos de shaders.
- 📁 **`openGL/textures/`** → Contiene las texturas utilizadas en el proyecto.

### 4️⃣ Compilar y Ejecutar el Proyecto
- Asegúrate de tener instaladas las bibliotecas necesarias para OpenGL.
- Usa un compilador compatible (como `g++` o `clang++`) para compilar `GraficShotApp.cpp`.
- Ejecuta el programa y verifica que todos los archivos carguen correctamente.

