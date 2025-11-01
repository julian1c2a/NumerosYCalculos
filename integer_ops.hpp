#pragma once

/* ==============================================================================
 * Archivo: integer_ops.hpp
 * Autor:   Gemini
 *
 * Objetivo:
 * Implementa funciones matemáticas básicas para enteros (potencia, logaritmo).
 *
 * Refactorizado: Las LUTs ahora están en 'internal/lookup_tables.hpp'.
 * ==============================================================================
 */

#include <array>    // Para las LUTs
#include <concepts> // Para std::integral (si C++20)
#include <limits>   // Para numeric_limits
#include <numbers_calculations/core/extended_type_traits.hpp> // Para enable_if_t
#include <numbers_calculations/core/extended_type_traits.hpp> // Para is_supported_integer_v
#include <numbers_calculations/core/math_errors.hpp> // Para Expected y MathError
#include <numbers_calculations/math/internal/lookup_tables.hpp> // <-- CORREGIDO: Ruta relativa a internal/
#include <stdexcept> // Para std::domain_error

// --- Detección de intrínsecos de compilador para log2 ---
#if defined(_MSC_VER)
#include <intrin.h>
#define HAS_INTRINSIC_BITSCAN
#elif defined(__GNUC__) || defined(__clang__)
#define HAS_INTRINSIC_BUILTIN_CLZ
#endif

// --- Detección de std::bit_width (C++20) ---
#if __cplusplus >= 202002L
#include <bit>
#define HAS_CPP20_BITWIDTH
#endif

namespace numbers_calculations::math {

namespace internal {

// ======================================================================
// SECCIÓN DE LOOKUP TABLES (LUTs) CONSTEXPR
// ... (Este bloque ha sido movido a 'internal/lookup_tables.hpp') ...
// ======================================================================

// ======================================================================
// SECCIÓN DE ALGORITMOS GENÉRICOS (FALLBACKS)
// ======================================================================

/**
 * @brief Implementación genérica de potencia (exponenciación binaria).
 * @note Algoritmo O(log n), usado como fallback si la base no está en LUT.
 */
template <typename T_Base, typename T_Exp,
          std::enable_if_t<core::is_supported_integer_v<T_Base> &&
                               core::is_unsigned_v<T_Exp>,
                           int> = 0>
constexpr core::Expected<T_Base> generic_power(T_Base base,
                                               T_Exp exp) noexcept {
  T_Base result{1};
  T_Base b = base;
  T_Exp e = exp;

  if (exp == 0)
    return T_Base{1};
  if (base == 0)
    return T_Base{0};

  while (e > 0) {
    if (e % 2 == 1) { // Si el exponente es impar
      // Comprobación de overflow: result * b > max
      if (b != 0 && result > std::numeric_limits<T_Base>::max() / b) {
        return core::Unexpected(core::MathError::Overflow);
      }
      result *= b;
    }

    if (e > 1) { // Solo si no es la última iteración
      // Comprobación de overflow: b * b > max
      if (b != 0 && b > std::numeric_limits<T_Base>::max() / b) {
        return core::Unexpected(core::MathError::Overflow);
      }
      b *= b; // Elevar la base al cuadrado
    }
    e /= 2; // Siguiente dígito del exponente
  }
  return result;
}

/**
 * @brief Implementación genérica de logaritmo (bucle de división).
 * @note Algoritmo O(log_base(n)), usado como fallback.
 */
template <typename T_Base, typename T_Val,
          std::enable_if_t<core::is_supported_integer_v<T_Base> &&
                               core::is_supported_integer_v<T_Val>,
                           int> = 0>
constexpr core::Expected<unsigned int> generic_log(T_Base base,
                                                   T_Val n) noexcept {

  if (base <= 1)
    return core::Unexpected(core::MathError::DomainError);
  if (n <= 0)
    return core::Unexpected(core::MathError::DomainError);
  if (n == 1)
    return 0;

  unsigned int log = 0;
  T_Val current = n;

  while (current >= base) {
    current /= base;
    log++;
  }
  return log;
}

} // namespace internal

// ==========================================================================
// API PÚBLICA: integer_power
// ==========================================================================

/**
 * @brief Calcula la potencia entera (base ^ exp).
 *
 * Es `constexpr` y utiliza `lookup_tables` (LUTs) para bases comunes
 * (2, 3, 5, 10) para un rendimiento O(1) en tiempo de compilación.
 *
 * Para otras bases, recurre a un algoritmo O(log n) de exponenciación binaria.
 *
 * @tparam T_Base Tipo entero de la base.
 * @tparam T_Exp Tipo entero sin signo del exponente.
 * @param base La base.
 * @param exp El exponente (debe ser positivo o 0).
 * @return Un `core::Expected<T_Base>` con el resultado o `MathError::Overflow`.
 *
 * @test_property integer_power(2, 10) == 1024
 * @test_property integer_power(7, 5) == 16807
 * @test_property integer_power(10, 38) (uint128_t) == 10...0 (38 ceros)
 * @test_property integer_power(10, 39) (uint128_t) == MathError::Overflow
 * @test_property integer_power(2, 128) (uint128_t) == MathError::Overflow
 */
template <typename T_Base, typename T_Exp,
          std::enable_if_t<core::is_supported_integer_v<T_Base> &&
                               core::is_unsigned_v<T_Exp>,
                           int> = 0>
constexpr core::Expected<T_Base> integer_power(T_Base base,
                                               T_Exp exp) noexcept {

  // --- Dispatcher de LUTs Constexpr ---

  if (base == 2) {
    if (exp < internal::POWERS_OF_2.size()) {
      return static_cast<T_Base>(internal::POWERS_OF_2[exp]);
    }
    return core::Unexpected(core::MathError::Overflow);
  }
  if (base == 3) {
    if (exp < internal::POWERS_OF_3.size()) {
      // Comprobamos si el valor 0 es por overflow o es 3^0
      if (internal::POWERS_OF_3[exp] == 0 && exp != 0)
        return core::Unexpected(core::MathError::Overflow);
      return static_cast<T_Base>(internal::POWERS_OF_3[exp]);
    }
    return core::Unexpected(core::MathError::Overflow);
  }
  if (base == 5) {
    if (exp < internal::POWERS_OF_5.size()) {
      if (internal::POWERS_OF_5[exp] == 0 && exp != 0)
        return core::Unexpected(core::MathError::Overflow);
      return static_cast<T_Base>(internal::POWERS_OF_5[exp]);
    }
    return core::Unexpected(core::MathError::Overflow);
  }
  if (base == 10) {
    if (exp < internal::POWERS_OF_10.size()) {
      if (internal::POWERS_OF_10[exp] == 0 && exp != 0)
        return core::Unexpected(core::MathError::Overflow);
      return static_cast<T_Base>(internal::POWERS_OF_10[exp]);
    }
    return core::Unexpected(core::MathError::Overflow);
  }

  // --- Fallback a algoritmo genérico O(log n) ---
  return internal::generic_power(base, exp);
}

// ==========================================================================
// API PÚBLICA: integer_log (Logaritmos)
// ==========================================================================

/**
 * @brief [OPTIMIZADO] Calcula el logaritmo entero base 2 (floor(log2(n))).
 *
 * Utiliza la optimización más rápida posible:
 * - C++20: `std::bit_width`
 * - C++17: Intrínsecos de compilador (`__builtin_clz` o `_BitScanReverse`)
 *
 * @tparam T Tipo entero sin signo.
 * @param n El número (debe ser > 0).
 * @return Un `core::Expected<unsigned int>` con el resultado o
 * `MathError::DomainError`.
 *
 * @test_property integer_log2(1) == 0
 * @test_property integer_log2(2) == 1
 * @test_property integer_log2(1023) == 9
 * @test_property integer_log2(1024) == 10
 * @test_property integer_log2(0) == MathError::DomainError
 */
template <typename T, std::enable_if_t<core::is_unsigned_v<T>, int> = 0>
constexpr core::Expected<unsigned int> integer_log2(T n) noexcept {
  if (n == 0) {
    return core::Unexpected(core::MathError::DomainError);
  }

#ifdef HAS_CPP20_BITWIDTH
  // C++20: El método más rápido y portable
  return std::bit_width(n) - 1;
#else
  // C++17: Usamos intrínsecos de compilador

  // Para __uint128_t (que no es un tipo nativo para los intrínsecos)
  if constexpr (std::is_same_v<T, uint128_t>) {
    // Dividimos en dos partes de 64 bits
    uint64_t high = static_cast<uint64_t>(n >> 64);
    if (high != 0) {
      // El bit está en la parte alta
#if defined(HAS_INTRINSIC_BUILTIN_CLZ)
      return (127 - __builtin_clzll(high));
#elif defined(HAS_INTRINSIC_BITSCAN)
      unsigned long index;
      _BitScanReverse64(&index, high);
      return index + 64;
#else
      return internal::generic_log(2, n).value(); // Fallback
#endif
    }
    // El bit está en la parte baja
    uint64_t low = static_cast<uint64_t>(n);
#if defined(HAS_INTRINSIC_BUILTIN_CLZ)
    return (63 - __builtin_clzll(low));
#elif defined(HAS_INTRINSIC_BITSCAN)
    unsigned long index;
    _BitScanReverse64(&index, low);
    return index;
#else
    return internal::generic_log(2, n).value(); // Fallback
#endif
  }
  // Para tipos nativos (uint64_t, uint32_t)
  else if constexpr (sizeof(T) <= sizeof(unsigned long long)) {
    unsigned long long val = static_cast<unsigned long long>(n);
#if defined(HAS_INTRINSIC_BUILTIN_CLZ)
    return ((sizeof(unsigned long long) * 8 - 1) - __builtin_clzll(val));
#elif defined(HAS_INTRINSIC_BITSCAN)
    unsigned long index;
    _BitScanReverse64(&index, val);
    return index;
#else
    return internal::generic_log(2, n).value(); // Fallback
#endif
  } else {
    // Fallback para tipos de Boost (cpp_int, etc.)
    return internal::generic_log(2, n);
  }
#endif // HAS_CPP20_BITWIDTH
}

/**
 * @brief [OPTIMIZADO] Calcula el logaritmo entero base 10 (floor(log10(n))).
 *
 * Utiliza una búsqueda binaria `constexpr` sobre la LUT de `POWERS_OF_10`.
 *
 * @tparam T Tipo entero (idealmente sin signo).
 * @param n El número (debe ser > 0).
 * @return Un `core::Expected<unsigned int>` con el resultado o
 * `MathError::DomainError`.
 *
 * @test_property integer_log10(1) == 0
 * @test_property integer_log10(9) == 0
 * @test_property integer_log10(10) == 1
 * @test_property integer_log10(999) == 2
 * @test_property integer_log10(1000) == 3
 * @test_property integer_log10(0) == MathError::DomainError
 */
template <typename T,
          std::enable_if_t<core::is_supported_integer_v<T>, int> = 0>
constexpr core::Expected<unsigned int> integer_log10(T n) noexcept {
  if (n <= 0) {
    return core::Unexpected(core::MathError::DomainError);
  }

  // Búsqueda binaria sobre la LUT de POWERS_OF_10
  // (std::lower_bound en C++20 es constexpr, pero
  // escribimos un bucle simple para C++17)
  unsigned int low = 0;
  unsigned int high = internal::POWERS_OF_10.size() - 1;
  unsigned int log = 0;

  while (low <= high) {
    unsigned int mid = low + (high - low) / 2;
    if (internal::POWERS_OF_10[mid] == 0) { // Overflow en la tabla
      high = mid - 1;
      continue;
    }

    if (static_cast<uint128_t>(n) >= internal::POWERS_OF_10[mid]) {
      log = mid; // Este es un candidato válido
      low = mid + 1;
    } else {
      high = mid - 1;
    }
  }
  return log;
}

/**
 * @brief Calcula el logaritmo entero (floor(log_base(n))).
 *
 * Es `constexpr` y un dispatcher:
 * - Si base == 2, usa la optimización de bit-scan (O(1)).
 * - Si base == 10, usa la optimización de búsqueda binaria (O(log n)).
 * - Para otras bases, recurre a un bucle de división genérico (O(log n)).
 *
 * @tparam T_Base Tipo entero de la base.
 * @tparam T_Val Tipo entero del número.
 * @param base La base (debe ser > 1).
 * @param n El número (debe ser > 0).
 * @return Un `core::Expected<unsigned int>` con el resultado o
 * `MathError::DomainError`.
 */
template <typename T_Base, typename T_Val,
          std::enable_if_t<core::is_supported_integer_v<T_Base> &&
                               core::is_supported_integer_v<T_Val>,
                           int> = 0>
constexpr core::Expected<unsigned int> integer_log(T_Base base,
                                                   T_Val n) noexcept {

  if (base <= 1)
    return core::Unexpected(core::MathError::DomainError);
  if (n <= 0)
    return core::Unexpected(core::MathError::DomainError);

  // --- Dispatcher de optimizaciones ---
  if (base == 2) {
    // Asegurarnos de que n es sin signo para integer_log2
    if constexpr (std::is_signed_v<T_Val>) {
      return integer_log2(static_cast<std::make_unsigned_t<T_Val>>(n));
    } else {
      return integer_log2(n);
    }
  }
  if (base == 10) {
    return integer_log10(n);
  }

  // --- Fallback a algoritmo genérico ---
  return internal::generic_log(base, n);
}

} // namespace numbers_calculations::math
