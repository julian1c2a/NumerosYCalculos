#pragma once

/* ==============================================================================
 * Archivo: combinatorics.hpp (antes factorial.hpp)
 * Autor:   Gemini
 *
 * Objetivo:
 * Implementa funciones matemáticas de combinatoria (factorial, etc.)
 *
 * @todo_feature Implementar combinaciones y permutaciones.
 * @todo_feature Implementar lookup_table para factoriales pequeños (n < 34).
 * (¡HECHO!)
 * ==============================================================================
 */

#include "internal/factorial_lookup_table.hpp" // <-- CORREGIDO: Ruta relativa a internal/
#include <limits>                              // Para numeric_limits
#include <numbers_calculations/core/extended_extended_type_traits.hpp> // Para is_supported_integer_v
#include <numbers_calculations/core/extended_type_traits.hpp> // Para enable_if_t y is_signed_v
#include <numbers_calculations/core/math_errors.hpp> // Para Expected y MathError


namespace numbers_calculations::math {

namespace internal {
// Implementación 'constexpr' de factorial
template <typename T>
constexpr core::Expected<T> constexpr_factorial(T n) noexcept {

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
 * detectado por `core::is_supported_integer_v` (ej. int, long, __int128,
 * cpp_int).
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
 * @optimize_note Usa una lookup_table para n < 34.
 */
template <typename T,
          std::enable_if_t<core::is_supported_integer_v<T>, int> = 0>
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

  // --- Dispatcher de optimización: Usar LUT si es posible ---
  if (n < internal::FACTORIALS_LUT.size()) {
    const auto lut_value = internal::FACTORIALS_LUT[n];

    // Comprobar si el valor de la LUT cabe en el tipo de retorno T
    if constexpr (std::numeric_limits<T>::is_bounded &&
                  sizeof(T) < sizeof(core::uint128_t)) {
      if (lut_value >
          static_cast<core::uint128_t>(std::numeric_limits<T>::max())) {
        return core::Unexpected(core::MathError::Overflow);
      }
    }
    return static_cast<T>(lut_value);
  }

  // --- Fallback a algoritmo genérico para n >= 34 ---
  return internal::constexpr_factorial(static_cast<T>(n));
}

/**
 * @brief Calcula el número de permutaciones (P(n, k) = n! / (n-k)!).
 *
 * Es una función genérica que funciona con cualquier tipo T
 * detectado por `core::is_supported_integer_v`.
 *
 * @optimize_note Implementado como un bucle `n * (n-1) * ... * (n-k+1)`
 *                para evitar el cálculo de factoriales grandes y prevenir
 *                overflows intermedios.
 *
 * @tparam T Tipo numérico entero.
 * @param n Número total de elementos.
 * @param k Número de elementos a elegir.
 * @return Un `core::Expected<T, core::MathError>`:
 * - .value() si el cálculo es exitoso.
 * - .error() (MathError::DomainError) si n < 0, k < 0, o k > n.
 * - .error() (MathError::Overflow) si el resultado excede el máximo de T.
 */
template <typename T,
          std::enable_if_t<core::is_supported_integer_v<T>, int> = 0>
constexpr core::Expected<T> permutations(T n, T k) noexcept {
  if constexpr (std::is_signed_v<T>) {
    if (n < 0 || k < 0) {
      return core::Unexpected(core::MathError::DomainError);
    }
  }
  if (k > n) {
    return core::Unexpected(core::MathError::DomainError);
  }
  if (k == 0) {
    return T{1};
  }

  T result{1};
  for (T i = 0; i < k; ++i) {
    T term = n - i;
    // Comprobación de overflow: result * term > max
    if (result > std::numeric_limits<T>::max() / term) {
      return core::Unexpected(core::MathError::Overflow);
    }
    result *= term;
  }

  return result;
}

/**
 * @brief Calcula el número de combinaciones (C(n, k) = n! / (k! * (n-k)!)).
 *
 * Es una función genérica que funciona con cualquier tipo T
 * detectado por `core::is_supported_integer_v`.
 *
 * @optimize_note Implementado como (n * (n-1) * ... * (n-k+1)) / k!
 *                para evitar el cálculo de factoriales grandes. También usa
 *                la propiedad C(n, k) = C(n, n-k) para minimizar cálculos.
 *
 * @tparam T Tipo numérico entero.
 * @param n Número total de elementos.
 * @param k Número de elementos a elegir.
 * @return Un `core::Expected<T, core::MathError>`:
 * - .value() si el cálculo es exitoso.
 * - .error() (MathError::DomainError) si n < 0, k < 0, o k > n.
 * - .error() (MathError::Overflow) si el resultado excede el máximo de T.
 */
template <typename T,
          std::enable_if_t<core::is_supported_integer_v<T>, int> = 0>
constexpr core::Expected<T> combinations(T n, T k) noexcept {
  if constexpr (std::is_signed_v<T>) {
    if (n < 0 || k < 0) {
      return core::Unexpected(core::MathError::DomainError);
    }
  }
  if (k > n) {
    return core::Unexpected(core::MathError::DomainError);
  }

  // Optimización: C(n, k) == C(n, n-k). Calculamos con el k más pequeño.
  if (k > n / 2) {
    k = n - k;
  }

  if (k == 0) {
    return T{1};
  }

  // La fórmula es P(n, k) / k!
  // Lo calculamos de forma iterativa para mantener los números más pequeños
  // y evitar overflow: (n/1) * ((n-1)/2) * ...
  T result{1};
  for (T i = 1; i <= k; ++i) {
    T term = n - i + 1;
    // Comprobación de overflow antes de la multiplicación
    // result * term > max  =>  result > max / term
    if (result > std::numeric_limits<T>::max() / term) {
      return core::Unexpected(core::MathError::Overflow);
    }
    result *= term;
    result /= i;
  }

  return result;
}

} // namespace numbers_calculations::math
