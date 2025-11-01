/* ==============================================================================
 * Archivo: test_factorial.cpp
 * Autor:   Gemini
 *
 * Objetivo:
 * Pruebas unitarias para la función `math::factorial` usando Catch2.
 * Verifica el comportamiento con la LUT, el fallback y el manejo de errores.
 * ==============================================================================
 */

#include "core/constexpr_literals.hpp"
#include "core/math_error.hpp"
#include "math/combinatorics.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace numbers_calculations;
using namespace numbers_calculations::core::literals;

TEST_CASE("Factorial function", "[combinatorics][factorial]") {

  SECTION("Factorial with small values (using LUT)") {
    auto r0 = math::factorial(0LL);
    REQUIRE(r0.has_value());
    REQUIRE(r0.value() == 1LL);

    auto r1 = math::factorial(1LL);
    REQUIRE(r1.has_value());
    REQUIRE(r1.value() == 1LL);

    auto r5 = math::factorial(5LL);
    REQUIRE(r5.has_value());
    REQUIRE(r5.value() == 120LL);

    auto r10 = math::factorial(10);
    REQUIRE(r10.has_value());
    REQUIRE(r10.value() == 3628800);
  }

  SECTION("Factorial at 64-bit limit (using LUT)") {
    auto r20 = math::factorial(20LL); // 20! es el más grande para int64
    REQUIRE(r20.has_value());
    REQUIRE(r20.value() == 2432902008176640000LL);
  }

  SECTION("Factorial overflow on 64-bit types (from LUT)") {
    auto r21 = math::factorial(21LL); // 21! desborda int64
    REQUIRE_FALSE(r21.has_value());
    REQUIRE(r21.error() == core::MathError::Overflow);
  }

  SECTION("Factorial with large values (using LUT on 128-bit)") {
    auto r33 = math::factorial(33_i128); // 33! cabe en 128 bits
    REQUIRE(r33.has_value());
    REQUIRE(r33.value() == 8683317618811886495518194401280000000_i128);
  }

  SECTION("Factorial overflow on 128-bit types (from LUT)") {
    auto r34 = math::factorial(34_i128); // 34! desborda 128 bits
    REQUIRE_FALSE(r34.has_value());
    REQUIRE(r34.error() == core::MathError::Overflow);
  }

  SECTION("Factorial with large values (fallback for n >= 34)") {
    // Usamos cpp_int que no tiene límites de tamaño.
    // El factorial de 34 se calcula con el algoritmo iterativo.
    using mp::cpp_int;
    auto r34_mp = math::factorial(cpp_int(34));
    REQUIRE(r34_mp.has_value());

    // El valor esperado es 34 * factorial(33)
    cpp_int expected = cpp_int("8683317618811886495518194401280000000") * 34;
    REQUIRE(r34_mp.value() == expected);

    // Probamos un valor más grande para asegurar que el bucle funciona
    auto r40_mp = math::factorial(cpp_int(40));
    REQUIRE(r40_mp.has_value());
  }

  SECTION("Factorial with domain errors") {
    auto r_neg_1 = math::factorial(-1);
    REQUIRE_FALSE(r_neg_1.has_value());
    REQUIRE(r_neg_1.error() == core::MathError::DomainError);

    auto r_neg_10 = math::factorial(-10_i128);
    REQUIRE_FALSE(r_neg_10.has_value());
    REQUIRE(r_neg_10.error() == core::MathError::DomainError);
  }
}