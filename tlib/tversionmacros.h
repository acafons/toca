#ifndef __T_VERSION_MACROS_H__
#define __T_VERSION_MACROS_H__

#if !defined (__TLIB_H_INSIDE__) && !defined (TLIB_COMPILATION)
#error "Only <tlib.h> can be included directly."
#endif

/* Version boundaries checks */

#define T_ENCODE_VERSION(major,minor)   ((major) << 16 | (minor) << 8)

/* XXX: Every new stable minor release bump should add a macro here */

/**
 * TLIB_VERSION_1_0:
 *
 * A macro that evaluates to the 1.0 version of TLib, in a format
 * that can be used by the C pre-processor.
 *
 * Since: 1.0
 */
#define TLIB_VERSION_1_0       (T_ENCODE_VERSION (1, 0))

/**
 * TLIB_VERSION_CUR_STABLE:
 *
 * A macro that evaluates to the current stable version of TLib, in a format
 * that can be used by the C pre-processor.
 *
 * During an unstable development cycle, this evaluates to the next stable
 * (unreleased) version which will be the result of the development cycle.
 *
 * Since: 1.0
 */
#if (TLIB_MINOR_VERSION % 2)
#define TLIB_VERSION_CUR_STABLE         (T_ENCODE_VERSION (TLIB_MAJOR_VERSION, TLIB_MINOR_VERSION + 1))
#else
#define TLIB_VERSION_CUR_STABLE         (T_ENCODE_VERSION (TLIB_MAJOR_VERSION, TLIB_MINOR_VERSION))
#endif

/**
 * TLIB_VERSION_PREV_STABLE:
 *
 * A macro that evaluates to the previous stable version of TLib, in a format
 * that can be used by the C pre-processor.
 *
 * During an unstable development cycle, this evaluates to the most recent
 * released stable release, which preceded this development cycle.
 *
 * Since: 1.0
 */
#if (TLIB_MINOR_VERSION % 2)
#define TLIB_VERSION_PREV_STABLE        (T_ENCODE_VERSION (TLIB_MAJOR_VERSION, TLIB_MINOR_VERSION - 1))
#else
#define TLIB_VERSION_PREV_STABLE        (T_ENCODE_VERSION (TLIB_MAJOR_VERSION, TLIB_MINOR_VERSION - 2))
#endif

/**
 * TLIB_VERSION_MIN_REQUIRED:
 *
 * A macro that should be defined by the user prior to including
 * the tlib.h header.
 * The definition should be one of the predefined TLib version
 * macros: %TLIB_VERSION_1_0, %TLIB_VERSION_1_0,...
 *
 * This macro defines the earliest version of GLib that the package is
 * required to be able to compile against.
 *
 * If the compiler is configured to warn about the use of deprecated
 * functions, then using functions that were deprecated in version
 * %TLIB_VERSION_MIN_REQUIRED or earlier will cause warnings (but
 * using functions deprecated in later releases will not).
 *
 * Since: 2.32
 */
/* If the package sets TLIB_VERSION_MIN_REQUIRED to some future
 * TLIB_VERSION_X_Y value that we don't know about, it will compare as
 * 0 in preprocessor tests.
 */
#ifndef TLIB_VERSION_MIN_REQUIRED
# define TLIB_VERSION_MIN_REQUIRED      (TLIB_VERSION_CUR_STABLE)
#elif TLIB_VERSION_MIN_REQUIRED == 0
# undef  TLIB_VERSION_MIN_REQUIRED
# define TLIB_VERSION_MIN_REQUIRED      (TLIB_VERSION_CUR_STABLE + 2)
#endif

/**
 * TLIB_VERSION_MAX_ALLOWED:
 *
 * A macro that should be defined by the user prior to including
 * the tlib.h header.
 * The definition should be one of the predefined GLib version
 * macros: %TLIB_VERSION_1_0, %TLIB_VERSION_1_0,...
 *
 * This macro defines the latest version of the GLib API that the
 * package is allowed to make use of.
 *
 * If the compiler is configured to warn about the use of deprecated
 * functions, then using functions added after version
 * %TLIB_VERSION_MAX_ALLOWED will cause warnings.
 *
 * Unless you are using TLIB_CHECK_VERSION() or the like to compile
 * different code depending on the TLib version, then this should be
 * set to the same value as %TLIB_VERSION_MIN_REQUIRED.
 *
 * Since: 1.0
 */
#if !defined (TLIB_VERSION_MAX_ALLOWED) || (TLIB_VERSION_MAX_ALLOWED == 0)
# undef TLIB_VERSION_MAX_ALLOWED
# define TLIB_VERSION_MAX_ALLOWED      (TLIB_VERSION_CUR_STABLE)
#endif

/* sanity checks */
#if TLIB_VERSION_MIN_REQUIRED > TLIB_VERSION_CUR_STABLE
#error "TLIB_VERSION_MIN_REQUIRED must be <= TLIB_VERSION_CUR_STABLE"
#endif
#if TLIB_VERSION_MAX_ALLOWED < TLIB_VERSION_MIN_REQUIRED
#error "TLIB_VERSION_MAX_ALLOWED must be >= TLIB_VERSION_MIN_REQUIRED"
#endif
#if TLIB_VERSION_MIN_REQUIRED < TLIB_VERSION_2_26
#error "TLIB_VERSION_MIN_REQUIRED must be >= TLIB_VERSION_2_26"
#endif

/* These macros are used to mark deprecated functions in GLib headers,
 * and thus have to be exposed in installed headers. But please
 * do *not* use them in other projects. Instead, use G_DEPRECATED
 * or define your own wrappers around it.
 */
#define TLIB_AVAILABLE_IN_ALL                   _TLIB_EXTERN

#endif

