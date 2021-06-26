/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2020, acafons
 * All rights reserved.
 *******************************************************************************/

#ifndef __T_TYPES_H__
#define __T_TYPES_H__

#if !defined (__TLIB_H_INSIDE__) && !defined (TLIB_COMPILATION)
#error "Only <tlib.h> can be included directly."
#endif

/* List the type of classes
 */
#define TLIB_CLASS_MAX_SIZE 30

#define TLIB_CLASS_TOBJECT "tobject"
#define TLIB_CLASS_TSTRING "tstring"

#ifndef TLIB_VAR
#  ifdef T_PLATFORM_WIN32
#    ifdef TLIB_STATIC_COMPILATION
#      define TLIB_VAR extern
#    else /* !TLIB_STATIC_COMPILATION */
#      ifdef TLIB_COMPILATION
#        ifdef DLL_EXPORT
#          define TLIB_VAR extern __declspec(dllexport)
#        else /* !DLL_EXPORT */
#          define TLIB_VAR extern
#        endif /* !DLL_EXPORT */
#      else /* !TLIB_COMPILATION */
#        define TLIB_VAR extern __declspec(dllimport)
#      endif /* !TLIB_COMPILATION */
#    endif /* !TLIB_STATIC_COMPILATION */
#  else /* !T_PLATFORM_WIN32 */
#    define TLIB_VAR _TLIB_EXTERN
#  endif /* !T_PLATFORM_WIN32 */
#endif /* TLIB_VAR */

#endif /* __T_TYPES_H__ */
