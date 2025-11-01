#pragma once

/* ==============================================================================
 * Archivo: constexpr_literals.hpp
 * Autor:   Gemini
 *
 * Objetivo:
 * Define literales de usuario `constexpr` para crear tipos enteros extendidos
 * de forma intuitiva, como `123_i128` o `10_pow2_ui128`.
 *
 * Cumple con la especificación de "gemini.md":
 * - Literales para `__int128` y `__uint128_t`.
 * - Literales para potencias de 2.
 * - Totalmente `constexpr` para uso en tiempo de compilación.
 *
 * @todo_feature Añadir literales para tipos de Boost.Multiprecision.
 * ==============================================================================
 */
#include <numbers_calculations/core/extended_type_traits.hpp> // Para int128_t, uint128_t
#include <stdexcept> // Para std::domain_error en el parser

namespace numbers_calculations::core {

namespace internal {

/**
 * @brief Parsea una cadena de caracteres a un tipo entero en tiempo de
 * compilación.
 * @note Lanza una excepción si hay caracteres no válidos, lo que detiene
 * la compilación si se usa en un contexto `constexpr`.
 */
template <typename T> constexpr T constexpr_string_to_int(const char *str) {
  T result = 0;
  const char *p = str;
  bool is_negative = false;

  if (*p == '-') {
    is_negative = true;
    ++p;
  }

  while (*p != '\0') {
    if (*p >= '0' && *p <= '9') {
      result = result * 10 + (*p - '0');
    } else {
      // Esto causará un error de compilación si se usa en un contexto
      // `constexpr` con una cadena inválida.
      throw std::domain_error("Caracter no válido en literal numérico");
    }
    ++p;
  }

  return is_negative ? -result : result;
}

} // namespace internal

inline namespace literals {

#if HAS_NATIVE_INT128

/**
 * @brief Literal de usuario para crear un `int128_t` a partir de una cadena.
 * @example auto n = 12345678901234567890_i128;
 */
constexpr int128_t operator"" _i128(const char *str) {
  return internal::constexpr_string_to_int<int128_t>(str);
}

/**
 * @brief Literal de usuario para crear un `uint128_t` a partir de una cadena.
 * @example auto n = 12345678901234567890_ui128;
 */
constexpr uint128_t operator"" _ui128(const char *str) {
  return internal::constexpr_string_to_int<uint128_t>(str);
}

/**
 * @brief Literal de usuario para calcular 2 elevado a N como `int128_t`.
 * @example auto n = 65_pow2_i128; // 2^65
 */
constexpr int128_t operator"" _pow2_i128(unsigned long long n) {
  if (n >= 127) {
    throw std::domain_error("Exponente demasiado grande para _pow2_i128");
  }
  return static_cast<int128_t>(1) << n;
}

/**
 * @brief Literal de usuario para calcular 2 elevado a N como `uint128_t`.
 * @example auto n = 127_pow2_ui128; // 2^127
 */
constexpr uint128_t operator"" _pow2_ui128(unsigned long long n) {
  if (n >= 128) {
    throw std::domain_error("Exponente demasiado grande para _pow2_ui128");
  }
  return static_cast<uint128_t>(1) << n;
}

#endif // HAS_NATIVE_INT128

} // namespace literals

} // namespace numbers_calculations::core

// Para facilitar el uso, se puede hacer `using namespace
// numbers_calculations::core::literals;`