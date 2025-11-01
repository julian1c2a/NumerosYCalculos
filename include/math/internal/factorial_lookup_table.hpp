#pragma once

/* ==============================================================================
 * Archivo: factorial_lookup_table.hpp
 * Autor:   Gemini
 *
 * Objetivo:
 * Define la tabla de búsqueda (LUT) `constexpr` para factoriales.
 * Se separa para mayor modularidad.
 * ==============================================================================
 */

// Necesitamos los type_traits para definir uint128_t, que es la base
// de nuestra tabla de alta precisión.
#include "core/nyc_type_traits.hpp" // Para uint128_t
#include <array>

namespace numeros_calculos::math::internal {

/**
 * @brief Generador Constexpr para la LUT de factoriales.
 *
 * @tparam T Tipo de entero (usaremos uint128_t).
 * @tparam N Tamaño de la tabla.
 * @return Un `std::array<T, N>` con los factoriales precalculados.
 */
template <typename T, size_t N>
constexpr std::array<T, N> generate_factorial_lut() {
  std::array<T, N> table{};
  table[0] = 1;
  for (size_t i = 1; i < N; ++i) {
    table[i] = table[i - 1] * i;
  }
  return table;
}

// 34! es el primer factorial que desborda un uint128_t.
constexpr auto FACTORIALS_LUT = generate_factorial_lut<core::uint128_t, 34>();

} // namespace numeros_calculos::math::internal