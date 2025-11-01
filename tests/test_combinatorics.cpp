/* ==============================================================================
 * Archivo: test_combinatorics.cpp
 * Autor:   Gemini
 *
 * Objetivo:
 * Pruebas unitarias para la función `math::factorial` usando Catch2.
 * Verifica el comportamiento con la LUT, el fallback y el manejo de errores.
 * ==============================================================================
 */

#include <catch2/catch_test_macros.hpp>
#include <numbers_calculations/core/constexpr_literals.hpp>
#include <numbers_calculations/core/math_error.hpp>
#include <numbers_calculations/math/combinatorics.hpp>

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

TEST_CASE("Permutations function", "[combinatorics][permutations]") {

  SECTION("Permutations with small values") {
    auto r_5_2 = math::permutations(5LL, 2LL); // 5*4 = 20
    REQUIRE(r_5_2.has_value());
    REQUIRE(r_5_2.value() == 20LL);

    auto r_10_3 = math::permutations(10, 3); // 10*9*8 = 720
    REQUIRE(r_10_3.has_value());
    REQUIRE(r_10_3.value() == 720);

    auto r_n_0 = math::permutations(100, 0); // P(n,0) = 1
    REQUIRE(r_n_0.has_value());
    REQUIRE(r_n_0.value() == 1);

    auto r_n_n = math::permutations(5LL, 5LL); // P(n,n) = n!
    REQUIRE(r_n_n.has_value());
    REQUIRE(r_n_n.value() == 120LL);
  }

  SECTION("Permutations with domain errors") {
    auto r_k_gt_n = math::permutations(5, 6);
    REQUIRE_FALSE(r_k_gt_n.has_value());
    REQUIRE(r_k_gt_n.error() == core::MathError::DomainError);

    auto r_neg_n = math::permutations(-5, 2);
    REQUIRE_FALSE(r_neg_n.has_value());
    REQUIRE(r_neg_n.error() == core::MathError::DomainError);

    auto r_neg_k = math::permutations(5, -2);
    REQUIRE_FALSE(r_neg_k.has_value());
    REQUIRE(r_neg_k.error() == core::MathError::DomainError);
  }

  SECTION("Permutations with overflow") {
    // P(34, 2) cabe en 128 bits, pero P(34, 33) no.
    auto r_overflow = math::permutations(34_i128, 33_i128);
    REQUIRE_FALSE(r_overflow.has_value());
    REQUIRE(r_overflow.error() == core::MathError::Overflow);
  }
}

TEST_CASE("Combinations function", "[combinatorics][combinations]") {

  SECTION("Combinations with small values") {
    auto r_5_2 = math::combinations(5LL, 2LL); // (5*4)/2 = 10
    REQUIRE(r_5_2.has_value());
    REQUIRE(r_5_2.value() == 10LL);

    // Probar optimización C(n, k) == C(n, n-k)
    auto r_5_3 = math::combinations(5LL, 3LL); // (5*4*3)/(3*2*1) = 10
    REQUIRE(r_5_3.has_value());
    REQUIRE(r_5_3.value() == 10LL);

    auto r_10_3 = math::combinations(10, 3); // (10*9*8)/(3*2*1) = 120
    REQUIRE(r_10_3.has_value());
    REQUIRE(r_10_3.value() == 120);
  }

  SECTION("Combinations with overflow") {
    // C(67, 34) es el número central más grande que cabe en 128 bits
    auto r_ok = math::combinations(67_i128, 34_i128);
    REQUIRE(r_ok.has_value());

    auto r_overflow = math::combinations(68_i128, 34_i128);
    REQUIRE_FALSE(r_overflow.has_value());
    REQUIRE(r_overflow.error() == core::MathError::Overflow);
  }
}