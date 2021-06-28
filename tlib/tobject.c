/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2020, acafons
 * All rights reserved.
 *******************************************************************************/

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "tobject.h"
#include "tobject-p.h"
#include "ttypes.h"

#define TLIB_OBJECT_MAGIC 0x70CAC0DE

/**
 * @addtogroup TLib
 * @{
 */

/**
 * @defgroup tobject Object type
 * @brief The base object type
 * 
 * tobject is the fundamental type providing the common attributes and
 * methods for all object types in TLIB.
 */

static int __tobject_hash(const void *this)
{
        (void)(this);
        return -1;
}

static bool __tobject_equals(const void *this, const void *ref)
{
        tobject *o = (tobject *) this;
        tobject *r = (tobject *) ref;

        return (o == r);
}

static const char* __tobject_getclass(const void *this)
{
        tobject *o = (tobject *) this;

        return o->class_type;
}

static tobject* __tobject_clone(const void *this)
{
        (void)(this);
        return NULL;
}

static const char* __tobject_to_string(const void *this)
{
        static char *s = "Empty object.";

        (void)(this);

        return s;
}

static void __tobject_free(void *this)
{
        tobject *o = (tobject *) this;
        free(o);
}

static void __init_vtable(tobject * const o)
{
        o->vtable.tobject_clone     = __tobject_clone;
        o->vtable.tobject_equals    = __tobject_equals;
        o->vtable.tobject_getclass  = __tobject_getclass;
        o->vtable.tobject_hash      = __tobject_hash;
        o->vtable.tobject_to_string = __tobject_to_string;
        o->vtable.tobject_free      = __tobject_free;
}

/**
 * Initiliazes a object.
 * 
 * @param[in] o The object to be initialized.
 */
void tobject_init(tobject * const o)
{
        o->magic = TLIB_OBJECT_MAGIC;
        __init_vtable(o);
}

/**
 * Destructs a object.
 * 
 * @param[in] o The object to be freed.
 */
void tobject_free(tobject *o)
{
        o->vtable.tobject_free(o);
}

/**
 * Returns a hash code value for the object.
 * 
 * @param[in] this The object parameter.
 * 
 * @returns the hash code value for this object or -1 if not supported.
 */
int tobject_hash(const tobject *o)
{
        return o->vtable.tobject_hash(o);
}

/**
 * Indicates whether some other object is "equal to" this one.
 * 
 * @param[in] o    The object parameter.
 * @param[in] ref  The reference object with which to compare.
 * 
 * @returns {@code true} if this object is the same as the obj
 *          argument; {@code false} otherwise. 
 */
bool tobject_equals(const tobject *o, const tobject *ref)
{
        return o->vtable.tobject_equals(o, ref);
}

/**
 * Returns the runtime class of this {@code Object}.
 * 
 * @param[in] o The object parameter.
 * 
 * @returns a pointer to the class type string. The string cannot be freed or
 *          changed.
 */
const char* tobject_getclass(const tobject *o)
{
        return o->vtable.tobject_getclass(o);
}

/**
 * Returns a string representation of the object.
 * 
 * @param[in] o The object parameter.
 * 
 * @returns a pointer to the string or NULL if insufficient memory.
 *          The string cannot be freed or changed.
 */
const char* tobject_to_string(const tobject *o)
{
        return o->vtable.tobject_to_string(o);
}

/**
 * Creates and returns a copy of this object.
 * 
 * @param[in] o The object parameter.
 * 
 * @returns a clone of the object or NULL if insufficent memory was available or
 *          the object doesn't support the clone operation.
 */
tobject* tobject_clone(const tobject *o)
{
        return o->vtable.tobject_clone(o);
}

/**
 * Validates if an object is a type of tobject.
 * 
 * @param[in] o The object parameter.
 * 
 * @returns {@code true} if the object is a tobject; {@code false} otherwise.
 */
bool tobject_istypeof_object(const tobject *o)
{
        return o->magic == TLIB_OBJECT_MAGIC;
}
/** @} */
