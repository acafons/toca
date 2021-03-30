#ifndef __T_OBJECT_H__
#define __T_OBJECT_H__

#if !defined (__TLIB_H_INSIDE__) && !defined (TLIB_COMPILATION)
#error "Only <tlib.h> can be included directly."
#endif

#include <stdbool.h>
#include <tmacros.h>
#include <tversionmacros.h>

T_BEGIN_DECLS

typedef struct TObject TObject;

TLIB_AVAILABLE_IN_ALL
TObject*	t_object_new		();

TLIB_AVAILABLE_IN_ALL
void		t_object_init		(TObject       * const object);

TLIB_AVAILABLE_IN_ALL
void		t_object_free		(TObject       *object);

TLIB_AVAILABLE_IN_ALL
int		t_object_hash		(const TObject *object);

TLIB_AVAILABLE_IN_ALL
bool		t_object_equals		(const TObject *object,
					 const TObject *ref);
TLIB_AVAILABLE_IN_ALL
const char*	t_object_type_class	(const TObject *object);

TLIB_AVAILABLE_IN_ALL
TObject*	t_object_clone		(const TObject *object);

T_END_DECLS

#endif /* __T_STRING_H__ */
