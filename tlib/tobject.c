#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "tobject.h"
#include "tobject-p.h"
#include "ttypes.h"


static int
__t_object_hash(const void *this)
{
	(void)(this);
	return -1;
}

static bool
__t_object_equals(const void *this,
		  const void *obj)
{
	TObject *object = (TObject *) this;
	TObject *ref = (TObject *) obj;

	return (object == ref);
}

const char *
__t_object_get_class(const void *this)
{
	TObject *object = (TObject *) this;

	return strdup(object->class_type);
}

static TObject *
__t_object_clone(const void *this)
{
	(void)(this);
	return NULL;
}

static void
__t_object_free(void *this)
{
	TObject *object = (TObject *) this;
	free(object);
}

/**
 * t_object_new:
 *
 * Construct a new object.
 * 
 * Returns: on success, functions return a pointer to the object allocated. It
 *          returns NULL if insufficent memory was available.
 */
TObject *
t_object_new()
{
	return (TObject *) calloc(1, sizeof(TObject));
}

/**
 * t_object_init:
 * @object: the object to be initialized.
 *
 * Initiliaze a object.
 * 
 */
void
t_object_init(TObject * const object)
{
	strncpy(object->class_type, CLASS_TOBJECT, CLASS_MAX_SIZE);

	object->clone_of_func  = __t_object_clone;
	object->equals_func    = __t_object_equals;
	object->get_class_func = __t_object_get_class;
	object->hash_func      = __t_object_hash;
	object->free_func      = __t_object_free;
}

/**
 * Destruct a object.
 * 
 * @param[in] object The object parameter.
 * 
 * @return none.
 */
void
t_object_free(TObject *object)
{
	if (!object) return;

	object->free_func(object);
}

/**
 * Returns a hash code value for the object.
 * 
 * @param[in] this The object parameter.
 * 
 * @return a hash code value for this object or -1 if not supported.
  */
int
t_object_hash(const TObject *object)
{
    return object->hash_func(object);
}

/**
 * Indicates whether some other object is "equal to" this one.
 * 
 * @param[in] this The object parameter.
 * @param[in] ref  The reference object with which to compare.
 * 
 * @return {@code true} if this object is the same as the obj
 *         argument; {@code false} otherwise. 
 */
bool
t_object_equals(const TObject *object,
		const TObject *ref)
{
    return object->equals_func(object, ref);
}

/**
 * Returns the runtime class of this {@code Object}.
 * 
 * @param[in] this The object parameter.
 * 
 * @return a pointer to the class type string. It returns NULL if insufficient
 *         memory was available. The caller is responsible for free the string.
 */
const char *
t_object_get_class(const TObject *object)
{
    return object->get_class_func(object);
}

/**
 * Creates and returns a copy of this object.
 * 
 * @param[in] this The object parameter.
 * 
 * @return a clone of the object or NULL if insufficent memory was available or
 *         the object doesn't support the clone operation.
 */
TObject *object_clone(const TObject *object)
{
    return object->clone_of_func(object);
}

