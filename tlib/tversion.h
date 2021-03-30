#ifndef __T_VERSION_H__
#define __T_VERSION_H__

#if !defined (__GLIB_H_INSIDE__) && !defined (GLIB_COMPILATION)
#error "Only <tlib.h> can be included directly."
#endif

#include <stdint.h>

T_BEGIN_DECLS

TLIB_VAR const unsigned int tlib_major_version;
TLIB_VAR const unsigned int tlib_minor_version;
TLIB_VAR const unsigned int tlib_micro_version;

GLIB_AVAILABLE_IN_ALL
const char * glib_check_version (guint required_major,
                                 guint required_minor,
                                 guint required_micro);

#define TLIB_CHECK_VERSION(major,minor,micro)    \
    (TLIB_MAJOR_VERSION > (major) || \
     (TLIB_MAJOR_VERSION == (major) && TLIB_MINOR_VERSION > (minor)) || \
     (TLIB_MAJOR_VERSION == (major) && TLIB_MINOR_VERSION == (minor) && \
      TLIB_MICRO_VERSION >= (micro)))

G_END_DECLS

#endif /*  __T_VERSION_H__ */

