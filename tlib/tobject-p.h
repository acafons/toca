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

#ifndef __TOBJECT_H_P__
#define __TOBJECT_H_P__

#if !defined (__TLIB_H_INSIDE__) && !defined (TLIB_COMPILATION)
#error "Only <tlib.h> can be included directly."
#endif

#include <tlib/tobject.h>
#include "ttypes.h"

TLIB_BEGIN_DECLS

struct tobject_vtable
{
        int         (*tobject_hash)      (const void *);
        bool        (*tobject_equals)    (const void *, const void *);
        const char *(*tobject_getclass)  (const void *);
        tobject    *(*tobject_clone)     (const void *);
        const char *(*tobject_to_string) (const void *);
        void        (*tobject_free)      (void *);
};

struct tobject
{
        unsigned int           magic;
        char                   class_type[TLIB_CLASS_MAX_SIZE];
        struct tobject_vtable *vtable;
};

TLIB_END_DECLS

#endif //__TOBJECT_H_P__
