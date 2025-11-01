@echo off
setlocal enabledelayedexpansion

rem ---
rem --- Script de Build Multi-Configuracion para NumerosYCalculos
rem --- Uso: build_multi_final.bat [compilador] [tipo] [estandar_cpp]
rem ---
rem --- Argumentos:
rem ---   [compilador]: gcc, clang, msvc
rem ---   [tipo]:       debug, release
rem ---   [estandar_cpp]: 17, 20, 23
rem ---
rem --- Ejemplo: .\scripts\build_multi_final.bat gcc debug 17
rem --- Ejemplo: .\scripts\build_multi_final.bat msvc release 23
rem ---

rem --- 1. Validacion de Argumentos ---
if "%~1"=="" (
    echo [ERROR] No se especifico compilador. Opciones: gcc, clang, msvc
    goto :usage
)
if "%~2"=="" (
    echo [ERROR] No se especifico tipo de build. Opciones: debug, release
    goto :usage
)
if "%~3"=="" (
    echo [ERROR] No se especifico estandar de C++. Opciones: 17, 20, 23
    goto :usage
)

set "COMPILER=%~1"
set "BUILD_TYPE=%~2"
set "CXX_STD=%~3"

set "BUILD_TYPE_CMAKE=Release"
if /i "%BUILD_TYPE%"=="debug" (
    set "BUILD_TYPE_CMAKE=Debug"
)


rem --- 2. Definir Directorio de Build (basado en gemini.md) ---
set "BUILD_DIR=build/%COMPILER%_%BUILD_TYPE%_cpp%CXX_STD%"
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."

rem --- 3. Configuracion especifica del Compilador ---

rem --- NUEVO: Manejo del comando 'clean' ---
if /i "%~4"=="clean" (
    echo [CLEAN] Borrando el directorio de build: %BUILD_DIR%
    if exist "%PROJECT_ROOT%/%BUILD_DIR%" (
        rmdir /s /q "%PROJECT_ROOT%/%BUILD_DIR%"
        echo [CLEAN] Directorio borrado.
    ) else (
        echo [CLEAN] El directorio no existia. No se necesita limpieza.
    )
)

set "CMAKE_ARGS=-G "MinGW Makefiles" -S "%PROJECT_ROOT%" -B "%PROJECT_ROOT%/%BUILD_DIR%""
set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_BUILD_TYPE=%BUILD_TYPE_CMAKE%"

echo [CONFIG] Compilador:   %COMPILER%
echo [CONFIG] Tipo:         %BUILD_TYPE% (CMake: %BUILD_TYPE_CMAKE%)
echo [CONFIG] Estandar C++: %CXX_STD%
echo [CONFIG] Directorio:   %BUILD_DIR%
echo -----------------------------------------------------------------

if "%COMPILER%"=="gcc" (
    rem --- Config GCC (MSYS2/UCRT64) ---
    set "UCRT64_HOME=C:\msys64\ucrt64"
    set "PATH=%UCRT64_HOME%\bin;%PATH%"
    set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_C_COMPILER=gcc.exe"
    set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_CXX_COMPILER=g++.exe"
    set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_CXX_STANDARD=%CXX_STD%"
    goto :configure

) else if "%COMPILER%"=="clang" (
    rem --- Config Clang (MSYS2/UCRT64) ---
    set "UCRT64_HOME=C:\msys64\ucrt64"
    set "PATH=%UCRT64_HOME%\bin;%PATH%"
    set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_C_COMPILER=clang.exe"
    set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_CXX_COMPILER=clang++.exe"
    set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_CXX_STANDARD=%CXX_STD%"
    goto :configure

) else if "%COMPILER%"=="msvc" (
    rem --- Config MSVC (Visual Studio 2022) ---
    rem (Correccion C++23 -> /std:c++latest)
    
    if "%CXX_STD%"=="23" (
        set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_CXX_FLAGS="/std:c++latest""
    ) else (
        set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_CXX_STANDARD=%CXX_STD%"
    )
    
    rem --- Encontrar y llamar a vcvarsall.bat ---
    set "VSWHERE_PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
    if not exist "%VSWHERE_PATH%" (
        echo [ERROR] No se encontro 'vswhere.exe'. No se puede configurar el entorno MSVC.
        goto :eof
    )
    
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE_PATH%" -latest -property installationPath`) do (
        set "VS_INSTALL_PATH=%%i"
    )
    
    set "VCVARS_PATH=%VS_INSTALL_PATH%\VC\Auxiliary\Build\vcvarsall.bat"
    if not exist "%VCVARS_PATH%" (
        echo [ERROR] No se encontro 'vcvarsall.bat' en la ruta de VS: %VCVARS_PATH%
        echo [DEBUG] (Ruta de VS detectada: %VS_INSTALL_PATH%)
        goto :eof
    )
    
    echo [INFO] Cargando entorno MSVC desde: %VCVARS_PATH%
    call "%VCVARS_PATH%" x64
    
    rem --- Configurar vcpkg (para Boost) ---
    set "VCPKG_TOOLCHAIN_FILE=%PROJECT_ROOT%\vcpkg\scripts\buildsystems\vcpkg.cmake"
    if exist "%VCPKG_TOOLCHAIN_FILE%" (
        echo [INFO] Usando toolchain de vcpkg: %VCPKG_TOOLCHAIN_FILE%
        set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_TOOLCHAIN_FILE=%VCPKG_TOOLCHAIN_FILE%"
    ) else (
        echo [WARN] vcpkg no encontrado en %VCPKG_TOOLCHAIN_FILE%. La compilacion de Boost puede fallar.
    )

    goto :configure

) else (
    echo [ERROR] Compilador '%COMPILER%' no reconocido.
    goto :usage
)

:configure
rem --- 4. Ejecutar CMake (Configuracion) ---
echo. 
echo [CMD] cmake %CMAKE_ARGS%
cmake %CMAKE_ARGS%
if %errorlevel% neq 0 (
    echo [ERROR] Fallo la configuracion de CMake.
    goto :eof
)

rem --- 5. Ejecutar CMake (Compilacion) ---
echo.
echo [CMD] cmake --build "%PROJECT_ROOT%/%BUILD_DIR%"
cmake --build "%PROJECT_ROOT%/%BUILD_DIR%"
if %errorlevel% neq 0 (
    echo [ERROR] Fallo la compilacion.
    goto :eof
)

echo.
echo [SUCCESS] Compilacion completada en: %BUILD_DIR%
goto :eof

:usage
echo.
echo Uso: %~n0 [gcc^|clang^|msvc] [debug^|release] [17^|20^|23] [clean]
echo Ejemplo: %~n0 gcc debug 17
goto :eof
