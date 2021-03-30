#ifndef __T_OBJECT_H_P__
#define __T_OBJECT_H_P__

#if !defined (__TLIB_H_INSIDE__) && !defined (TLIB_COMPILATION)
#error "Only <tlib.h> can be included directly."
#endif

#include <tlib/tobject.h>
#include "ttypes.h"

T_BEGIN_DECLS

typedef int            (*vt_hash_func)      (const void *);
typedef bool           (*vt_equals_func)    (const void *, const void *);
typedef const char    *(*vt_get_class_func) (const void *);
typedef TObject       *(*vt_clone_func)     (const void *);
typedef void           (*vt_free_func)      (void *);

struct TObject {
	char              class_type[CLASS_MAX_SIZE];
	vt_hash_func      hash_func;
	vt_equals_func    equals_func;
	vt_get_class_func get_class_func;
	vt_clone_func     clone_of_func;
	vt_free_func      free_func;
};

T_END_DECLS

#endif //__T_OBJECT_H_P__

