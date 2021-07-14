/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2020, acafons
 * All rights reserved.
 *******************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include "tobject.h"
#include "tobject-p.h"
#include "tstring.h"

#if !defined(_MSC_VER) || (_MSC_VER < 1400)
#include "linux/string_s/string_s.h"
#endif

struct tstring
{
        tobject parent;
        char*   cstr;
        int     length;
        int     hash;
        bool    hash_is_zero;
        void    (*parent_free)(void* );
};

static int __tstring_hashcode(const void* this)
{
        tstring* s = (tstring*)this;

        if (s->hash || s->hash_is_zero) return s->hash;

        for (int i = 0; i < s->length; i++)
                s->hash = 31 * s->hash + (s->cstr[i] & 0xff);

        if (s->hash == 0) s->hash_is_zero = true;

        return s->hash;
}

static const char* __tstring_getclass(const void* this)
{
        tstring* s = (tstring*)this;
        return strdup(s->parent.class_type);
}

static bool __tstring_equals(const void* this, const void* ref)
{
        if (this == ref) return true;

        if (!ref || !tstring_istypeof_string(ref)) return false;

        tstring* s1 = (tstring*)this;
        tstring* s2 = (tstring*)ref;

        if (s1->length != s2->length) return false;

        return memcmp(s1->cstr, s2->cstr, s1->length) == 0;
}

static tobject* __tstring_clone(const void* this)
{
        tstring* s = (tstring*)this;
        tstring* d = (tstring*)malloc(sizeof(tstring));

        if (!d) return NULL;

        memcpy(d, s, sizeof(tstring));

        if (!(d->cstr = strdup(s->cstr)))
        {
                free(d);
                return NULL;
        }

        return (tobject*)d;
}

static void __tstring_free(void* this)
{
        if (!this) return;

        tstring* s = (tstring*)this;

        if (s->cstr) free(s->cstr);

        s->parent_free((tobject*)s);
}

static const char* __tstring_to_string(const void* this)
{
        tstring* s = (tstring *)this;
        return s->cstr;
}

static int __last_indexof(char* src, int src_count, const char* tgt,
                          int tgt_count, int from_index)
{
        int roff = src_count - tgt_count;

        if (roff < 0 || from_index < 0) return -1;

        char* p = src + roff - from_index;

        for (; p >= src; p--)
        {
                if (strncmp(p, tgt, tgt_count) == 0)
                        return p - src;
        }

        return -1;
}

/* strcasecmp: compare S1 and S2, ignoring case. */
static int __strcasecmp(const char* s1, const char* s2)
{
        const unsigned char* p1 = (const unsigned char*)s1;
        const unsigned char* p2 = (const unsigned char*)s2;
        int r;

        for (; (r = tolower(*p1) - tolower(*p2)) == 0 && *p1 != '\0';
                p1++, p2++);

        return r;
}

/* index: returns a pointer to the first occurrence of the character c in the
          string p.
*/
static char* __index(const char* p, int c)
{
        for (; *p != (char)c; p++)
                if (*p == '\0') return NULL;

        return (char*)p;
}

/* rindex: returns a pointer to the last occurrence of the character c in the
   string s.
*/
static char* __rindex(const char* p, int c)
{
        char* save;
        char ch = c;

        for (save = NULL;; p++)
        {
                if (*p == ch)   save = (char*)p;
                if (*p == '\0') return save;
        }
}

/* tstring_transform: convert string to upper or lower case. */
static tstring* __tstring_transform(const tstring* s, int (*func)(int))
{
        tstring* newstr = NULL;
        char*    buf    = strdup(s->cstr);

        if (!buf) return NULL;

        for (int i = 0; buf[i] != '\0'; i++) 
                buf[i] = func((unsigned char) buf[i]);
        
        if (!(newstr = tstring_new(buf))) free(buf);

        return newstr;
}

/**
* Check {@code begin}, {@code end} against {@code 0} and {@code length}
* bounds.
*
* @param[in] begin   The initial index.
* @param[in] end     The end index.
* @param[in] length  The length.
* 
* @returns {@code true} if the string object is out of bounds;
*          {@code false} otherwise. 
*/
static bool __check_bounds_begin_end(int begin, int end, int length)
{
        return (begin < 0 || begin > end || end > length);
}

/**
 * Check {@code offset}, {@code count} against {@code 0} and {@code length}
 * bounds.
 * 
 * @param[in] offset  The initial offset.
 * @param[in] count   The count.
 * @param[in] length  The length.
 * 
 * @returns {@code true} if the string object is out of bounds;
 *          {@code false} otherwise. 
 */
static bool __check_bounds_off_count(int offset, int count, int length)
{
        return (offset < 0 || count < 0 || offset > length - count);
}

static bool __check_bounds(const tstring* s, int src_begin, int src_end,
                           int dst_begin, int dst_len)
{
        return __check_bounds_begin_end(src_begin, src_end, s->length) ||
               __check_bounds_off_count(dst_begin, src_end - src_begin, dst_len);
}

/* get_string_size_to_copy: get the real size of the string in case there is a
 * '\0' between begin and end.
 */
static int __get_string_size_to_copy(const tstring* s, int src_begin,
                                     int src_end)
{
        int ulen = src_end - src_begin;
        if (ulen == 0) return 0;

        int slen = strlen(s->cstr + src_begin);

        return (ulen > slen ? slen : ulen);
}

static bool __create_cstr(tstring* s, const char* v, int vlen, int offset,
                          int len)
{
        if (__check_bounds_off_count(offset, len, vlen))
                return false;

        s->length = len > 0 ? len : 0;
        s->cstr   = (char*)calloc(s->length + 1, sizeof(char));

        if (!s->cstr) return false;

        memcpy(s->cstr, v + offset, s->length);

        return true;
}

static void __save_parent_vtable(tstring* s)
{
        s->parent_free = s->parent.vtable.tobject_free;
}

static void __override_parent_vtable(tstring* s)
{
        s->parent.vtable.tobject_hash      = __tstring_hashcode;
        s->parent.vtable.tobject_equals    = __tstring_equals;
        s->parent.vtable.tobject_getclass  = __tstring_getclass;
        s->parent.vtable.tobject_clone     = __tstring_clone;
        s->parent.vtable.tobject_free      = __tstring_free;
        s->parent.vtable.tobject_to_string = __tstring_to_string;
}

static void __set_classtype_as_string(tstring* s)
{
        strcpy_s(s->parent.class_type, sizeof(s->parent.class_type),
                 TLIB_CLASS_TSTRING);
}

static void __tstring_init(tstring* s)
{
        tobject_init((tobject*)s);
        __set_classtype_as_string(s);
        __save_parent_vtable(s);
        __override_parent_vtable(s);
}

/**
 * Private allocation of a new string object.
 * 
 * Stores the char[] value into a byte[] that each byte represents
 * the 8 low-order bits of the corresponding character, if the char[]
 * contains only latin1 character or a byte[] that stores all
 * characters in their byte sequences including the NULL terminator
 * in the middle of the value.
 * 
 * @param[in] v       The array that is the source of the caracteres.
 * @param[in] vlen    The size of v.
 * @param[in] offset  The initial offset.
 * @param[in] len     The length to be copied.
 * 
 * @return {@code true} if the string object was initialized with success;
 *         {@code false} otherwise. 
 */
static tstring* __tstring_new(const char* v, int vlen, int offset, int len)
{
        tstring* s = (tstring*)calloc(1, sizeof(tstring));
        if (!s) return NULL;

        if (!__create_cstr(s, v, vlen, offset, len))
        {
                free(s);
                return NULL;
        }

        __tstring_init(s);

        return s;
}

/**
 * Creates a new {@code tstring} that represents the sequence of
 * characters currently contained in the character array argument. The
 * contents of the character array are copied; subsequent modification of
 * the character array does not affect the newly created string.
 * 
 * @param[in] s  The initial value of the string.
 * 
 * @return On success, it returns a pointer to the string object allocated or
 *         NULL if insufficent memory.
 */
tstring* tstring_new(const char* s)
{
        return __tstring_new(s, strlen(s), 0, strlen(s));
}

/**
 * Creates a newly {@code tstring} object so that it represents the same
 * sequence of characters as the argument; in other words, the newly created
 * string is a copy of the argument string.
 * 
 * @param[in] s A {@code tstring} orignal parameter.
 * 
 * @return On success, it returns a pointer to the string object allocated or
 *         NULL if insufficent memory.
 */
tstring* tstring_new_v2(const tstring* s)
{
        return __tstring_new(s->cstr, s->length, 0, s->length);
}

/**
 * Creates a new {@code tstring} that contains characters from a subarray
 * of the character array argument. The contents of the subarray are copied;
 * subsequent modification of the character array does not affect the newly
 * created string.
 * 
 * @param[in] s       Array that is the source of characters.
 * @param[in] offset  The initial offset.
 * @param[in] count   The length of the subarray.
 * 
 * @return On success, it returns a pointer to the string object allocated or
 *         NULL if insufficent memory.
 */
tstring* tstring_new_v3(const char* s, int offset, int count)
{
        return __tstring_new(s, strlen(s), offset, count);
}

/**
 * Creates a new {@code tstring} object so that it represents
 * an empty character sequence.  Note that use of this constructor is
 * unnecessary since strings_t is immutable.
 * 
 * @param[in] string  The {@code tstring} object parameter.
 * 
 * @return {@code true} if the string object was initialized with success;
 *         {@code false} otherwise. 
 */
tstring* tstring_new_v4()
{
        return __tstring_new("", 0, 0, 0);
}

/**
 * This function deletes a {@code tstring} object. The string handle cannot
 * be used anymore when this function has been called.
 * 
 * Passing in a NULL pointer in {@code string} will make this function return
 * immediately with no action.
 * 
 * @param[in] s  The {@code tstring} object parameter.
 */
void tstring_free(tstring* s)
{
        if (!s) return;
        s->parent.vtable.tobject_free(s);
}

/**
 * Create an exact copy of the string.
 * 
 * @param[in] s  The {@code tstring} object parameter.
 * 
 * @returns a copy of the string. 
 */
tstring* tstring_clone(tstring* s)
{
        return (tstring*)s->parent.vtable.tobject_clone(s);
}

/**
 * Returns the length of this string. 
 * 
 * @param[in] string  The {@code tstring} object parameter.
 * 
 * @returns the length of the sequence of characters represented by this
 *          object. 
 */
int tstring_length(const tstring* s)
{
        return s->length;
}

/**
 * Returns {@code true} if, and only if, {@link #length()} is {@code 0}. 
 * 
 * @param[in] string  The {@code tstring} object parameter.
 * 
 * @returns {@code true} if {@link #length()} is {@code 0}, otherwise
 *          {@code false}
 */
bool tstring_isempty(const tstring* s)
{
        return s->length == 0;
}

/**
 * Returns the {@code char} value at the specified index. An index ranges
 * from {@code 0} to {@code length() - 1}. The first {@code char} value of
 * the sequence is at index {@code 0}, the next at index {@code 1}, and so
 * on, as for array indexing.
 * 
 * @param[in] s       The {@code tstring} object parameter.
 * @param[in] index   The index of the {@code char} value. If index is less
 *                    than {@code 0}, the first position is used and if index
 *                    is greater then {@code length() - 1}, the last position
 *                    is used.
 * 
 * @return the {@code char} value at the specified index of this string.
 *             The first {@code char} value is at index {@code 0}.
 */
char tstring_at(const tstring* s, int index)
{
        if (index <= 0)
                index = 0;
        else if (index >= s->length)
                index = s->length - 1;
    
        return s->cstr[index];
}

/**
 * Copy a sequence of characters from a subarray to {@code dst}. The copy is
 * done until the length is reached or a '\0' is found. Therefore, if in the
 * middle of the string there is a '\0', the characters copied from
 * {@code src_begin} until this '\0'. The string placed in dst will not be
 * null-terminated.
 * 
 * @param[in] s          The {@code tstring} object parameter.
 * @param[in] src_begin  The index of the first character in the string to copy.
 * @param[in] src_end    The index after the last character in the string to copy.
 * @param[in] dst        The destination array of characters in which the
 *                       characters from String gets copied.
 * @param[in] dst_begin  The destination offset.
 * @param[in] dst_len    The length of the destination.
 * 
 * @returns the number of characters copied or -1 if the index is out of bounds.
 */
int tstring_getchars(const tstring* s, int src_begin, int src_end, char* dst,
                     int dst_begin, int dst_len)
{
        if (__check_bounds(s, src_begin, src_end, dst_begin, dst_len))
                return -1;

        int len = __get_string_size_to_copy(s, src_begin, src_end);

        if (len > 0) memcpy(dst + dst_begin, s->cstr + src_begin, len);

        return len;
}

/**
 * Validates if the specified object is a string type.
 * 
 * @param[in] s  The {@code tobject} parameter.
 * 
 * @returns {@code true} if the given object represents a {@code tstring}
 *          equivalent, {@code false} otherwise.
 */
bool tstring_istypeof_string(const tobject* o)
{
        if (!tobject_istypeof_object(o))
                return false;

        return strcmp(TLIB_CLASS_TSTRING, o->class_type) == 0;
}

/**
 * Compares this string to the specified string. The result is {@code true}
 * if and only if the argument is not {@code NULL} and is a {@code tstring}
 * object that represents the same sequence of characters as this string.
 * 
 * @param[in] s    The {@code tstring} object parameter.
 * @param[in] obj  The object to compare this {@code tstring} against.
 * 
 * @returns {@code true} if the given object represents a {@code String}
 *          equivalent to this string, {@code false} otherwise.
 */
bool tstring_equals(const tstring* s, const tstring* ref)
{
        return s == ref ? true : s->parent.vtable.tobject_equals(s, ref);
}

/**
 * Compares this {@code tstring} to another {@code tstring}, ignoring case
 * considerations. Two strings are considered equal ignoring case if they
 * are of the same length and the corresponding characters in the two strings
 * are equal ignoring case.
 * 
 * @param[in] s    The {@code tstring} object parameter.
 * @param[in] ref  The {@code tstring} to compare this {@code tstring}
 *                 against
 * 
 * @returns {@code true} if the arguments are not {@code NULL} and they
 *          represents an equivalent {@code tstring} ignoring case; {@code
 *          false} otherwise.
 */
bool tstring_equals_ignorecase(const tstring* s, const tstring* ref)
{
        if (s == ref)   return true;
        if (!s || !ref) return false;

        if (s->length != ref->length) return false;

        return __strcasecmp(s->cstr, ref->cstr) == 0;
}

/**
 * Compares the two strings {@code tstring} and {@code s2}. It returns an
 * integer less than, equal to, or greater than zero if {@code string} is
 * found, respectively, to be less than, to match, or be greater than {@code s}.
 * 
 * @param[in] s   The {@code tstring} object parameter.
 * @param[in] s2  The string to be compared.
 * 
 * @returns the value {@code 0} if the argument {@code s2} is equal to
 *          {@code string}; a value less than {@code 0} if this {@code string}
 *          is less than the {@code string} argument; and a value greater than
 *          {@code 0} if this {@code string} is greater than the string
 *          argument.
 */
int tstring_compare(const tstring* s, const char* str)
{
        return strcmp(s->cstr, str);
}

/**
 * Compares the two strings {@code string} and {@code s}. It returns an
 * integer less than, equal to, or greater than zero if {@code string} is
 * found, respectively, to be less than, to match, or be greater than {@code s}.
 * 
 * @param[in] s   The {@code tstring} object parameter.
 * @param[in] s2  The string to be compared.
 * 
 * @returns the value {@code 0} if the argument {@code s} is equal to
 *          {@code string}; a value less than {@code 0} if this {@code string}
 *          is less than the {@code string} argument; and a value greater than
 *          {@code 0} if this {@code string} is greater than the string
 *          argument.
 */
int tstring_compare_v2(const tstring* s, const tstring* str)
{
        return strcmp(s->cstr, str->cstr);
}

/**
 * Performs a byte-by-byte comparison of the strings {@code string} and
 * {@code s2}. It returns an integer less than, equal to, or greater than zero
 * if {@code string} is found, respectively, to be less than, to match, or be
 * greater than {@code s2}.
 * 
 * @param[in] s   The {@code tstring} object parameter.
 * @param[in] s2  The string to be compared.
 * 
 * @returns the value {@code 0} if the argument {@code s} is equal to
 *          {@code string}; a value less than {@code 0} if this {@code string}
 *          is less than the {@code string} argument; and a value greater than
 *          {@code 0} if this {@code string} is greater than the string
 *          argument.
 */
int tstring_compare_ignorecase(const tstring* s, const char* str)
{
        return __strcasecmp(s->cstr, str);
}

/**
 * Performs a byte-by-byte comparison of the strings {@code string} and
 * {@code s2}. It returns an integer less than, equal to, or greater than zero
 * if {@code string} is found, respectively, to be less than, to match, or be
 * greater than {@code s2}.
 * 
 * @param[in] s   The {@code tstring} object parameter.
 * @param[in] s2  The string to be compared.
 * 
 * @returns the value {@code 0} if the argument {@code s} is equal to
 *          {@code string}; a value less than {@code 0} if this {@code string}
 *          is less than the {@code string} argument; and a value greater than
 *          {@code 0} if this {@code string} is greater than the string
 *          argument.
 */
int tstring_compare_ignorecase_v2(const tstring* s, const tstring* str)
{
        return __strcasecmp(s->cstr, str->cstr);
}

/**
 * Tests if {@code string} starts with the specified prefix.
 * 
 * @param[in] s       The {@code tstring} object parameter.
 * @param[in] prefix  The prefix.
 * 
 * @returns {@code true} if the character sequence represented by the
 *          argument is a prefix of the character sequence represented by
 *          string; {@code false} otherwise.
 */
bool tstring_startswith(const tstring* s, const char* prefix)
{
        return strncmp(s->cstr, prefix, strlen(prefix)) == 0;
}

/**
 * Tests if the substring of {@code string} beginning at the specified index
 * starts with the specified prefix.
 * 
 * @param[in] s       The {@code tstring} object parameter.
 * @param[in] prefix  The prefix.
 * @param[in] offset  Where to begin looking in this string.
 * 
 * @returns {@code true} if the character sequence represented by the
 *          argument is a prefix of the substring of this object starting
 *          at index {@code offset}; {@code false} otherwise.
 */
bool tstring_startswith_v2(const tstring* s, const char* prefix, int offset)
{
        if (offset < 0 || offset > (s->length - (int) strlen(prefix)))
                return false;
    
        return strncmp(s->cstr + offset, prefix, strlen(prefix)) == 0;
}

/**
 * Tests if {@code string} starts with the specified prefix.
 * 
 * @param[in] s       The {@code tstring} object parameter.
 * @param[in] prefix  The prefix.
 * 
 * @returns {@code true} if the character sequence represented by the
 *          argument is a prefix of the character sequence represented by
 *          string; {@code false} otherwise.
 */
bool tstring_startswith_v3(const tstring* s, const tstring* prefix)
{
        return strncmp(s->cstr, prefix->cstr, prefix->length) == 0;
}

/**
 * Tests if the substring of {@code string} beginning at the specified index
 * starts with the specified prefix.
 * 
 * @param[in] s       The {@code tstring} object parameter.
 * @param[in] prefix  The prefix.
 * @param[in] offset  Where to begin looking in this string.
 * 
 * @returns {@code true} if the character sequence represented by the
 *          argument is a prefix of the substring of this object starting
 *          at index {@code offset}; {@code false} otherwise.
 */
bool tstring_startswith_v4(const tstring* s, const tstring* prefix, int offset)
{
        return strncmp(s->cstr + offset, prefix->cstr, prefix->length) == 0;
}

/**
 * Tests if {@code string} ends with the specified suffix.
 * 
 * @param[in] s       The {@code tstring} object parameter.
 * @param[in] suffix  The suffix.
 * 
 * @returns {@code true} if the character sequence represented by the
 *          argument is a suffix of the character sequence represented by
 *          string; {@code false} otherwise.
 */
bool tstring_endswith(const tstring* s, const char* suffix)
{
        int index = s->length - strlen(suffix);

        if (index < 0) return false;

        return strcmp(s->cstr + index, suffix) == 0;
}

/**
 * Tests if {@code string} ends with the specified suffix.
 * 
 * @param[in] s       The {@code tstring} object parameter.
 * @param[in] suffix  The suffix.
 * 
 * @returns {@code true} if the character sequence represented by the
 *          argument is a suffix of the character sequence represented by
 *          string; {@code false} otherwise.
 */
bool tstring_endswith_v2(const tstring* s, const tstring* suffix)
{
        int index = s->length - suffix->length;

        if (index < 0) return false;

        return strcmp(s->cstr + index, suffix->cstr) == 0;
}

/**
 * Returns a hash code for this string.
 * 
 * @param[in] s  The {@code tstring} object parameter.
 * 
 * @returns a hash code value for this object.
 */
int tstring_hashcode(const tstring* s)
{
        return s->parent.vtable.tobject_hash(s);
}

/**
 * Returns the index within this string of the first occurrence of the specified
 * character.
 * 
 * @param[in] s  The {@code tstring} object parameter.
 * @param[in] c  A character.
 * 
 * @returns the index of the first occurrence of the character in the character
 *          sequence represented by this object, or {@code -1} if the character
 *          does not occur.
 */
int tstring_indexof(const tstring* s, int c)
{
        char* p = __index(s->cstr, c);

        return p ? p - s->cstr : -1;
}

/**
 * Returns the index within this string of the first occurrence of the
 * specified character, starting the search at the specified index.
 * 
 * @param[in] s           The {@code tstring} object parameter.
 * @param[in] c           A character.
 * @param[in] from_index  The index to start the search from.
 * 
 * @returns the index of the first occurrence of the character in the character
 *          sequence represented by this object that is greater than or equal to
 *          {@code from_index}, or {@code -1} if the character does not occur.
 */
int tstring_indexof_v2(const tstring* s, int c, int from_index)
{
        if (from_index < 0 || from_index >= s->length)
                return -1;

        char* p = __index(s->cstr + from_index, c);

        return p ? p - s->cstr : -1;
}

/**
 * Returns the index within this string of the first occurrence of the
 * specified substring.
 * 
 * @param[in] s   The {@code tstring} object parameter.
 * @param[in] ps  The primitive substring to search for.
 * 
 * @returns the index of the first occurrence of the specified substring, or
 *          {@code -1} if there is no such occurrence.
 */
int tstring_indexof_v3(const tstring* s, const char* str)
{
    char* p = strstr(s->cstr, str);

    return p ? p - s->cstr : -1;
}

/**
 * Returns the index within this string of the first occurrence of the
 * specified substring, starting at the specified index.
 * 
 * @param[in] s           The {@code tstring} object parameter.
 * @param[in] str         The substring to search for.
 * @param[in] from_index  The index from which to start the search.
 * 
 * @returns the index of the first occurrence of the specified substring,
 *          starting at the specified index, or {@code -1} if there is no
 *          such occurrence.
 */
int tstring_indexof_v4(const tstring* s, const char* str, int from_index)
{
        if (from_index < 0 || from_index >= s->length)
                return -1;

        char* p = strstr(s->cstr + from_index, str);

        return p ? p - s->cstr : -1;
}

/**
 * Returns the index within this string of the first occurrence of the
 * specified substring.
 * 
 * @param[in] s    The {@code tstring} object parameter.
 * @param[in] str  The substring to search for.
 * 
 * @returns the index of the first occurrence of the specified substring, or
 *          {@code -1} if there is no such occurrence.
 */
int tstring_indexof_v5(const tstring* s, const tstring* str)
{
        char* p = strstr((char*)str->cstr, str->cstr);

        return p ? p - s->cstr : -1;
}

/**
 * Returns the index within this string of the first occurrence of the
 * specified substring, starting at the specified index.
 * 
 * @param[in] s           The {@code tstring} object parameter.
 * @param[in] str         The substring to search for.
 * @param[in] from_index  The index from which to start the search.
 * 
 * @returns the index of the first occurrence of the specified substring,
 *          starting at the specified index, or {@code -1} if there is no
 *          such occurrence.
 */
int tstring_indexof_v6(const tstring* s, const tstring* str, int from_index)
{
        if (from_index < 0 || from_index >= s->length)
                return -1;

        char* p = strstr(s->cstr + from_index, str->cstr);

        return p ? p - s->cstr : -1;
}

/**
 * Returns the index within this string of the last occurrence of
 * the specified character.
 * 
 * @param[in] s  The {@code tstring} object parameter.
 * @param[in] c  A character.
 * 
 * @returns the index of the last occurrence of the character in the
 *          character sequence represented by this object, or
 *          {@code -1} if the character does not occur.
 */
int tstring_last_indexof(const tstring* s, int c)
{
        char* p = __rindex(s->cstr, c);

        return p ? p - s->cstr : -1;
}

/**
 * Returns the index within this string of the last occurrence of the specified
 * character, searching backward starting at the specified index.
 * 
 * @param[in] s          The {@code tstring} object parameter.
 * @param[in] c           A character.
 * @param[in] from_index  The index to start the search from. There is no
 *                        restriction on the value of {@code from_index}. If it
 *                        is greater than or equal to the length of this string,
 *                        it has the same effect as if it were equal to one less
 *                        than the length of this string: this entire string may
 *                        be searched. If it is negative, it has the same effect
 *                        as if it were -1: -1 is returned.
 * 
 * @returns the index of the last occurrence of the character in the character
 *          sequence represented by this object that is less than or equal to
 *          {@code from_index}, or {@code -1} if the character does not occur
 *          before that point 
 */
int tstring_last_indexof_v2(const tstring* s, int c, int from_index)
{
        int offset = from_index >= s->length ? s->length - 1 : from_index;

        for (; offset >= 0; offset--)
                if (s->cstr[offset] == c) return offset;

        return -1;
}

/**
 * Returns the index within this string of the last occurrence of the
 * specified substring. 
 * 
 * @param[in] s    The {@code tstring} object parameter.
 * @param[in] str  The substring to search for.
 * 
 * @returns the index of the last occurrence of the specified substring, or
 *          {@code -1} if there is no such occurrence.
 */
int tstring_last_indexof_v3(const tstring* s, const char* str)
{
        return __last_indexof(s->cstr, s->length, str, strlen(str), 0);
}

/**
 * Returns the index within this string of the last occurrence of the specified
 * substring, searching backward starting at the specified index.
 * 
 * @param[in] string      The {@code tstring} object parameter.
 * @param[in] s           The substring to search for.
 * @param[in] from_index  The index to start the search from.
 * 
 * @return the index of the last occurrence of the specified substring,
 *         searching backward from the specified index, or {@code -1} if there
 *         is no such occurrence.
 */
int tstring_last_indexof_v4(const tstring* s, const char* str, int from_index)
{
        return __last_indexof(s->cstr, s->length, str, strlen(str),
                              from_index);
}

/**
 * Returns the index within this string of the last occurrence of the
 * specified substring. 
 * 
 * @param[in] s    The {@code tstring} object parameter.
 * @param[in] str  The substring to search for.
 * 
 * @returns the index of the last occurrence of the specified substring, or
 *          {@code -1} if there is no such occurrence.
 */
int tstring_last_indexof_v5(const tstring* s, const tstring* str)
{
        return __last_indexof(s->cstr, s->length, str->cstr, str->length, 0);
}

/**
 * Returns the index within this string of the last occurrence of the specified
 * substring, searching backward starting at the specified index.
 * 
 * @param[in] s           The {@code tstring} object parameter.
 * @param[in] str         The substring to search for.
 * @param[in] from_index  The index to start the search from.
 * 
 * @returns the index of the last occurrence of the specified substring,
 *          searching backward from the specified index, or {@code -1} if there
 *          is no such occurrence.
 */
int tstring_last_indexof_v6(const tstring* s, const tstring* str,
                            int from_index)
{
        return __last_indexof(s->cstr, s->length, str->cstr, str->length,
                              from_index);
}

/**
 * Returns a string that is a substring of this string. The substring begins
 * with the character at the specified index and extends to the end of this
 * string.
 * 
 * @param[in] s            The {@code tstring} object parameter.
 * @param[in] begin_index  The beginning index, inclusive.
 * 
 * @returns the specified substring or NULL if wasn't possible to allocate
 *          memory for the new string or {@code begin_index} is out of bound.
 */
tstring* tstring_substring(const tstring* s, int begin_index)
{
        int count = s->length - begin_index;

        return tstring_new_v3(s->cstr, begin_index, count);
}

/**
 * Returns a string that is a substring of this string. The
 * substring begins at the specified {@code begin_index} and
 * extends to the character at index {@code end_index}.
 * 
 * @param[in] s            The {@code tstring} object parameter.
 * @param[in] begin_index  The beginning index, inclusive.
 * @param[in] end_index    The ending index, exclusive.
 * 
 * @return the specified substring or NULL if wasn't possible to allocate
 *         memory for the new string or {@code begin_index} is out of bound.
 */
tstring* tstring_substring_v2(const tstring* s, int begin_index, int end_index)
{
        if (__check_bounds_off_count(begin_index, end_index, s->length))
                return NULL;
    
        int count = end_index - begin_index;

        return tstring_new_v3(s->cstr, begin_index, count);
}

/**
 * Concatenates the specified string to the end of this string.
 * If the length of the argument string is {@code 0}, then a copy of
 * {@code tstring} object is returned. Otherwise, a {@code tstring}
 * object is returned that represents a character sequence that is the
 * concatenation of the character sequence represented by this {@code tstring}
 * object and the character sequence represented by the argument string.
 * 
 * @param[in] s    The {@code tstring} object parameter.
 * @param[in] str  The {@code s} that is concatenated to the end
 *                 of this {@code string}.
 * 
 * @return a string that represents the concatenation of this object's
 *         characters followed by the string argument's characters.
 */
tstring* tstring_concat(const tstring* s, const char* str)
{
        return tstring_format("%s%s", s->cstr, str);
}

/**
 * Concatenates the specified string to the end of this string.
 * If the length of the argument string is {@code 0}, then a copy of
 * {@code tstring} object is returned. Otherwise, a {@code tstring}
 * object is returned that represents a character sequence that is the
 * concatenation of the character sequence represented by this {@code tstring}
 * object and the character sequence represented by the argument string.
 *
 * @param[in] s    The {@code tstring} object parameter.
 * @param[in] str  The {@code s} that is concatenated to the end
 *                 of this {@code string}.
 * 
 * @returns a string that represents the concatenation of this object's
 *          characters followed by the string argument's characters.
 */
tstring* tstring_concat_v2(const tstring* s, const tstring* str)
{
        return tstring_format("%s%s", s->cstr, str->cstr);
}

/**
 * Returns a string resulting from replacing all occurrences of {@code oldchar}
 * in this string with {@code newchar}.
 * 
 * If the character {@code oldchar} does not occur in the character sequence
 * represented by this {@code string} object, then a NULL reference is returned.
 * Otherwise, a new {@code string} object is returned that represents a
 * character sequence identical to the character sequence represented by this
 * {@code string} object, except that every occurrence of {@code oldchar} is
 * replaced by an occurrence of {@code newchar}.
 * 
 * 
 * @param[in] s        The {@code tstring} object parameter.
 * @param[in] oldchar  The old character.
 * @param[in] newchar  The new character.
 * 
 * @returns a new string derived from this string by replacing every occurrence
 *          of {@code oldchar} with {@code newchar} or NULL if the occurrences
 *          does not occur.
 */
tstring* tstring_replace(const tstring* s, char oldchar, char newchar)
{
        tstring* newstr = NULL;
        char*    buffer = NULL;
        int      len    = s->length;
        int      i      = 0;

        for (; i < len && s->cstr[i] != oldchar; i++);

        if (i >= len) return NULL;

        buffer = (char*)calloc(len + 1, sizeof(char));
        if (!buffer) return NULL;

        memcpy(buffer, s->cstr, len);

        for (; i < len; i++)
        {
                char c = s->cstr[i];
                buffer[i] = (c == oldchar ? newchar : c);
        }

        newstr = tstring_new_v3(buffer, 0, len);

        free(buffer);

        return newstr;
}

/**
 * Returns true if and only if this string contains the specified
 * sequence of char values.
 * 
 * @param[in] s    The {@code tstring} object parameter.
 * @param[in] str  The sequence to search for.
 * 
 * @return true if this string contains {@code s}, false otherwise.
 */
bool tstring_contains(const tstring* s, const char* str)
{
        return tstring_indexof_v3(s, str) >= 0;
}

/**
 * Count the occurrences of the substring str in string s.
 * 
 * @param[in] s    The {@code tstring} object parameter.
 * @param[in] str  The sequence to search for.
 * 
 * @returns the number occurences found or -1 if there is no
 *         memory enough to complete the operation. 
 */
int tstring_count_ocurrences_of(const tstring* s, const char* str)
{
        int   i;
        char* p;
        char* save = strdup(s->cstr);

        if (!save) return -1;
        
        for (i=0, p=save; strtok_r(p, str, &p); i++);

        free(save);

        return i;
}

/**
 * Splits this string around matches of the given delim.
 * 
 * @param[in]  s        The {@code tstring} object parameter.
 * @param[in]  delim    A set of bytes that delimit the tokens in the parsed
 *                      string
 * @param[out] strings  The array of strings computed by splitting this string
 *                      around matches of the given {@code delim}. This
 *                      paremeter can be NULL if the only interest is the
 *                      number of the computed strings. 
 * @returns a integer containing the number of the strings computed by splitting
 *          this, {@code 0} if the string wasn't splitted or -1 if there is no
 *          memory enough to allocated the string splitted. 
 */
int tstring_split(const tstring* s, const char* delim, tstring* strings[])
{
        if (!delim || !strings) return 0;

        int len = tstring_count_ocurrences_of(s, delim);
        if (len <= 0) return len;

        char* save = strdup(s->cstr);
        if (!save) return -1;

        tstring** list = (tstring **) calloc(len, sizeof(tstring *));
        if (!list) goto end;

        int i;
        char *p, *token;
        for (i=0, p=save; (token=strtok_r(p, delim, &p)) && i < len; i++)
                if (!(list[i] = tstring_new(token))) goto error;
        
        free(save);
        *strings = *list;

        return i;
error:
        while (--i >= 0) tstring_free(list[i]);

        free(list);
end:
        free(save);

        return -1;
}

/**
 * Splits this string around matches of the given delim.
 * 
 * @param[in]  s        The {@code tstring} object parameter.
 * @param[in]  delim    A set of bytes that delimit the tokens in the parsed
 *                      string
 * @param[out] strings  The array of strings computed by splitting this string
 *                      around matches of the given {@code delim}. This
 *                      paremeter can be NULL if the only interest is the
 *                      number of the computed strings. 
 * @return a integer containing the number of the strings computed by splitting
 *         this, {@code 0} if the string wasn't splitted or -1 if there is no
 *         memory enough to allocated the string splitted. 
 */
int tstring_split_v2(const tstring* s, const tstring* delim, tstring* strings[])
{
        return tstring_split(s, delim->cstr, strings);
}

/**
 * Converts all of the characters in this {@code tstring} to upper case.
 * 
 * @param[in] s  The {@code tstring} object parameter.
 * 
 * @returns the {@code tstring} converted to upper case.
 */
tstring* tstring_to_uppercase(const tstring* s)
{
        return __tstring_transform(s, toupper);
}

/**
 * Converts all of the characters in this {@code tstring} to lower case.
 * 
 * @param[in] s  The {@code tstring} object parameter.
 * 
 * @returns the {@code tstring} converted to lower case.
 */
tstring* tstring_to_lowercase(const tstring* s)
{
        return __tstring_transform(s, tolower);
}

/**
 * Returns a string whose value is this string, with all leading and trailing
 * space removed, 
 * 
 * @param[in] s  The {@code tstring} object parameter.
 * 
 * @returns a string whose value is this string, with all leading and trailing
 *          space removed, or NULL if no memory enough to allocated to
 *          complete the operation.
 */
tstring* tstring_trim(const tstring* s)
{
        int l = 0;
        int r = s->length;

        for (; (l < r) && ((s->cstr[l] & 0xff) <= ' '); l++);
        for (; (l < r) && ((s->cstr[r-1] & 0xff) <= ' '); r--);

        return (l > 0 || r < s->length) ?
                __tstring_new(s->cstr, r-l, l, r-l) : NULL;
}

/**
 * Returns {@code true} if the string is empty or contains only codepoints,
 * otherwise {@code false}.
 * 
 * @param[in] s  The {@code tstring} object parameter.
 * 
 * @returns {@code true} if the string is empty or contains only codepoints,
 *          otherwise {@code false}
 */
bool tstring_isblank(const tstring* s)
{
        int i = 0;

        for (; (s->cstr[i] != '\0') && ((s->cstr[i] & 0xff) <= ' '); i++);

        return i+1 == s->length;
}

/**
 * Returns the string representation of the {@code tstring} argument.
 * 
 * Whichever way you get a pointer, you must not access memory further along
 * from the pointer than the characters guaranteed present in the descriptions
 * above. Attempts to do so have undefined behaviour, with a very real chance of
 * application crashes and garbage results even for reads, and additionally
 * wholesale data, stack corruption and/or security vulnerabilities for writes.
 * 
 * @param[in] s  The {@code tstring} object parameter.
 * 
 * @returns a string representation of the {@code tstring} argument.
 */
const char* tstring_to_string(const tstring* s)
{
        return s->parent.vtable.tobject_to_string(s);
}

/**
 * Returns a formatted string using the specified format string and arguments.
 * 
 * @param[in] foramt  The format parameter.
 * 
 * @returns a formatted string.
 */
TLIB_GNUC_PRINTF(1, 2)
tstring* tstring_format(const char* format, ...)
{
        va_list args;

        va_start(args, format);
        int size = vsnprintf(NULL, 0, format, args);
        va_end(args);

        if (size < 0) return NULL;

        char* p = (char*)calloc(++size, sizeof(char));
        if (!p) return NULL;

        va_start(args, format);
        size = vsnprintf(p, size, format, args);
        va_end(args);

        tstring* newstr = NULL;
        if (size >= 0) newstr = tstring_new(p);
        
        free(p);

        return newstr;
}

/**
 * Returns the string representation of the {@code char} array argument. The
 * contents of the character array are copied, subsequent modification of the
 * character array does not affect the returned string.
 * 
 * @param[in] s  The character array.
 * 
 * @returns a {@code tstring} that contains the characters of the character
 *          array.
 */
tstring* tstring_value_of(const char* s)
{
        return tstring_new(s);
}

/**
 * Returns the string representation of a specific subarray of the {@code char}
 * array argument.
 * 
 * The {@code offset} argument is the index of the first character of the
 * subarray. The {@code count} argument specifies the length of the subarray.
 * The contents of the subarray are copied; subsequent modification of the
 * character array does not affect the returned string.
 * 
 * @param[in] s       The character array.
 * @param[in] offset  Initial offset of the subarray.
 * @param[in] count   Length of the subarray.

 * @returns a {@code tstring} that contains the characters of the character
 *          array.
 */
tstring* string_value_of_v2(char* s, int offset, int count)
{
        return tstring_new_v3(s, offset, count);
}

/**
 * Returns the string representation of the {@code char} argument.
 * 
 * @param[in] c  a {@code char}.
 *
 * @returns a string of length {@code 1} containing as its single character the
 *          argument {@code c}.
 */
tstring* tstring_value_of_v3(char c)
{
        return tstring_format("%c", c);
}

/**
 * Returns the string representation of the {@code int} argument.
 * 
 * @param[in] i  an {@code int}.
 *
 * @returns a string representation of the {@code int} argument.
 */
tstring* tstring_value_of_v4(int i)
{
        return tstring_format("%i", i);
}

/**
 * Returns the string representation of the {@code long} argument.
 * 
 * @param[in] l  a {@code int}.
 *
 * @returns a string representation of the {@code long} argument.
 */
tstring* tstring_value_of_v5(long l)
{
        return tstring_format("%li", l);

}

/**
 * Returns the string representation of the {@code float} argument.
 * 
 * @param[in] f  a {@code float}.
 *
 * @returns a string representation of the {@code float} argument.
 */
tstring* tstring_value_of_v6(float f)
{
        return tstring_format("%f", f);
}

/**
 * Returns the string representation of the {@code double} argument.
 * 
 * @param[in] d  a {@code double}.
 *
 * @returns a string representation of the {@code double} argument.
 */
tstring* tstring_value_of_v7(double d)
{
        return tstring_format("%lf", d);
}

/**
 * Returns the string representation of the {@code tobject} argument.
 * 
 * @param[in] o  a {@code tobject}.
 *
 * @returns a string representation of the {@code tobject} argument.
 */
tstring* tstring_value_of_v8(const tobject* o)
{
        if (!tobject_istypeof_object(o)) return NULL;

        return tstring_new(tobject_to_string(o));
}
