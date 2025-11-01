/* ==============================================================================
 * Archivo: main.cpp
 * Autor:   Gemini
 *
 * Objetivo:
 * Demo principal interactiva para probar y mostrar las características de
 * la biblioteca 'NumerosYCalculos'.
 * ==============================================================================
 */

// --- Includes de la Biblioteca ---
// Incluimos los headers públicos de la biblioteca.
// Los detalles de implementación (como lookup_tables) son incluidos
// por estos headers públicos cuando es necesario.
#include <numbers_calculations/core/constexpr_literals.hpp>
#include <numbers_calculations/core/extended_type_traits.hpp>
#include <numbers_calculations/core/numeric_io.hpp>
#include <numbers_calculations/math/combinatorics.hpp>
#include <numbers_calculations/math/integer_ops.hpp>

// --- Includes de la Biblioteca Estándar ---
#include <iomanip> // Para std::setw
#include <iostream>
#include <limits> // Para std::numeric_limits
#include <string>

// ==========================================================================
// Puntos de entrada para las Demos
// ==========================================================================
// Usaremos `using namespace` aquí para que las demos sean legibles.
// (En los headers .hpp nunca usamos `using namespace`).
using namespace numbers_calculations::core::literals;
using namespace numbers_calculations;
namespace mp = boost::multiprecision;

// --- Demo para la Fase 2a: Type Traits ---
void demo_extended_type_traits() {
  std::cout << "\n--- [Demo: Fase 2a - Type Traits] ---\n";
  std::cout << std::boolalpha; // Imprimir 'true'/'false'

  std::cout << "is_supported_integer_v<int>:         "
            << core::is_supported_integer_v<int> << "\n";
  std::cout << "is_supported_integer_v<double>:      "
            << core::is_supported_integer_v<double> << "\n";
  std::cout << "is_supported_integer_v<uint128_t>:   "
            << core::is_supported_integer_v<uint128_t> << "\n";
  std::cout << "is_supported_integer_v<mp::cpp_int>: "
            << core::is_supported_integer_v<mp::cpp_int> << "\n";

  std::cout << "\n--- std::numeric_limits<uint128_t> ---\n";
  std::cout << "digits10: " << std::numeric_limits<uint128_t>::digits10 << "\n";
  std::cout << "max():    " << std::numeric_limits<uint128_t>::max() << "\n";
}

// --- Demo para la Fase 2b: Numeric I/O ---
void demo_numeric_io() {
  std::cout << "\n--- [Demo: Fase 2b - Numeric I/O] ---\n";

  uint128_t big_val = std::numeric_limits<uint128_t>::max();
  int128_t min_val = std::numeric_limits<int128_t>::min();

  std::cout << "max() de uint128_t:\n" << big_val << "\n";
  std::cout << "min() de int128_t:\n" << min_val << "\n";

  std::cout
      << "\nPrueba de entrada (Escribe un número grande y pulsa Enter):\n> ";
  try {
    int128_t input_val;
    std::cin >> input_val;
    if (std::cin.fail()) {
      // Limpiar el estado de error de cin
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cout << "¡Entrada no válida detectada por cin.fail()!\n";
    } else {
      std::cout << "Has introducido: " << input_val << "\n";
      std::cout << "Multiplicado x2: " << (input_val * 2) << "\n";
    }
  } catch (const std::exception &e) {
    std::cout << "Excepción capturada (según lo esperado): " << e.what()
              << "\n";
    // Limpiar el estado de error de cin después de una excepción
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
}

// --- Demo para la Fase 2c: Constexpr Literals ---
void demo_constexpr_literals() {
  std::cout << "\n--- [Demo: Fase 2c - Constexpr Literals] ---\n";

  // Prueba de `static_assert`: si esto compila, los literales SON constexpr.
  static_assert(123_i128 == (int128_t)123);
  static_assert(10_pow2_i128 == (int128_t)1024);
  static_assert(123_ui128 == (uint128_t)123);
  static_assert(10_pow2_ui128 == (uint128_t)1024);
  static_assert(1_i128 + 1_i128 == 2_i128);

  std::cout << "static_asserts (constexpr) pasados con éxito.\n";

  // Prueba en tiempo de ejecución
  auto literal_val = 123456789012345678901234567890_i128;
  auto pow2_val = 65_pow2_i128; // > 64 bits

  std::cout << "Valor de literal grande: " << literal_val << "\n";
  std::cout << "Valor de 65_pow2_i128:  " << pow2_val << "\n";
}

// --- Demo para la Fase 2d (rev): Integer Ops ---
template <typename T> void test_power(T base, unsigned int exp) {
  auto result = math::integer_power(base, exp);
  std::cout << "  " << std::setw(3) << std::left << base << "^ " << std::setw(3)
            << std::left << exp << " = ";
  if (result.has_value()) {
    std::cout << result.value() << "\n";
  } else {
    std::cout << core::error_to_string(result.error()) << "\n";
  }
}

template <typename T> void test_log(T base, T n) {
  auto result = math::integer_log(base, n);
  std::cout << "  log" << std::setw(2) << std::left << base << "("
            << std::setw(10) << std::right << n << ") = ";
  if (result.has_value()) {
    std::cout << result.value() << "\n";
  } else {
    std::cout << core::error_to_string(result.error()) << "\n";
  }
}

void demo_integer_ops() {
  std::cout << "\n--- [Demo: Fase 2d (rev) - Integer Ops] ---\n";

  std::cout << "Pruebas de 'integer_power' (con dispatcher de LUTs):\n";
  test_power(2_i128, 10u);  // Usa LUT O(1)
  test_power(10_i128, 38u); // Usa LUT O(1)
  test_power(10_i128, 39u); // Error: Overflow (desde LUT)
  test_power(7_i128, 5u);   // Usa Fallback O(log n)
  test_power(3_i128, 81u);  // Error: Overflow (desde Fallback)

  std::cout << "\nPruebas de 'integer_log' (con dispatcher de optimización):\n";
  test_log(2_i128, 1023_i128);  // Usa optimización log2
  test_log(2_i128, 1024_i128);  // Usa optimización log2
  test_log(10_i128, 999_i128);  // Usa optimización log10 (búsqueda binaria)
  test_log(10_i128, 1000_i128); // Usa optimización log10 (búsqueda binaria)
  test_log(7_i128, 342_i128);   // Usa fallback (bucle genérico)
  test_log(7_i128, 343_i128);   // Usa fallback (bucle genérico)
  test_log(1_i128, 10_i128);    // Error: Dominio
  test_log(10_i128, 0_i128);    // Error: Dominio
}

// --- Demo para la Fase 2d: Combinatorics (Factorial) ---
template <typename T> void test_factorial(T n) {
  auto result = math::factorial(n);
  std::cout << "  factorial(" << std::setw(3) << std::left << n << ") = ";
  if (result.has_value()) {
    std::cout << result.value() << "\n";
  } else {
    std::cout << core::error_to_string(result.error()) << "\n";
  }
}

void demo_combinatorics() {
  std::cout << "\n--- [Demo: Fase 2d - Combinatorics (Factorial)] ---\n";

  std::cout << "Pruebas de 'factorial' con int64_t:\n";
  test_factorial(5LL);
  test_factorial(20LL); // Límite para 64 bits
  test_factorial(21LL); // Overflow
  test_factorial(-1LL); // Dominio

  std::cout << "\nPruebas de 'factorial' con int128_t:\n";
  test_factorial(33_i128); // Límite para 128 bits
  test_factorial(34_i128); // Overflow
}

// --- Demo para la Fase 2e: Permutaciones y Combinaciones ---
template <typename T> void test_permutations(T n, T k) {
  auto result = math::permutations(n, k);
  std::cout << "  P(" << n << ", " << k << ") = ";
  if (result.has_value()) {
    std::cout << result.value() << "\n";
  } else {
    std::cout << core::error_to_string(result.error()) << "\n";
  }
}

template <typename T> void test_combinations(T n, T k) {
  auto result = math::combinations(n, k);
  std::cout << "  C(" << n << ", " << k << ") = ";
  if (result.has_value()) {
    std::cout << result.value() << "\n";
  } else {
    std::cout << core::error_to_string(result.error()) << "\n";
  }
}

// ==========================================================================
// Bucle Principal de la Demo
// ==========================================================================
int main() {

  std::cout << "=============================================\n";
  std::cout << "   Bienvenido a la Demo de NumerosYCalculos  \n";
  std::cout << "=============================================\n";

  demo_extended_type_traits();
  // demo_numeric_io(); // Descomentar para probar la E/S interactiva
  demo_constexpr_literals();
  demo_integer_ops();
  demo_combinatorics();

  // Nueva demo para Permutaciones y Combinaciones
  std::cout << "\n--- [Demo: Fase 2e - Perms y Combs] ---\n";
  test_permutations(10LL, 3LL);
  test_combinations(10LL, 3LL);
  test_combinations(20LL, 10LL);       // C(20,10)
  test_combinations(68_i128, 34_i128); // Overflow

  std::cout << "\n--- [Demo Finalizada] ---\n";

  return 0;
}
