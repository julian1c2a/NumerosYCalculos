# Guión de Construcción del Proyecto NumerosYCalculos


## Objetivo del Proyecto


Crear un proyecto C++ avanzado de algoritmos matemáticos sobre números enteros y racionales con las siguientes características:


### Requisitos Funcionales


1. **Biblioteca de Funciones Matemáticas sobre Z y R Avanzada**

   - Implementar algoritmos de factorial, combinaciones, permutaciones

   - Soporte para números enteros no estándar propios de compiladores de 128 bits (`__int128`)
   
   - Soporte para números enteros procedentes de Boost.Multiprecision, tanto los 3 tipos de longitud 
   arbitraria (cpp_int, tom_int y mpz_int), como para los tipos de 128, 256, 512, 1024 y 2048 bits.

   - Completación de los std::extended_type_traits para los nuevos tipos y la extensión de std::numeric_limits
   también a esos tipos enteros y los racionales basados en esos tipos.

   - Funciones constexpr para cálculos en tiempo de compilación (nivel de optimización)                                                                  
   
   - Implementación de tablas constexpr lookup_tables (nuevo nivel de optimización)

   - Literales personalizados para facilitar el uso, sobre todo en tiempo de compilación, por lo que 
   damos prioridad a los literales puedan ser constexpr.
   
   - El tratamiento de errores será basado en `std::option`, `std::expected` y si es necesario y su finalidad 
   es el cálculo, sin embargo cuando la finalidad sea I/O preferiremos trabajaremos con `try`, `catch`, `throw` 
   y excepciones en general. En general, funciones puras `noexcept`.
   
   - Soporte completo de E/S para tipos `__int128`, para tipos -`using mp = boost::multiprecision´- 
   `mp::int128_t`, `mp::int256_t`, `mp::int512_t`, `mp::int1024_t`, `mp::int2048_t`, y sus respectivos 
   `unsigned` y los tipos `mp::cpp_int`, `mp::tom_int` y `mp::mpz_int`. Funciones `to_string`, `from_string` 
   y `to_cstr` y `from_cstr` para trabajar con arrays `char[]` para funciones `constexpr`.
   
   - El tratamiento de E/S se hará tanto con codificación `ASCII` como con `UTF-8`, o incluso mejor `UTF-32` 
   para evitar la variabilidad de la longitud de caracteres. En el caso `ASCII`, se tendrá cuidado que los 
   caracteres que serán de alfabeto español, salgan bien en terminales.
   
   - La documentación será profusa, y en el código usaremos `doxygen` para todas las funciones y, cuando se 
   trate de matemáticas, explicaciones didácticas incluso.
   
   - Importante: Deberían poder pasarse las propiedades de las funciones matemáticas núcleares a comprobar 
   en tests de estas funciones en algún tipo de comentario `doxygen` (no sé si habrá un método mejor, ya que 
   no conozco `Catch2` ni ninguna biblioteca de test unitarios ni otros métodos de comentario automatizado como 
   `doxygen`). Igualmente, me gustaría poder pasar sugerencias de optimización a través de estos comentarios en 
   esas funciones. Igualmente en el comentario `doxygen` del archivo debería poder sugerir nuevas funciones a 
   implementar.
   
   - En general prefiero snake_case a pascal_case. No tengo claro aún una convención de nombres para las funciones,
   archivos cabecera, y los distintos `namespace`.
   
   - Datos de la instalación: MSYS2 en `"/c/msys64/ucrt64"`, y VC en `"d:\\program files\\Microsoft Visual Studio\\"`

2. **Sistema de Tipos Avanzado**

   - Traits para detectión automática de tipos numéricos

   - Soporte para Boost.Multiprecision

   - Integración con tipos estándar y extendidos


3. **Aplicaciones de Demostración**

   - Demo principal interactivo.
   
   - Demo de funciones sencillas: potencias de base enteras y exponente entero, logaritmos de base entera de un 
   entero, algoritmos de euclides, máximo común múltiple, mínimo común divisor, función operador sumatorio (solo 
   enteros por ahora) a la que se le pasa la función interna al sumatorio y los límites de sumación, factoriales, 
   números binomiales, función operador productorio, números de Bernoulli. 
   
   - Demo de optimización de factoriales, demo de factoriales muy grandes.

   - Suite completa (muy y profusamente completa) de pruebas unitarias, con Catch2 o con la librería de Boost.
   Habrá que decidirlo.
   
   - Pruebas para medición de tiempos y ocupación en memoria, benchmarks, con salidas a formatos markdown y csv.


### Requisitos Técnicos


1. **Multi-Compilador**

   - GCC (MSYS2/MinGW)

   - Clang (MSYS2)

   - MSVC (Visual Studio 2022)


2. **Multi-Estándar**

   - C++17, C++20, C++23


3. **Sistema de Build Robusto**

   - CMake con Ninja

   - Scripts de build automatizados

   - **Directorios separados por configuración**: `build_[compilador]_[tipo]_cpp[estándar]`

   - Ejemplos: `build_gcc_debug_cpp17`, `build_clang_release_cpp20`, `build_msvc_release_cpp23`


4. **Gestión de Dependencias**

   - Boost (via vcpkg para MSVC, sistema para GCC/Clang)

   - Catch2 (como dependencia de CMake)

   - FetchContent para dependencias automáticas


### Estructura de Archivos Requerida


```

NumerosYCalculos/

├── CMakeLists.txt                         # Build system principal

├── README.md                              # Documentación

├── include/                               # Estructura por directorios como si fuesen 
                                           # módulos en Python o Rust
│   ├── combinatorics.hpp                  # Header principal de la biblioteca

│   ├── constexpr_literals.hpp             # Literales constexpr y funciones

│   ├── int128_io.hpp                      # E/O completa para __int128

│   └── number_extended_type_traits.hpp             # Traits de tipos numéricos

├── src/                                   # Estructura por directorios como si fuesen 
                                           # módulos en Python o Rust
│   ├── main.cpp                           # Demo principal interactivo

│   └── combinatorics.cpp                  # Implementación de la biblioteca

├── tests/

│   └── test_combinatorics.cpp             # Suite de pruebas unitarias

├── benchmarks/

│   └── benchmarks.cpp                     # Suite de funciones de medida

├── doc/

│   └──                                    # Documentación resultante markdown, latex, html
                                           # y resultados de benchmarks (y csv) por directorios.

├── factorial_optimization_demo.cpp        # Demo de optimizaciones

└── scripts de build/

    ├── build_multi_final.bat             # Script Windows multi-directorio

    └── build_simple.sh                   # Script bash corregido

└── build/                                # ¿podría estar en un directorio distinto?

    ├── gcc_debug 
	├── gcc_release 
    ├── clang_debug 
    ├── clang_release 
    ├── msvc_debug 
    └── msvc_release 

```


### Características Específicas de Implementación


#### 1. Literales Constexpr Avanzados

```cpp

// Debe permitir uso como:

auto fact = 20_i128;

auto power = 10_pow2_i128;  // 2^10

auto combo = constexpr_combination(50_i128, 25_i128);

```


#### 2. Sistema de E/S Completo para __int128

```cpp

// Debe soportar:

__int128 big_number = 123456789012345678901234567890_i128;

std::cout << big_number << std::endl;

std::cin >> big_number;

std::string str = int128_io::to_string(big_number);

```


#### 3. Traits de Detección Automática

```cpp

// Debe detectar automáticamente tipos numéricos:

template<typename T>

constexpr bool is_extended_integer_v = /* detección automática */;

```


#### 4. CMakeLists.txt Multi-Compilador

- Detección automática de compilador

- Configuración específica por compilador

- Integración con vcpkg (MSVC) y sistema (GCC/Clang)

- FetchContent para Catch2


### Scripts de Build Requeridos


#### 1. Script Multi-Directorio (build_multi_final.bat) - PRINCIPAL

```bash

# Debe permitir:

./build_multi_final.bat gcc debug 17      # → build_gcc_debug_cpp17/

./build_multi_final.bat clang release 20  # → build_clang_release_cpp20/

./build_multi_final.bat msvc release 23   # → build_msvc_release_cpp23/

```


#### 2. Script Bash No-Bloqueante (build_simple.sh) - SECUNDARIO

- **CRÍTICO**: No debe colgarse en "STOP 1"

- Debe terminar en segundos, no minutos

- Evitar funciones complejas de detección de versiones

- **Redirección inteligente**: Si no encuentra herramientas o se solicita MSVC, debe redirigir al script.bat

- Manejo simple y directo de dependencias

- **Nota**: MSVC no es compatible con scripts bash - debe redirigir al .bat


### Problemas Conocidos a Evitar


1. **Script Bash Colgado**

   - No usar subshells complejos para detección de versiones

   - Evitar funciones que causen deadlocks en MSYS2

   - Implementar timeouts y salidas de emergencia


2. **Dependencias de Boost**

   - MSVC requiere vcpkg

   - GCC/Clang usan instalación del sistema

   - Manejar graciosamente la ausencia de dependencias


3. **Compatibilidad Multi-Compilador**

   - Warnings específicos por compilador

   - Flags de compilación diferenciados

   - Rutas de herramientas específicas


### Resultados Esperados


Al final del proyecto debe:


1. **Compilar exitosamente** en todas las configuraciones

2. **Ejecutar pruebas** sin errores

3. **Generar directorios separados** por configuración

4. **Scripts funcionales** que no se cuelguen

5. **Demos interactivos** que muestren las capacidades


### Comandos de Verificación


```bash

# Script principal .bat debe funcionar sin colgarse:

./build_multi_final.bat gcc debug 17

./build_multi_final.bat clang release 20  

./build_multi_final.bat msvc release 23


# Script bash debe terminar rápido con redirección inteligente:

./build_simple.sh gcc debug 17     # → Redirige a .bat si no encuentra herramientas

./build_simple.sh msvc debug 17    # → Redirige a .bat (MSVC no compatible)


# Debe generar estos directorios:

build_gcc_debug_cpp17/

build_clang_release_cpp20/

build_msvc_release_cpp23/

```


### Lecciones Aprendidas y Limitaciones


#### Contextos de Ejecución en Windows:

1. **PowerShell/CMD**: Ideal para MSVC y herramientas Windows nativas

2. **MSYS2 Terminal**: Ideal para GCC/Clang con herramientas Unix-like  

3. **Bash del Sistema**: Limitado, no tiene acceso completo a herramientas MSYS2


#### Estrategia de Scripts:

- **Script .bat como principal**: Funciona en todos los contextos de Windows

- **Script .sh como secundario**: Con redirección inteligente al .bat cuando sea necesario

- **Detección de limitaciones**: El script bash debe detectar cuando no puede funcionar y redirigir


#### Compatibilidad de Compiladores:

- **GCC/Clang**: Funcionan mejor desde MSYS2, pero también desde .bat

- **MSVC**: Solo funciona correctamente desde PowerShell/CMD con vcvars


### Notas Importantes


- **Prioridad máxima**: Sistema multi-directorio funcional

- **Prioridad alta**: Scripts que no se cuelguen + redirección inteligente

- **Foco técnico**: __int128, constexpr, noexcept, funciones `gcd`, `lcm`, combinatoria avanzada  

- **Compatibilidad**: Windows con MSYS2, Visual Studio, CLion y herramientas nativas

- **Robustez**: Scripts deben manejar graciosamente las limitaciones del entorno
