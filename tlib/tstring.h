#ifndef _STRING_T_H_
#define _STRING_T_H_

#include <stdbool.h>
#include "native_type.h"
#include "object_t.h"

struct string_t;

/* Memory allocator */
struct string_t *string_new();

/* Init object */
bool string_init(struct string_t *, char *s);
bool string_init_v2(struct string_t *, struct string_t *original);
bool string_init_v3(struct string_t *, char *s, int offset, int count);
bool string_init_v4(struct string_t *, byte *bytes, int bytes_len, int offset,
                    int count);
bool string_init_v5(struct string_t *);

/* Cleanup */
void string_free(struct string_t *);

/* Methods */
int  string_length(struct string_t *);
bool string_is_empty(struct string_t *);
char string_char_at(struct string_t *, int index);
int  string_get_chars(struct string_t *, int src_begin, int src_end, char *dst,
                      int dst_begin, int dst_len);
int  string_get_bytes(struct string_t *, int src_begin, int src_end, byte *dst,
                      int dst_begin, int dst_len);

bool string_equals(struct string_t *, struct object_t *obj);
bool string_equals_ignore_case(struct string_t *, struct string_t *s);

int  string_compare_to(struct string_t *, char *s);
int  string_compare_to_v2(struct string_t *, struct string_t *s);
int  string_compare_to_ignore_case(struct string_t *, char *s);
int  string_compare_to_ignore_case_v2(struct string_t *, struct string_t *s);

bool string_starts_with(struct string_t *, char *prefix);
bool string_starts_with_v2(struct string_t *, char *prefix, int offset);
bool string_starts_with_v3(struct string_t *, struct string_t *prefix);
bool string_starts_with_v4(struct string_t *, struct string_t *prefix,
                           int offset);

bool string_ends_with(struct string_t *, char *suffix);
bool string_ends_with_v2(struct string_t *, struct string_t *suffix);

int string_hash_code(struct string_t *);

int string_index_of(struct string_t *, int c);
int string_index_of_v2(struct string_t *, int c, int from_index);
int string_index_of_v3(struct string_t *, char *s);
int string_index_of_v4(struct string_t *, char *s, int from_index);
int string_index_of_v5(struct string_t *, struct string_t *s);
int string_index_of_v6(struct string_t *, struct string_t *s, int from_index);

int string_last_index_of(struct string_t *, int c);
int string_last_index_of_v2(struct string_t *, int c, int from_index);
int string_last_index_of_v3(struct string_t *, char *s);
int string_last_index_of_v4(struct string_t *, char *s, int from_index);
int string_last_index_of_v5(struct string_t *, struct string_t *s);
int string_last_index_of_v6(struct string_t *, struct string_t *s,
                            int from_index);

struct string_t *string_substring(struct string_t *, int begin_index);
struct string_t *string_substring_v2(struct string_t *, int begin_index,
                                     int end_index);

struct string_t *string_concat(struct string_t *, char *s);
struct string_t *string_concat_v2(struct string_t *, struct string_t *s);

struct string_t *string_replace(struct string_t *, char oldchar, char newchar);

bool string_contains(struct string_t *, char *s);

int string_split(struct string_t *, const char *delim,
                 struct string_t **strings);
int string_split_v2(struct string_t *, struct string_t *s,
                    struct string_t **strings);

struct string_t *string_to_lower_case(struct string_t *);
struct string_t *string_to_upper_case(struct string_t *);

struct string_t *string_trim(struct string_t *);
struct string_t *string_strip(struct string_t *);

bool string_is_blank(struct string_t *);

int string_index_of_non_white_space(struct string_t *);
int string_last_of_non_white_space(struct string_t *);

char *string_to_charp(struct string_t *);

struct string_t *string_repeat(struct string_t *, int count);

/* Static Methods */

struct string_t *string_format(const char *format, ...);

struct string_t *string_value_of(char *);
struct string_t *string_value_of_v2(char *, int offset, int count);
struct string_t *string_value_of_v3(char);
struct string_t *string_value_of_v4(int);
struct string_t *string_value_of_v5(long);
struct string_t *string_value_of_v6(float);
struct string_t *string_value_of_v7(double);
struct string_t *string_value_of_v8(struct object_t *);

#endif
