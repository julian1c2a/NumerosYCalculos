#pragma once

/* ==============================================================================
 * Archivo: combinatorics.hpp
 * Autor:   Gemini
 *
 * Objetivo:
 * Implementa funciones matemáticas de combinatoria (factorial, etc.)
 *
 * @todo_feature Implementar combinaciones y permutaciones.
 * @todo_feature Implementar lookup_table para factoriales pequeños (n < 20).
 * ==============================================================================
 */

#include "core/extended_type_traits.hpp" // Para is_integer_v
#include "core/math_errors.hpp"          // Para Expected y MathError (NUEVO)
#include <limits>                        // Para numeric_limits
#include <numbers_calculations/core/extended_type_traits.hpp> // Para enable_if_t y is_signed_v


namespace numbers_calculations::math {

namespace internal {
// Implementación 'constexpr' de factorial
template <typename T>
constexpr core::Expected<T> constexpr_factorial(T n) noexcept {
  // SFINAE ya ha comprobado que T es un entero

  // 1. Comprobación de Dominio (DomainError)
  if (n < 0) {
    return core::Unexpected(core::MathError::DomainError);
  }

  T result{1};
  T i{1};

  while (i <= n) {
    // 2. Comprobación de Overflow
    // (result * i) > max  <=>  result > (max / i)
    if (i > 1 && result > (std::numeric_limits<T>::max() / i)) {
      return core::Unexpected(core::MathError::Overflow);
    }

    result *= i;
    ++i;
  }

  return result; // Éxito
}
} // namespace internal

/**
 * @brief Calcula el factorial de un número entero (n!).
 *
 * Es una función genérica que funciona con cualquier tipo T
 * detectado por `core::is_integer_v` (ej. int, long, __int128, cpp_int).
 *
 * @tparam T Tipo numérico entero.
 * @param n El número (debe ser no negativo).
 * @return Un `core::Expected<T, core::MathError>`:
 * - .value() si el cálculo es exitoso.
 * - .error() (MathError::DomainError) si n < 0.
 * - .error() (MathError::Overflow) si el resultado
 * excede std::numeric_limits<T>::max().
 *
 * @test_property factorial(0) == 1
 * @test_property factorial(1) == 1
 * @test_property factorial(5) == 120
 * @test_property factorial(20) == 2432902008176640000
 * @test_property factorial(-1) == MathError::DomainError
 * @test_property factorial(21) (para uint64_t) == MathError::Overflow
 * @test_property factorial(35) (para int128_t) == MathError::Overflow
 *
 * @optimize_note Considerar un lookup_table para n < 20.
 */
template <typename T, std::enable_if_t<core::is_integer_v<T>, int> = 0>
constexpr core::Expected<T> factorial(T n) noexcept {

  // Para tipos con signo, n < 0 es un error de dominio.
  if constexpr (std::is_signed_v<T>) {
    if (n < 0) {
      return core::Unexpected(core::MathError::DomainError);
    }
  }
  // Para tipos sin signo, n < 0 es imposible (se convierte en un
  // número positivo grande), por lo que el chequeo de overflow
  // debería detectarlo si n es muy grande.

  return internal::constexpr_factorial(n);
}

} // namespace numbers_calculations::math
