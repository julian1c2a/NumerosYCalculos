#pragma once

/* ==============================================================================
 * Archivo: lookup_tables.hpp
 * Autor:   Gemini (Refactorizado por sugerencia del usuario)
 *
 * Objetivo:
 * Define las tablas de búsqueda (LUTs) `constexpr` para potencias.
 * Se separa de 'integer_ops.hpp' para mayor modularidad.
 * ==============================================================================
 */

// Necesitamos los extended_type_traits para definir uint128_t, que es la base
// de nuestras tablas de alta precisión.
#include "core/extended_type_traits.hpp" // Para uint128_t
#include <array>
#include <limits>

namespace numbers_calculations::math::internal {

// ======================================================================
// SECCIÓN DE LOOKUP TABLES (LUTs) CONSTEXPR
// ======================================================================
// Tablas precalculadas para potencias de 2, 3, 5, 10 hasta uint128_t
// ----------------------------------------------------------------------

/**
 * @brief Generador Constexpr para las LUTs de potencias.
 *
 * @tparam T Tipo de entero (usaremos uint128_t).
 * @tparam N Tamaño de la tabla.
 * @param base La base para calcular la potencia.
 * @return Un `std::array<T, N>` con las potencias precalculadas.
 * Si ocurre un overflow, el resto de la tabla se rellena con 0.
 */
template <typename T, size_t N>
constexpr std::array<T, N> generate_power_lut(T base) {
  std::array<T, N> table{};
  T current_power = 1;
  for (size_t i = 0; i < N; ++i) {
    if (i > 0) {
      // Comprobación de overflow: (current_power * base) > max
      if (current_power > std::numeric_limits<T>::max() / base) {
        // El resto de la tabla serán 0 (indicador de overflow)
        break;
      }
      current_power *= base;
    }
    table[i] = current_power;
  }
  return table;
}

// Base 2: 2^0 a 2^127 (128 valores)
constexpr auto POWERS_OF_2 = generate_power_lut<uint128_t, 128>(2);
// Base 3: 3^0 a 3^80 (81 valores)
constexpr auto POWERS_OF_3 = generate_power_lut<uint128_t, 81>(3);
// Base 5: 5^0 a 5^55 (56 valores)
constexpr auto POWERS_OF_5 = generate_power_lut<uint128_t, 56>(5);
// Base 10: 10^0 a 10^38 (39 valores)
constexpr auto POWERS_OF_10 = generate_power_lut<uint128_t, 39>(10);

} // namespace numbers_calculations::math::internal
