#ifndef __T_MACROS__
#define __T_MACROS__

#if !defined (__TLIB_H_INSIDE__) && !defined (TLIB_COMPILATION)
#error "Only <tlib.h> can be included directly."
#endif

/* We include stddef.h to get the system's definition of NULL
 */
#include <stddef.h>

/**
 * T_GNUC_DEPRECATED:
 *
 * Expands to the GNU C `deprecated` attribute if the compiler is gcc.
 * It can be used to mark `typedef`s, variables and functions as deprecated.
 * When called with the `-Wdeprecated-declarations` option,
 * gcc will generate warnings when deprecated interfaces are used.
 *
 * Place the attribute after the declaration, just before the semicolon.
 *
 * |[<!-- language="C" -->
 * int my_mistake (void) G_GNUC_DEPRECATED;
 * ]|
 *
 * See the [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-deprecated-function-attribute) for more details.
 *
 * Since: 1.0
 */
#if    __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1) || defined (__clang__)
#define T_GNUC_DEPRECATED __attribute__((__deprecated__))
#else
#define T_GNUC_DEPRECATED
#endif /* __GNUC__ */

/**
 * T_GNUC_DEPRECATED_FOR:
 * @f: the intended replacement for the deprecated symbol,
 *     such as the name of a function
 *
 * Like %T_GNUC_DEPRECATED, but names the intended replacement for the
 * deprecated symbol if the version of gcc in use is new enough to support
 * custom deprecation messages.
 *
 * Place the attribute after the declaration, just before the semicolon.
 *
 * |[<!-- language="C" -->
 * int my_mistake (void) T_GNUC_DEPRECATED_FOR(my_replacement);
 * ]|
 *
 * See the [GNU C documentation](https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#index-deprecated-function-attribute) for more details.
 *
 * Note that if @f is a macro, it will be expanded in the warning message.
 * You can enclose it in quotes to prevent this. (The quotes will show up
 * in the warning, but it's better than showing the macro expansion.)
 *
 * Since: 1.0
 */
#if    __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5) || defined (__clang__)
#define T_GNUC_DEPRECATED_FOR(f)                        \
  __attribute__((deprecated("Use " #f " instead")))
#else
#define T_GNUC_DEPRECATED_FOR(f)        T_GNUC_DEPRECATED
#endif /* __GNUC__ */

/* Provide definitions for some commonly used macros.
 *  Some of them are only provided if they haven't already
 *  been defined. It is assumed that if they are already
 *  defined then the current definition is correct.
 */
#ifndef NULL
#  ifdef __cplusplus
#    define NULL        (0L)
#  else /* !__cplusplus */
#    define NULL        ((void*) 0)
#  endif /* !__cplusplus */
#endif

/*
 * The T_LIKELY and T_UNLIKELY macros let the programmer give hints to 
 * the compiler about the expected result of an expression. Some compilers
 * can use this information for optimizations.
 *
 * The _T_BOOLEAN_EXPR macro is intended to trigger a gcc warning when
 * putting assignments in t_return_if_fail ().  
 */
#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
#define _T_BOOLEAN_EXPR(expr)                   \
 G_GNUC_EXTENSION ({                            \
   int _t_boolean_var_;                         \
   if (expr)                                    \
      _t_boolean_var_ = 1;                      \
   else                                         \
      _t_boolean_var_ = 0;                      \
   _t_boolean_var_;                             \
})
#define T_LIKELY(expr) (__builtin_expect (_T_BOOLEAN_EXPR(expr), 1))
#define T_UNLIKELY(expr) (__builtin_expect (_T_BOOLEAN_EXPR(expr), 0))
#else
#define T_LIKELY(expr) (expr)
#define T_UNLIKELY(expr) (expr)
#endif

#if    __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1) || defined (__clang__)
#define T_DEPRECATED __attribute__((__deprecated__))
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
#define T_DEPRECATED __declspec(deprecated)
#else
#define T_DEPRECATED
#endif

#if    __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5) || defined (__clang__)
#define T_DEPRECATED_FOR(f) __attribute__((__deprecated__("Use '" #f "' instead")))
#elif defined(_MSC_FULL_VER) && (_MSC_FULL_VER > 140050320)
#define T_DEPRECATED_FOR(f) __declspec(deprecated("is deprecated. Use '" #f "' instead"))
#else
#define T_DEPRECATED_FOR(f) T_DEPRECATED
#endif

#if    __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5) || defined (__clang__)
#define T_UNAVAILABLE(maj,min) __attribute__((deprecated("Not available before " #maj "." #min)))
#elif defined(_MSC_FULL_VER) && (_MSC_FULL_VER > 140050320)
#define T_UNAVAILABLE(maj,min) __declspec(deprecated("is not available before " #maj "." #min))
#else
#define T_UNAVAILABLE(maj,min) T_DEPRECATED
#endif

#ifndef _TLIB_EXTERN
#define _TLIB_EXTERN extern
#endif

/* These macros are used to mark deprecated symbols in TLib headers,
 * and thus have to be exposed in installed headers. But please
 * do *not* use them in other projects. Instead, use T_DEPRECATED
 * or define your own wrappers around it.
 */

#ifdef TLIB_DISABLE_DEPRECATION_WARNINGS
#define TLIB_DEPRECATED _TLIB_EXTERN
#define TLIB_DEPRECATED_FOR(f) _TLIB_EXTERN
#define TLIB_UNAVAILABLE(maj,min) _TLIB_EXTERN
#define TLIB_UNAVAILABLE_STATIC_INLINE(maj,min)
#else
#define TLIB_DEPRECATED T_DEPRECATED _TLIB_EXTERN
#define TLIB_DEPRECATED_FOR(f) T_DEPRECATED_FOR(f) _TLIB_EXTERN
#define TLIB_UNAVAILABLE(maj,min) T_UNAVAILABLE(maj,min) _TLIB_EXTERN
#define TLIB_UNAVAILABLE_STATIC_INLINE(maj,min) T_UNAVAILABLE(maj,min)
#endif

#define T_STRINGIFY(macro_or_string)	T_STRINGIFY_ARG (macro_or_string)
#define	T_STRINGIFY_ARG(contents)	#contents

/* Provide a string identifying the current code position */
#if defined(__GNUC__) && (__GNUC__ < 3) && !defined(__cplusplus)
#define T_STRLOC	__FILE__ ":" T_STRINGIFY (__LINE__) ":" __PRETTY_FUNCTION__ "()"
#else
#define T_STRLOC	__FILE__ ":" T_STRINGIFY (__LINE__)
#endif

/* Provide a string identifying the current function, non-concatenatable */
#if defined (__GNUC__) && defined (__cplusplus)
#define T_STRFUNC     ((const char*) (__PRETTY_FUNCTION__))
#elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define T_STRFUNC     ((const char*) (__func__))
#elif defined (__GNUC__) || (defined(_MSC_VER) && (_MSC_VER > 1300))
#define T_STRFUNC     ((const char*) (__FUNCTION__))
#else
#define T_STRFUNC     ((const char*) ("???"))
#endif

/* Guard C code in headers, while including them from C++ */
#ifdef  __cplusplus
#define T_BEGIN_DECLS  extern "C" {
#define T_END_DECLS    }
#else
#define T_BEGIN_DECLS
#define T_END_DECLS
#endif

#endif

