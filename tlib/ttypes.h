/* TLIB - Library of useful and simple routines for C programming
 * Copyright (C) 2021  Anderson Fonseca
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

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
