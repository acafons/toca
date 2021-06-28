/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2020, acafons
 * All rights reserved.
 *******************************************************************************/

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
        struct tobject_vtable  vtable;
};

TLIB_END_DECLS

#endif //__TOBJECT_H_P__
