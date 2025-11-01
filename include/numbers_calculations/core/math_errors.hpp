#pragma once

/* ==============================================================================
 * Archivo: math_errors.hpp
 * Autor:   Gemini
 *
 * Objetivo:
 * Define el sistema de manejo de errores para funciones de cálculo puras,
 * tal como se especifica en "gemini.md".
 *
 * Implementa:
 * - Un `enum class` para errores granulares.
 * - Una implementación de `std::expected` compatible con C++17.
 * ==============================================================================
 */

// tl::expected es la implementación de referencia para std::expected
// compatible con C++17. La añadiremos vía FetchContent en el CMakeLists.txt
// raíz.
#include <tl/expected.hpp>

namespace numbers_calculations::core {

/**
 * @brief Define los posibles errores que pueden ocurrir en
 * funciones de cálculo puras (no de E/S).
 */
enum class MathError {
  NoError,
  Overflow,
  Underflow,
  DivisionByZero,
  DomainError // Ej. log(-1), sqrt(-1), factorial(-1)
};

/**
 * @brief Alias global para el tipo `expected`.
 *
 * Usaremos esto en lugar de std::optional para poder devolver
 * la *razón* por la que un cálculo falló.
 *
 * Uso:
 * Expected<int, MathError> mi_funcion(int n);
 *
 * if (auto resultado = mi_funcion(10)) {
 * // Éxito: resultado.value() contiene el 'int'
 * } else {
 * // Fracaso: resultado.error() contiene el 'MathError'
 * }
 */
template <class T, class E = MathError> using Expected = tl::expected<T, E>;

// Helper para crear un error inesperado
template <class E = MathError> using Unexpected = tl::unexpected<E>;

} // namespace numbers_calculations::core
