/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2020, acafons
 * All rights reserved.
 *******************************************************************************/

#ifndef __T_VERSION_H__
#define __T_VERSION_H__

#if !defined (__TLIB_H_INSIDE__) && !defined (TLIB_COMPILATION)
#error "Only <tlib.h> can be included directly."
#endif

#include <stdint.h>
#include <tlib/ttypes.h>

TLIB_BEGIN_DECLS

TLIB_VAR const unsigned int tlib_major_version;
TLIB_VAR const unsigned int tlib_minor_version;
TLIB_VAR const unsigned int tlib_micro_version;

TLIB_AVAILABLE_IN_ALL
const char * tlib_check_version (int required_major,
                                 int required_minor,
                                 int required_micro);

#define TLIB_CHECK_VERSION(major,minor,micro)    \
    (TLIB_MAJOR_VERSION > (major) || \
     (TLIB_MAJOR_VERSION == (major) && TLIB_MINOR_VERSION > (minor)) || \
     (TLIB_MAJOR_VERSION == (major) && TLIB_MINOR_VERSION == (minor) && \
      TLIB_MICRO_VERSION >= (micro)))


TLIB_END_DECLS

#endif /*  __T_VERSION_H__ */

