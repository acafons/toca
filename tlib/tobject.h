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

#ifndef __T_OBJECT_H__
#define __T_OBJECT_H__

#if !defined (__TLIB_H_INSIDE__) && !defined (TLIB_COMPILATION)
#error "Only <tlib.h> can be included directly."
#endif

#include <stdbool.h>
#include <tmacros.h>
#include <tversion.h>

TLIB_BEGIN_DECLS

typedef struct tobject tobject;

TLIB_AVAILABLE_IN_ALL
void tobject_init(tobject * const o);

TLIB_AVAILABLE_IN_ALL
void tobject_free(tobject *o);

TLIB_AVAILABLE_IN_ALL
int tobject_hash(const tobject *o);

TLIB_AVAILABLE_IN_ALL
bool tobject_equals(const tobject *o, const tobject *ref);

TLIB_AVAILABLE_IN_ALL
const char* tobject_getclass(const tobject *o);

TLIB_AVAILABLE_IN_ALL
tobject* tobject_clone(const tobject *o);

TLIB_AVAILABLE_IN_ALL
const char* tobject_to_string(const tobject *o);

TLIB_AVAILABLE_IN_ALL
bool tobject_istypeof_object(const tobject *o);

TLIB_END_DECLS

#endif /* __T_OBJECT_H__ */
