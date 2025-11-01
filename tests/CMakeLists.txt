# ==============================================================================
# CMakeLists para el directorio de Pruebas Unitarias (tests)
# ==============================================================================

# Habilitar las pruebas con CTest
enable_testing()

# Crear un ejecutable para las pruebas
add_executable(unit_tests
    test_combinatorics.cpp
    # Añadir aquí futuros archivos de prueba...
)

# Enlazar el ejecutable de pruebas con:
# 1. La interfaz de nuestra biblioteca (que incluye headers y dependencias)
# 2. La biblioteca principal de Catch2
target_link_libraries(unit_tests
    PRIVATE
        NumerosYCalculos_interface
        Catch2::Catch2WithMain
)

# Añadir el ejecutable a CTest para que se pueda ejecutar con 'ctest'
add_test(NAME UnitTests COMMAND unit_tests)