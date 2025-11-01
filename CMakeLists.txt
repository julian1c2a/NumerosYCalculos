cmake_minimum_required(VERSION 3.15)
project(NumerosYCalculos VERSION 1.0
      DESCRIPTION "Biblioteca de algoritmos matemáticos avanzados en C++"
      LANGUAGES CXX)

# ==============================================================================
# CONFIGURACIÓN GLOBAL DEL PROYECTO
# ==============================================================================
message(STATUS "Configurando proyecto: ${PROJECT_NAME} v${PROJECT_VERSION}")

# CMAKE_CXX_STANDARD se pasará desde el script de build (ej. -DCMAKE_CXX_STANDARD=17)
if(NOT CMAKE_CXX_STANDARD)
    message(STATUS "No se especificó CMAKE_CXX_STANDARD, usando 17 por defecto.")
    set(CMAKE_CXX_STANDARD 17)
endif()
message(STATUS "Usando C++ Estándar: ${CMAKE_CXX_STANDARD}")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Política para FetchContent (evita warning CMP0135)
set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)


# ==============================================================================
# GESTIÓN DE DEPENDENCIAS (FetchContent)
# ==============================================================================
include(FetchContent)

# --- Patrón Find-Or-Fetch para Dependencias ---
# Primero, intentamos encontrar el paquete. Si no está, lo descargamos.

# 1. Catch2 (para Pruebas Unitarias)
find_package(Catch2 3 QUIET)
if(NOT Catch2_FOUND)
    message(STATUS "Catch2 no encontrado. Descargando con FetchContent...")
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.2
    )
    # Deshabilitamos los tests de Catch2, no los necesitamos
    set(CATCH_BUILD_TESTING OFF CACHE BOOL "Disable Catch2's own tests")
    FetchContent_MakeAvailable(Catch2)
endif()

# 2. tl::expected (Para manejo de errores en C++17)
# tl::expected es una biblioteca de solo cabeceras, por lo que el impacto
# de FetchContent es mínimo. Lo mantenemos como está por simplicidad.
FetchContent_Declare(tl_expected
    GIT_REPOSITORY https://github.com/TartanLlama/expected.git
    GIT_TAG v1.1.0 EXCLUDE_FROM_ALL CMAKE_ARGS -DEXPECTED_BUILD_TESTS=OFF)
FetchContent_MakeAvailable(tl_expected)

# ==============================================================================
# CONFIGURACIÓN DEL COMPILADOR (Flags y Warnings)
# ==============================================================================

if(MSVC)
    # Flags para MSVC (Visual Studio)
    # /W4 (Nivel de warning alto), /WX (Warnings como errores en Release)
    # /permissive- (Modo estándar estricto)
    add_compile_options(/W4 /permissive-)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/WX)
    endif()
    
    # MSVC necesita que le digamos que use UTF-8 para los fuentes
    add_compile_options(/utf-8)

else()
    # Flags para GCC y Clang
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-Werror)
    endif()

    # --- Manejo de __int128 ---
    # Detectar si el compilador soporta __int128
    include(CheckTypeSize)
    check_type_size("__int128" SIZEOF_INT128 LANGUAGE CXX)
    if(SIZEOF_INT128 MATCHES "16")
        message(STATUS "Compilador soporta __int128 (16 bytes).")
        add_definitions(-DHAS_NATIVE_INT128=1)
    else()
        message(STATUS "Compilador NO soporta __int128.")
        add_definitions(-DHAS_NATIVE_INT128=0)
    endif()
endif()

# ==============================================================================
# INCLUDES Y SUBDIRECTORIOS
# ==============================================================================

# Creamos una biblioteca INTERFACE para agrupar los includes globales
add_library(numbers_calculations_interface INTERFACE)

# Añadimos los directorios de inclusión a nuestra biblioteca de interfaz.
# Marcamos 'include' como SYSTEM para poder usar <numbers_calculations/...>
target_include_directories(numbers_calculations_interface
    SYSTEM INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/internal> # Para detalles de implementación
        $<BUILD_INTERFACE:${Catch2_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${tl_expected_SOURCE_DIR}/include>
)

# Procesar los subdirectorios
add_subdirectory(src)
add_subdirectory(tests)
# add_subdirectory(benchmarks)

message(STATUS "Configuración de CMake completada.")
