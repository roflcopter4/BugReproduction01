#pragma once
#ifndef MKW3FOswbCrnAZjlsqQvpNOJnqADHMTVtSgUEsIrLrbofhZOWrRNmd0jIUCxRC
#define MKW3FOswbCrnAZjlsqQvpNOJnqADHMTVtSgUEsIrLrbofhZOWrRNmd0jIUCxRC //NOLINT(clang-diagnostic-unused-macros)
/***************************************************************************************/


#define ND [[nodiscard]]
#define UU [[maybe_unused]]

#if defined __has_include && __has_include(<QString>) && __has_include(<QStringView>)
# include <QString>
# include <QStringView>
namespace util {

template <size_t N>
# if defined _MSC_VER
__forceinline
# elif defined __GNUC__
__attribute__((__always_inline__))
# endif
consteval QStringView makeQStringView(char16_t const (&literal)[N])
{
    return QStringView{literal, static_cast<qsizetype>(N - 1)};
}

namespace Literals {
inline namespace StringLiterals {
consteval QStringView operator""_sv(char16_t const *str, size_t size) noexcept
{
    return QStringView{str, static_cast<qsizetype>(size)};
}
} // namespace string_literals
} // namespace literals

} // namespace util

using namespace Qt::Literals::StringLiterals;
using util::Literals::StringLiterals::operator""_sv;
#endif

#if defined __has_include && __has_include(<string>) && __has_include(<chrono>)
# include <chrono>
# include <string>
using namespace std::literals; //NOLINT(clang-diagnostic-header-hygiene)
#endif

#define DELETE_COPY_ROUTINES(CLASS)                                                 \
    CLASS(CLASS const &)            = delete;/*NOLINT(bugprone-macro-parentheses)*/ \
    CLASS &operator=(CLASS const &) = delete /*NOLINT(bugprone-macro-parentheses)*/

#define DELETE_MOVE_ROUTINES(CLASS)                                                      \
    CLASS(CLASS &&) noexcept            = delete; /*NOLINT(bugprone-macro-parentheses)*/ \
    CLASS &operator=(CLASS &&) noexcept = delete  /*NOLINT(bugprone-macro-parentheses)*/

#define DELETE_COPY_MOVE_ROUTINES(CLASS) \
    DELETE_COPY_ROUTINES(CLASS);         \
    DELETE_MOVE_ROUTINES(CLASS)

#define DEFAULT_COPY_ROUTINES(CLASS)                                                  \
    CLASS(CLASS const &)            = default; /*NOLINT(bugprone-macro-parentheses)*/ \
    CLASS &operator=(CLASS const &) = default  /*NOLINT(bugprone-macro-parentheses)*/

#define DEFAULT_MOVE_ROUTINES(CLASS)                                                      \
    CLASS(CLASS &&) noexcept            = default; /*NOLINT(bugprone-macro-parentheses)*/ \
    CLASS &operator=(CLASS &&) noexcept = default  /*NOLINT(bugprone-macro-parentheses)*/

#define DEFAULT_COPY_MOVE_ROUTINES(CLASS) \
    DEFAULT_COPY_ROUTINES(CLASS);         \
    DEFAULT_MOVE_ROUTINES(CLASS)


#if defined _MSC_VER
# define PRETTY_FUNCTION_SIG __FUNCSIG__
#elif defined __GNUC__
# define PRETTY_FUNCTION_SIG __PRETTY_FUNCTION__
#else
# define PRETTY_FUNCTION_SIG __func__
#endif


/***************************************************************************************/
#endif
