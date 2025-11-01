#pragma once

/* ==============================================================================
 * Archivo: nyc_extended_type_traits.hpp (antes extended_type_traits.hpp)
 * Autor:   Gemini
 *
 * Objetivo:
 * Extender los type traits de C++ (<extended_type_traits>) para que reconozcan
 * tipos numéricos no estándar, cruciales para esta biblioteca:
 *
 * 1.  Tipos `__int128` (nativos de GCC/Clang).
 * 2.  Tipos `boost::multiprecision` (tanto de ancho fijo como arbitrario).
 *
 * Este archivo cumple con las especificaciones de "gemini.md" v2:
 * - Detección de tipos numéricos extendidos.
 * - Integración con Boost.Multiprecision.
 * - Extensión de std::numeric_limits (se añadirá en futuras fases).
 *
 * @todo_feature Añadir especializaciones para std::numeric_limits.
 * @todo_feature Añadir traits para tipos racionales (ej: cpp_rational).
 * ==============================================================================
 */

#include <extended_type_traits> // Para std::is_integral, std::integral_constant, etc.
#include <limits>               // Para std::numeric_limits


// --- Detección de Compilador y Soporte de __int128 ---
// __int128_t y __uint128_t son extensiones de GCC y Clang.
#if defined(__GNUC__) || defined(__clang__)
#define HAS_NATIVE_INT128 1
#else
#define HAS_NATIVE_INT128 0
#endif

// --- Inclusión de Boost.Multiprecision ---
// Se asume que Boost está disponible (vía vcpkg para MSVC o sistema para
// GCC/Clang) Si no se encuentra, los traits para Boost simplemente evaluarán a
// 'false'.
#ifdef __has_include
#if __has_include(<boost/multiprecision/cpp_int.hpp>)
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/number.hpp>
// --- INICIO DE CORRECCIÓN ---
// Incluir headers para mpz_int y tom_int (como se pidió en gemini.md)
#if __has_include(<boost/multiprecision/gmp.hpp>)
#include <boost/multiprecision/gmp.hpp> // Para mpz_int
#endif
#if __has_include(<boost/multiprecision/tommath.hpp>)
#include <boost/multiprecision/tommath.hpp> // Para tom_int
#endif
// --- FIN DE CORRECCIÓN ---
#define HAS_BOOST_MULTIPRECISION 1
#else
#define HAS_BOOST_MULTIPRECISION 0
#endif
#else
// Fallback para compiladores más antiguos (como MSVC sin /std:c++latest)
// Este es un chequeo menos robusto, pero necesario.
// El CMakeLists.txt debería asegurar que la inclusión funcione.
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/number.hpp>
// --- INICIO DE CORRECCIÓN ---
// Incluir gmp y tommath, asumiendo que están si cpp_int lo está
// (El toolchain de vcpkg o la instalación del sistema deben proveerlos)
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/tommath.hpp>
// --- FIN DE CORRECCIÓN ---
#define HAS_BOOST_MULTIPRECISION 1
#endif

// === Inicio del namespace de la Biblioteca ===
// (Usando snake_case como se solicitó)
namespace numbers_calculations::core {

// --- Alias para tipos __int128 (si están disponibles) ---
#if HAS_NATIVE_INT128
using int128_t = __int128_t;
using uint128_t = __uint128_t;
#endif

// --- INICIO DE CORRECCIÓN ---
// --- Alias para tipos Boost de ancho fijo no predefinidos ---
// (Cumpliendo la especificación de gemini.md de 2048 bits)
#if HAS_BOOST_MULTIPRECISION
using int2048_t =
    boost::multiprecision::number<boost::multiprecision::cpp_int_backend<
        2048, 2048, boost::multiprecision::signed_magnitude,
        boost::multiprecision::unchecked, void>>;
using uint2048_t =
    boost::multiprecision::number<boost::multiprecision::cpp_int_backend<
        2048, 2048, boost::multiprecision::unsigned_magnitude,
        boost::multiprecision::unchecked, void>>;
#endif
// --- FIN DE CORRECCIÓN ---

// --- Traits de Detección Primaria ---

// 1. is_native_int128
template <typename T> struct is_native_int128 : std::false_type {};
#if HAS_NATIVE_INT128
template <> struct is_native_int128<int128_t> : std::true_type {};
template <> struct is_native_int128<uint128_t> : std::true_type {};
#endif
template <typename T>
inline constexpr bool is_native_int128_v = is_native_int128<T>::value;

// 2. is_boost_number
// Detecta cualquier tipo del backend de boost::multiprecision
template <typename T> struct is_boost_number : std::false_type {};
#if HAS_BOOST_MULTIPRECISION
template <typename Backend,
          boost::multiprecision::expression_template_option ExpressionTemplates>
struct is_boost_number<
    boost::multiprecision::number<Backend, ExpressionTemplates>>
    : std::true_type {};
#endif
template <typename T>
inline constexpr bool is_boost_number_v =
    is_boost_number<std::decay_t<T>>::value;

// 3. is_boost_integer
// Detecta específicamente los tipos enteros de Boost
template <typename T> struct is_boost_integer : std::false_type {};
#if HAS_BOOST_MULTIPRECISION
// Tipos de ancho fijo
template <>
struct is_boost_integer<boost::multiprecision::int128_t> : std::true_type {};
template <>
struct is_boost_integer<boost::multiprecision::uint128_t> : std::true_type {};
template <>
struct is_boost_integer<boost::multiprecision::int256_t> : std::true_type {};
template <>
struct is_boost_integer<boost::multiprecision::uint256_t> : std::true_type {};
template <>
struct is_boost_integer<boost::multiprecision::int512_t> : std::true_type {};
template <>
struct is_boost_integer<boost::multiprecision::uint512_t> : std::true_type {};
template <>
struct is_boost_integer<boost::multiprecision::int1024_t> : std::true_type {};
template <>
struct is_boost_integer<boost::multiprecision::uint1024_t> : std::true_type {};
// --- INICIO DE CORRECCIÓN ---
// Añadir detección para los alias de 2048 bits
template <> struct is_boost_integer<int2048_t> : std::true_type {};
template <> struct is_boost_integer<uint2048_t> : std::true_type {};
// --- FIN DE CORRECCIÓN ---
// Tipos de ancho arbitrario
template <>
struct is_boost_integer<boost::multiprecision::cpp_int> : std::true_type {};
// --- INICIO DE CORRECCIÓN ---
// Añadir detección para mpz_int y tom_int
template <>
struct is_boost_integer<boost::multiprecision::mpz_int> : std::true_type {};
template <>
struct is_boost_integer<boost::multiprecision::tom_int> : std::true_type {};
// --- FIN DE CORRECCIÓN ---
// @todo_feature Añadir detección explícita para tom_int y mpz_int si es
// necesario (¡Hecho!)
#endif
template <typename T>
inline constexpr bool is_boost_integer_v =
    is_boost_integer<std::decay_t<T>>::value ||
    (is_boost_number_v<T> && boost::multiprecision::number_category<T>::value ==
                                 boost::multiprecision::number_kind_integer);

// 4. is_extended_integer (¡El Trait Clave!)
// Es verdadero si T es un entero no estándar que soportamos.
template <typename T>
struct is_extended_integer
    : std::bool_constant<is_native_int128_v<T> || is_boost_integer_v<T>> {};
template <typename T>
inline constexpr bool is_extended_integer_v = is_extended_integer<T>::value;

// 5. is_supported_integer
// Es verdadero si T es CUALQUIER entero (estándar o extendido)
template <typename T>
struct is_supported_integer
    : std::bool_constant<std::is_integral_v<T> || is_extended_integer_v<T>> {};
template <typename T>
inline constexpr bool is_supported_integer_v = is_supported_integer<T>::value;

// --- Especialización de std::numeric_limits para __int128 ---
// (Cumpliendo la especificación de "gemini.md")
// Esto es necesario para que funciones como std::max() funcionen.
#if HAS_NATIVE_INT128
} // cerrar namespace temporalmente para especializar std::

namespace std {

template <> class numeric_limits<numbers_calculations::core::int128_t> {
public:
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
  static constexpr bool has_denorm_loss = false;
  static constexpr std::float_round_style round_style = std::round_toward_zero;
  static constexpr bool is_iec559 = false;
  static constexpr bool is_bounded = true;
  static constexpr bool is_modulo = true;
  // --- INICIO DE CORRECCIÓN ---
  static constexpr int digits = 127;  // 128 bits - 1 bit de signo
  static constexpr int digits10 = 38; // floor(127 * log10(2))
  // --- FIN DE CORRECCIÓN ---
  static constexpr int max_digits10 = 0;
  static constexpr int radix = 2;
  static constexpr int min_exponent = 0;
  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr bool traps = true;
  static constexpr bool tinyness_before = false;

  static constexpr numbers_calculations::core::int128_t min() noexcept {
    return static_cast<numbers_calculations::core::int128_t>(1) << 127;
  }
  static constexpr numbers_calculations::core::int128_t lowest() noexcept {
    return min();
  }
  static constexpr numbers_calculations::core::int128_t max() noexcept {
    return ~min();
  }
  static constexpr numbers_calculations::core::int128_t epsilon() noexcept {
    return 0;
  }
  static constexpr numbers_calculations::core::int128_t round_error() noexcept {
    return 0;
  }
  static constexpr numbers_calculations::core::int128_t infinity() noexcept {
    return 0;
  }
  static constexpr numbers_calculations::core::int128_t quiet_NaN() noexcept {
    return 0;
  }
  static constexpr numbers_calculations::core::int128_t
  signaling_NaN() noexcept {
    return 0;
  }
  static constexpr numbers_calculations::core::int128_t denorm_min() noexcept {
    return 0;
  }
};

// (Especialización para uint128_t)
template <> class numeric_limits<numbers_calculations::core::uint128_t> {
public:
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = false;
  static constexpr bool is_integer = true;
  // ... (resto de miembros similares a int128_t) ...
  // --- INICIO DE CORRECCIÓN ---
  static constexpr int digits = 128;  // 128 bits, sin signo
  static constexpr int digits10 = 38; // floor(128 * log10(2))
  // --- FIN DE CORRECCIÓN ---

  static constexpr numbers_calculations::core::uint128_t min() noexcept {
    return 0;
  }
  static constexpr numbers_calculations::core::uint128_t lowest() noexcept {
    return 0;
  }
  static constexpr numbers_calculations::core::uint128_t max() noexcept {
    return ~static_cast<numbers_calculations::core::uint128_t>(0);
  }
  // ... (resto de miembros en 0) ...
};
} // namespace std

namespace numbers_calculations::core {
// Los numeric_limits para Boost ya están proporcionados por la propia
// biblioteca Boost. No necesitamos especializarlos manualmente.
} // namespace numbers_calculations::core

#endif // HAS_NATIVE_INT128
