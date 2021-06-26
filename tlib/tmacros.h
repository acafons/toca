/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2020, acafons
 * All rights reserved.
 *******************************************************************************/

#ifndef __T_MACROS__
#define __T_MACROS__

#if !defined (__TLIB_H_INSIDE__) && !defined (TLIB_COMPILATION)
#error "Only <tlib.h> can be included directly."
#endif

/* We include stddef.h to get the system's definition of NULL
 */
#include <stddef.h>

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
 * The TLIB_LIKELY and TLIB_UNLIKELY macros let the programmer give hints to 
 * the compiler about the expected result of an expression. Some compilers
 * can use this information for optimizations.
 *
 * The _TLIB_BOOLEAN_EXPR macro is intended to trigger a gcc warning when
 * putting assignments in t_return_if_fail ().  
 */
#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
#define _TLIB_BOOLEAN_EXPR(expr)                \
 G_GNUC_EXTENSION ({                            \
   int _tlib_boolean_var_;                      \
   if (expr)                                    \
      _tlib_boolean_var_ = 1;                   \
   else                                         \
      _tlib_boolean_var_ = 0;                   \
   _tlib_boolean_var_;                          \
})
#define TLIB_LIKELY(expr) (__builtin_expect (_T_BOOLEAN_EXPR(expr), 1))
#define TLIB_UNLIKELY(expr) (__builtin_expect (_T_BOOLEAN_EXPR(expr), 0))
#else
#define TLIB_LIKELY(expr) (expr)
#define TLIB_UNLIKELY(expr) (expr)
#endif

#if    __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1) || defined (__clang__)
#define TGNU_DEPRECATED __attribute__((__deprecated__))
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
#define TGNU_DEPRECATED __declspec(deprecated)
#else
#define TGNU_DEPRECATED
#endif

#if    __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5) || defined (__clang__)
#define TGNU_DEPRECATED_FOR(f) __attribute__((__deprecated__("Use '" #f "' instead")))
#elif defined(_MSC_FULL_VER) && (_MSC_FULL_VER > 140050320)
#define TGNU_DEPRECATED_FOR(f) __declspec(deprecated("is deprecated. Use '" #f "' instead"))
#else
#define TGNU_DEPRECATED_FOR(f) TGNU_DEPRECATED
#endif

#if    __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5) || defined (__clang__)
#define TGNU_UNAVAILABLE(maj,min) __attribute__((deprecated("Not available before " #maj "." #min)))
#elif defined(_MSC_FULL_VER) && (_MSC_FULL_VER > 140050320)
#define TGNU_UNAVAILABLE(maj,min) __declspec(deprecated("is not available before " #maj "." #min))
#else
#define TGNU_UNAVAILABLE(maj,min) TLIB_DEPRECATED
#endif

#ifndef _TLIB_EXTERN
#define _TLIB_EXTERN extern
#endif

/* These macros are used to mark deprecated symbols in TLib headers,
 * and thus have to be exposed in installed headers. But please
 * do *not* use them in other projects.
 */

#define TLIB_AVAILABLE_IN_ALL  _TLIB_EXTERN

#ifdef TLIB_DISABLE_DEPRECATION_WARNINGS
#define TLIB_DEPRECATED _TLIB_EXTERN
#define TLIB_DEPRECATED_FOR(f) _TLIB_EXTERN
#define TLIB_UNAVAILABLE(maj,min) _TLIB_EXTERN
#define TLIB_UNAVAILABLE_STATIC_INLINE(maj,min)
#else
#define TLIB_DEPRECATED TGNU_DEPRECATED _TLIB_EXTERN
#define TLIB_DEPRECATED_FOR(f) TGNU_DEPRECATED_FOR(f) _TLIB_EXTERN
#define TLIB_UNAVAILABLE(maj,min) TGNU_UNAVAILABLE(maj,min) _TLIB_EXTERN
#define TLIB_UNAVAILABLE_STATIC_INLINE(maj,min) TGNU_UNAVAILABLE(maj,min)
#endif

#define TLIB_STRINGIFY(macro_or_string)	TLIB_STRINGIFY_ARG (macro_or_string)
#define	TLIB_STRINGIFY_ARG(contents)	#contents

/* Provide a string identifying the current code position */
#if defined(__GNUC__) && (__GNUC__ < 3) && !defined(__cplusplus)
#define TLIB_STRLOC	__FILE__ ":" TLIB_STRINGIFY (__LINE__) ":" __PRETTY_FUNCTION__ "()"
#else
#define TLIB_STRLOC	__FILE__ ":" TLIB_STRINGIFY (__LINE__)
#endif

/* Provide a string identifying the current function, non-concatenatable */
#if defined (__GNUC__) && defined (__cplusplus)
#define TLIB_STRFUNC     ((const char*) (__PRETTY_FUNCTION__))
#elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define TLIB_STRFUNC     ((const char*) (__func__))
#elif defined (__GNUC__) || (defined(_MSC_VER) && (_MSC_VER > 1300))
#define TLIB_STRFUNC     ((const char*) (__FUNCTION__))
#else
#define TLIB_STRFUNC     ((const char*) ("???"))
#endif

/* Guard C code in headers, while including them from C++ */
#ifdef  __cplusplus
#define TLIB_BEGIN_DECLS  extern "C" {
#define TLIB_END_DECLS    }
#else
#define TLIB_BEGIN_DECLS
#define TLIB_END_DECLS
#endif

#endif
