/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2020, acafons
 * All rights reserved.
 *******************************************************************************/

#ifndef __T_OBJECT_H__
#define __T_OBJECT_H__

#if !defined (__TLIB_H_INSIDE__) && !defined (TLIB_COMPILATION)
#error "Only <tlib.h> can be included directly."
#endif

#include <stdbool.h>
#include <tlib/tmacros.h>
#include <tlib/tversion.h>

TLIB_BEGIN_DECLS

typedef struct tobject tobject;

TLIB_AVAILABLE_IN_ALL
void tobject_init(tobject* const o);

TLIB_AVAILABLE_IN_ALL
void tobject_free(tobject* o);

TLIB_AVAILABLE_IN_ALL
int tobject_hash(const tobject* o);

TLIB_AVAILABLE_IN_ALL
bool tobject_equals(const tobject* o, const tobject* ref);

TLIB_AVAILABLE_IN_ALL
const char* tobject_getclass(const tobject* o);

TLIB_AVAILABLE_IN_ALL
tobject* tobject_clone(const tobject* o);

TLIB_AVAILABLE_IN_ALL
const char* tobject_to_string(const tobject* o);

TLIB_AVAILABLE_IN_ALL
bool tobject_istypeof_object(const tobject* o);

TLIB_END_DECLS

#endif /* __T_OBJECT_H__ */
