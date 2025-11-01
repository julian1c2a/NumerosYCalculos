#pragma once

/* ==============================================================================
 * Archivo: numeric_io.hpp
 * Autor:   Gemini
 *
 * Objetivo:
 * Proporciona sobrecargas de `operator<<` y `operator>>` para los tipos
 * de enteros extendidos (como __int128), permitiendo su uso nativo con
 * `std::ostream` y `std::istream` (ej. `std::cout`, `std::cin`).
 *
 * Cumple con la especificación de "gemini.md":
 * - Soporte completo de E/S para `__int128`.
 * - Manejo de errores para `istream` usando `failbit`.
 *
 * @todo_feature Extender para que funcione con tipos de Boost.Multiprecision.
 * ==============================================================================
 */

#include <algorithm> // Para std::reverse
#include <istream>
#include <limits> // Para std::numeric_limits
#include <numbers_calculations/core/extended_type_traits.hpp>
#include <ostream>
#include <string>
#include <vector>

namespace numbers_calculations::core {

// Declaraciones adelantadas para resolver ambigüedad con GCC 15+
#if HAS_NATIVE_INT128
std::ostream &operator<<(std::ostream &os, const uint128_t &val);
std::ostream &operator<<(std::ostream &os, const int128_t &val);
#endif

#if HAS_NATIVE_INT128

/**
 * @brief Sobrecarga del operador de salida (<<) para `uint128_t`.
 * Permite imprimir valores de 128 bits sin signo en un `std::ostream`.
 */
inline std::ostream &operator<<(std::ostream &os, const uint128_t &val) {
  if (val == 0) {
    os << '0';
    return os;
  }

  // Convertimos el número a una cadena de caracteres manualmente.
  std::string s;
  s.reserve(40); // Suficiente para 2^128 - 1
  uint128_t temp = val;

  while (temp > 0) {
    s += (temp % 10) + '0';
    temp /= 10;
  }

  std::reverse(s.begin(), s.end());
  os << s;
  return os;
}

/**
 * @brief Sobrecarga del operador de salida (<<) para `int128_t`.
 * Permite imprimir valores de 128 bits con signo en un `std::ostream`.
 */
inline std::ostream &operator<<(std::ostream &os, const int128_t &val) {
  if (val == 0) {
    os << '0';
    return os;
  }

  // Manejar el caso del valor mínimo, que no tiene un positivo equivalente.
  if (val == std::numeric_limits<int128_t>::min()) {
    os << "-170141183460469231731687303715884105728";
    return os;
  }

  std::string s;
  s.reserve(41); // 40 dígitos + signo
  int128_t temp = val;

  if (temp < 0) {
    os << '-';
    temp = -temp; // Ahora podemos tratarlo como positivo
  }

  while (temp > 0) {
    s += (temp % 10) + '0';
    temp /= 10;
  }

  std::reverse(s.begin(), s.end());
  os << s;
  return os;
}

/**
 * @brief Sobrecarga del operador de entrada (>>) para `int128_t`.
 * Permite leer valores de 128 bits con signo desde un `std::istream`.
 * Maneja overflow y formato inválido estableciendo el `failbit` del stream.
 */
inline std::istream &operator>>(std::istream &is, int128_t &val) {
  std::string s;
  is >> s; // Leemos la entrada como una cadena

  if (s.empty()) {
    is.setstate(std::ios_base::failbit);
    return is;
  }

  val = 0;
  size_t i = 0;
  bool is_negative = false;

  if (s[0] == '-') {
    is_negative = true;
    i = 1;
  } else if (s[0] == '+') {
    i = 1;
  }

  if (i == s.length()) { // Solo había un signo
    is.setstate(std::ios_base::failbit);
    return is;
  }

  for (; i < s.length(); ++i) {
    if (s[i] < '0' || s[i] > '9') { // Caracter no numérico
      is.setstate(std::ios_base::failbit);
      return is;
    }

    int digit = s[i] - '0';
    // Comprobación de overflow antes de multiplicar
    if (val > (std::numeric_limits<int128_t>::max() - digit) / 10) {
      is.setstate(std::ios_base::failbit);
      return is;
    }
    val = val * 10 + digit;
  }

  if (is_negative) {
    val = -val;
  }

  return is;
}

#endif // HAS_NATIVE_INT128

} // namespace numbers_calculations::core
