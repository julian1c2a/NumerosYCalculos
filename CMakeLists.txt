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

# ==============================================================================
# GESTIÓN DE DEPENDENCIAS (FetchContent)
# ==============================================================================
include(FetchContent)

# 1. Catch2 (para Pruebas Unitarias)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# 2. tl::expected (Para manejo de errores en C++17) - NUEVO
FetchContent_Declare(
    tl_expected
    GIT_REPOSITORY https://github.com/TartanLlama/expected.git
    GIT_TAG        v1.1.0
)
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
    check_type_size("__int128" SIZEOF_INT128)
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

# Añadimos el directorio 'include' globalmente
target_include_directories(numeros_calculos_interface
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Añadimos los directorios de las dependencias
target_include_directories(numeros_calculos_interface
    INTERFACE
        ${catch2_SOURCE_DIR}/src # Para Catch2
        ${tl_expected_SOURCE_DIR}/include # Para tl::expected
)

# Procesar los subdirectorios
add_subdirectory(src)
add_subdirectory(tests)
# add_subdirectory(benchmarks)

message(STATUS "Configuración de CMake completada.")
