# src/CMakeLists.txt
#
# Objetivo:
# Este CMakeLists define los ejecutables de demostración (como 'demo_main')
# que se compilan desde los archivos .cpp en este directorio.

# 1. Demo Principal
# --------------------------------
# Definimos el ejecutable 'demo_main' a partir de 'main.cpp'
add_executable(demo_main
    main.cpp
)

# Enlazamos la demo con nuestra biblioteca principal ('NumerosYCalculos_lib')
# para que 'main.cpp' pueda usar los headers de 'include/'.
# 'NumerosYCalculos_lib' ya tiene los include_directories correctos
# (definidos en el CMakeLists.txt raíz).
target_link_libraries(demo_main
    PRIVATE
    NumerosYCalculos_lib
)

# Nos aseguramos de que esta demo se compile con el estándar de C++
# que se pasó al script de build (ej. 17, 20, 23).
set_target_properties(demo_main PROPERTIES
    CXX_STANDARD ${CMAKE_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# --------------------------------
# (Aquí añadiremos más demos a medida que las creemos,
#  como 'factorial_optimization_demo' cuando creemos ese archivo)

