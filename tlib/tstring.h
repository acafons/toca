/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2020, acafons
 * All rights reserved.
 *******************************************************************************/

#ifndef __TSTRING_H_
#define __TSTRING_H_

#if !defined (__TLIB_H_INSIDE__) && !defined (TLIB_COMPILATION)
#error "Only <tlib.h> can be included directly."
#endif

#include <stdbool.h>
#include <tversion.h>
#include <tobject.h>
#include <tmacros.h>

TLIB_BEGIN_DECLS

typedef struct tstring tstring;

TLIB_AVAILABLE_IN_ALL
tstring* tstring_new(const char *s);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_new_v2(const tstring *s);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_new_v3(const char *s, int offset, int count);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_new_v4();

TLIB_AVAILABLE_IN_ALL
void tstring_free(tstring *s);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_clone(tstring *s);

TLIB_AVAILABLE_IN_ALL
int  tstring_length(const tstring *s);

TLIB_AVAILABLE_IN_ALL
bool tstring_isempty(const tstring *s);

TLIB_AVAILABLE_IN_ALL
char tstring_at(const tstring *s, int index);

TLIB_AVAILABLE_IN_ALL
int  tstring_getchars(const tstring *s, int src_begin, int src_end, char *dst,
                      int dst_begin, int dst_len);

TLIB_AVAILABLE_IN_ALL
bool tstring_istypeof_string(const tobject *o);

TLIB_AVAILABLE_IN_ALL
bool tstring_equals(const tstring *s, const tstring *ref);

TLIB_AVAILABLE_IN_ALL
bool tstring_equals_ignorecase(const tstring *s, const tstring *ref);

TLIB_AVAILABLE_IN_ALL
int  tstring_compare(const tstring *s, const char *str);

TLIB_AVAILABLE_IN_ALL
int  tstring_compare_v2(const tstring *s, const tstring *str);

TLIB_AVAILABLE_IN_ALL
int tstring_compare_ignorecase(const tstring *s, const char *str);

TLIB_AVAILABLE_IN_ALL
int tstring_compare_ignorecase_v2(const tstring *s, const tstring *str);

TLIB_AVAILABLE_IN_ALL
bool tstring_startswith(const tstring *s, const char *prefix);

TLIB_AVAILABLE_IN_ALL
bool tstring_startswith_v2(const tstring *s, const char *prefix, int offset);

TLIB_AVAILABLE_IN_ALL
bool tstring_startswith_v3(const tstring *s, const tstring *prefix);

TLIB_AVAILABLE_IN_ALL
bool tstring_startswith_v4(const tstring *s, const tstring *prefix, int offset);

TLIB_AVAILABLE_IN_ALL
bool tstring_endswith(const tstring *s, const char *suffix);

TLIB_AVAILABLE_IN_ALL
bool tstring_endswith_v2(const tstring *s, const tstring *suffix);

TLIB_AVAILABLE_IN_ALL
int tstring_hashcode(const tstring *s);

TLIB_AVAILABLE_IN_ALL
int tstring_indexof(const tstring *s, int c);

TLIB_AVAILABLE_IN_ALL
int tstring_indexof_v2(const tstring *s, int c, int from_index);

TLIB_AVAILABLE_IN_ALL
int tstring_indexof_v3(const tstring *s, const char *str);

TLIB_AVAILABLE_IN_ALL
int tstring_indexof_v4(const tstring *s, const char *str, int from_index);

TLIB_AVAILABLE_IN_ALL
int tstring_indexof_v5(const tstring *s, const tstring *str);

TLIB_AVAILABLE_IN_ALL
int tstring_indexof_v6(const tstring *s, const tstring *str, int from_index);

TLIB_AVAILABLE_IN_ALL
int tstring_last_indexof(const tstring *s, int c);

TLIB_AVAILABLE_IN_ALL
int tstring_last_indexof_v2(const tstring *s, int c, int from_index);

TLIB_AVAILABLE_IN_ALL
int tstring_last_indexof_v3(const tstring *s, const char *str);

TLIB_AVAILABLE_IN_ALL
int tstring_last_indexof_v4(const tstring *s, const char *str, int from_index);

TLIB_AVAILABLE_IN_ALL
int tstring_last_indexof_v5(const tstring *s, const tstring *str);

TLIB_AVAILABLE_IN_ALL
int tstring_last_indexof_v6(const tstring *s, const tstring *str,
                            int from_index);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_substring(const tstring *s, int begin_index);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_substring_v2(const tstring *s, int begin_index, int end_index);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_concat(const tstring *s, const char *str);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_concat_v2(const tstring *s, const tstring *str);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_replace(const tstring *s, char oldchar, char newchar);

TLIB_AVAILABLE_IN_ALL
bool tstring_contains(const tstring *s, const char *str);

TLIB_AVAILABLE_IN_ALL
int tstring_count_ocurrences_of(const tstring *s, const char *str);

TLIB_AVAILABLE_IN_ALL
int tstring_split(const tstring *s, const char *delim, tstring *strings[]);

TLIB_AVAILABLE_IN_ALL
int tstring_split_v2(const tstring *s, const tstring *delim,
                     tstring *strings[]);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_to_lowercase(const tstring *s);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_to_uppercase(const tstring *s);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_trim(const tstring *s);

TLIB_AVAILABLE_IN_ALL
bool tstring_isblank(const tstring *s);

TLIB_AVAILABLE_IN_ALL
const char* tstring_to_string(const tstring *s);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_format(const char *format, ...);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_value_of(const char *);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_value_of_v2(const char *, int offset, int count);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_value_of_v3(char);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_value_of_v4(int);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_value_of_v5(long);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_value_of_v6(float);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_value_of_v7(double);

TLIB_AVAILABLE_IN_ALL
tstring* tstring_value_of_v8(const tobject *o);

TLIB_END_DECLS

#endif
