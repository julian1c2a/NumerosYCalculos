#!/bin/bash

# ---
# --- Script de Build Secundario (Bash/MSYS2)
# --- Este script es una envoltura ligera.
# ---
# --- Logica de Redireccion Inteligente (segun gemini.md):
# --- 1. Si se pide MSVC, redirige a .bat (MSVC no es compatible con Bash).
# --- 2. Si se pide GCC/Clang pero NO se encuentran (ej. Bash estandar vs MSYS2),
# ---    redirige a .bat (que puede encontrar MSYS2 en el PATH).
# --- 3. Si se pide GCC/Clang y SE encuentran, compila directamente.
# ---

# --- 1. Validacion de Argumentos ---
if [ "$#" -ne 3 ]; then
    echo "[ERROR] Uso: $0 [gcc|clang|msvc] [debug|release] [17|20|23]"
    echo "[INFO] Ejemplo: $0 gcc debug 17"
    exit 1
fi

COMPILER="$1"
BUILD_TYPE="$2"
CXX_STD="$3"
BUILD_TYPE_CMAKE=$(echo "$BUILD_TYPE" | awk '{print ($0 == "debug") ? "Debug" : "Release"}')

# Directorio del script y raiz del proyecto
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
PROJECT_ROOT=$( cd -- "$SCRIPT_DIR/.." &> /dev/null && pwd )

echo "[CONFIG] Compilador:   $COMPILER"
echo "[CONFIG] Tipo:         $BUILD_TYPE (CMake: $BUILD_TYPE_CMAKE)"
echo "[CONFIG] Estandar C++: $CXX_STD"

# --- 2. Logica de Redireccion Inteligente ---

# 2a: Redirigir MSVC siempre
if [[ "$COMPILER" == "msvc" ]]; then
    echo "[REDIRECT] MSVC no es compatible con el entorno Bash/MSYS2."
    echo "[REDIRECT] Ejecutando el script .bat principal..."
    echo "-----------------------------------------------------------------"
    # Ejecutar el script .bat (necesita sintaxis de ruta de Windows)
    cmd.exe /C "cd $PROJECT_ROOT && .\\scripts\\build_multi_final.bat $COMPILER $BUILD_TYPE $CXX_STD"
    exit $?
fi

# 2b: Comprobar si GCC/Clang existen
CXX_COMPILER="g++"
if [[ "$COMPILER" == "clang" ]]; then
    CXX_COMPILER="clang++"
fi

if ! command -v $CXX_COMPILER &> /dev/null; then
    echo "[REDIRECT] No se encontro '$CXX_COMPILER' en el PATH de este entorno Bash."
    echo "[REDIRECT] (Asegurate de estar en una terminal MSYS2/UCRT64, no en Git Bash)."
    echo "[REDIRECT] Intentando ejecutar el script .bat como alternativa..."
    echo "-----------------------------------------------------------------"
    cmd.exe /C "cd $PROJECT_ROOT && .\\scripts\\build_multi_final.bat $COMPILER $BUILD_TYPE $CXX_STD"
    exit $?
fi

# --- 3. Compilacion Nativa en Bash (Si las comprobaciones pasan) ---
echo "[INFO] Compilador '$CXX_COMPILER' encontrado. Procediendo con la compilacion en Bash."
echo "-----------------------------------------------------------------"

BUILD_DIR="build_${COMPILER}_${BUILD_TYPE}_cpp${CXX_STD}"
BUILD_PATH="$PROJECT_ROOT/$BUILD_DIR"

# --- 4. Ejecutar CMake (Configuracion) ---
cmake -G Ninja \
    -S "$PROJECT_ROOT" \
    -B "$BUILD_PATH" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE_CMAKE" \
    -DCMAKE_CXX_STANDARD="$CXX_STD"

if [ $? -ne 0 ]; then
    echo "[ERROR] Fallo la configuracion de CMake."
    exit 1
fi

# --- 5. Ejecutar CMake (Compilacion) ---
cmake --build "$BUILD_PATH"

if [ $? -ne 0 ]; then
    echo "[ERROR] Fallo la compilacion."
    exit 1
fi

echo "[SUCCESS] Compilacion completada en: $BUILD_DIR"
exit 0

# --- EOF ---