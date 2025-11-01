# Proyecto NumerosYCalculos

## Objetivo del Proyecto

Una biblioteca C++ de alto rendimiento, moderna y multi-compilador para algoritmos matemáticos avanzados.

El objetivo es proporcionar implementaciones robustas, genéricas y `constexpr` para operaciones con enteros de precisión extendida, incluyendo `__int128`, `uint128_t` y tipos de `boost::multipricision` (hasta 2048 bits).

### Características Principales

* **Sistema de Tipos Avanzado:** Detección y traits para enteros extendidos.
* **E/S Robusta:** Soporte completo de `std::ostream` y `std::istream` para `__int128`, incluyendo `to_string`.
* **`constexpr` Core:** Literales de usuario (ej. `_i128`) y funciones matemáticas (ej. `integer_power`) evaluables en tiempo de compilación.
* **Manejo de Errores Moderno:** Usa `core::Expected<T, E>` (basado en `tl::expected`) para un manejo de errores funcional y `noexcept`.
* **Optimizado:** Utiliza `lookup_tables` (LUTs) y algoritmos optimizados (como `integer_log2` intrínseco) para bases comunes.

## ¿Cómo Compilar? (Multi-Configuración)

Este proyecto utiliza un sistema de build basado en CMake, gestionado por scripts que generan directorios de build separados para cada configuración.

**¡IMPORTANTE!** El script principal para Windows es `build_multi_final.bat`.

### Uso del Script (Windows CMD / PowerShell)

Navega a la raíz del proyecto y usa el siguiente comando:

```batch
.\scripts\build_multi_final.bat [compilador] [tipo] [estandar_cpp]
```

**Argumentos:**

1.  **`[compilador]`**: El compilador a usar.
    * `gcc`: Para GCC (desde MSYS2/MinGW).
    * `clang`: Para Clang (desde MSYS2).
    * `msvc`: Para Microsoft Visual C++ (Visual Studio 2022).
2.  **`[tipo]`**: El tipo de build.
    * `debug`: Compila con información de depuración (más lento).
    * `release`: Compila con optimizaciones (más rápido).
3.  **`[estandar_cpp]`**: El estándar de C++ a forzar.
    * `17`: (C++17)
    * `20`: (C++20)
    * `23`: (C++23 o `/std:c++latest` para MSVC).

### Ejemplos

```batch
rem Compilar con GCC, en modo Debug, usando C++17
.\scripts\build_multi_final.bat gcc debug 17

rem Compilar con Clang, en modo Release, usando C++20
.\scripts\build_multi_final.bat clang release 20

rem Compilar con MSVC, en modo Release, usando C++23 (latest)
.\scripts\build_multi_final.bat msvc release 23
```

Esto configurará, compilará y generará los ejecutables (ej. `demo_main.exe`) dentro de un directorio de build único, como `build_gcc_debug_cpp17/`.

## Estructura de Directorios

```
NumerosYCalculos/
+-- build_[config]/    # (Generado por el script)
+-- include/           # Headers de la biblioteca (API pública)
¦   +-- core/          # (type_traits, numeric_io, constexpr_literals...)
¦   +-- math/          # (integer_ops, combinatorics...)
+-- src/               # Demos y ejecutables (main.cpp)
+-- tests/             # Pruebas unitarias (Catch2)
+-- benchmarks/        # Pruebas de rendimiento
+-- scripts/           # Scripts de automatización (build_multi_final.bat)
+-- CMakeLists.txt     # Script principal de CMake
+-- LICENSE            # Licencia del proyecto
```

## Licencia

Este proyecto está bajo la **Licencia MIT**. Consulta el archivo `LICENSE` para más detalles.
    </script>

    <!-- 6. Script para renderizar el Markdown -->
    <script>
        document.addEventListener('DOMContentLoaded', () => {
            const markdownInput = document.getElementById('markdown-content').textContent;
            const contentContainer = document.getElementById('content-container');
            
            // Configurar Marked.js
            marked.setOptions({
                gfm: true, // GitHub Flavored Markdown
                breaks: false,
                sanitize: false // Confiamos en nuestro propio contenido
            });
            
            // Convertir y aplicar estilos
            contentContainer.innerHTML = marked.parse(markdownInput);
            
            // Aplicar las clases de tipografía de Tailwind al contenido
            contentContainer.classList.add(
                'prose',                  // Estilos base de tipografía
                'prose-lg',               // Tamaño de fuente más grande
                'dark:prose-invert',      // Estilos para modo oscuro
                'prose-headings:font-bold',
                'prose-a:text-blue-600',
                'dark:prose-a:text-blue-400',
                'prose-pre:bg-gray-100',  // Estilo de bloques de código
                'dark:prose-pre:bg-gray-900',
                'prose-pre:rounded-lg',
                'prose-code:text-purple-600',
                'dark:prose-code:text-purple-400',
                'max-w-none'              // Eliminar límite de ancho de 'prose'
            );
        });

