#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#include "native_type.h"
#include "object_t.h"
#include "object_t_p.h"

#define CLASS_TYPE "string"


typedef struct {
    object_t    object;
    byte       *value;
    int         length;
    int         hash;
    bool        hash_is_zero;
    free_func_t parent_free_func;
} string_t;


int __string_hash_code(void *this)
{
    string_t *string = (string_t *) this;
    size_t i;
    int hc = 0;

    if (hc == 0 && !string->hash_is_zero) {
        for (i = 0; i < string->length; i++) {
            hc = 31 * hc + (string->value[i] & 0xff);
        }

        if (hc == 0)
            string->hash_is_zero = true;
        else
            string->hash = hc;
    }

    return hc;
}

bool __string_equals(void *this, void *obj)
{
    if (this == obj) return true;

    string_t *s1 = (string_t *) this;
    string_t *s2 = string_value_of_v8(obj);

    if (!s2) return false;

    if (s1->length != s2->length) return false;

    if (memcmp(s1->value, s2->value, s1->length) != 0)
        return false;
    
    return true;
}

char *__string_get_class(void *this)
{
    string_t *string = (string_t *) this;

    return strdup(string->object.class_type);
}

struct string_t *__string_clone(void *this)
{
    string_t *s1 = (string_t *) this;
    string_t *s2 = (string_t *) malloc(sizeof(string_t));

    if (!s2) return NULL;

    memcpy(s2, s1, sizeof(string_t));

    return s2;
}

void __string_free(void *this)
{
    string_t *string = (string_t *) this;

    if (string->value) free(string->value);

    string->parent_free_func((struct object *) string);
}

int __last_index_of(char *src, int src_count, char *tgt, int tgt_count,
                    int from_index)
{
    int roff = src_count - tgt_count;

    if (roff < 0 || from_index < 0) return -1;

    char *p = src + roff - from_index;

    for (; p >= src; p--) {
        if (strncmp(p, tgt, tgt_count) == 0)
            return p - src;
    }

    return -1;
}

/**
 * Check {@code offset}, {@code count} against {@code 0} and {@code length}
 * bounds.
 * 
 * @param[in] offset  The initial offset.
 * @param[in] count   The count.
 * @param[in] length  The length.
 * 
 * @return {@code true} if the string object is out of bounds;
 *         {@code false} otherwise. 
 */
bool __check_bounds_off_count(int offset, int count, int length)
{
    return (offset < 0 || count < 0 || offset > length - count);
}

/**
 * Package private initialization.
 * 
 * Stores the char[] value into a byte[] that each byte represents
 * the 8 low-order bits of the corresponding character, if the char[]
 * contains only latin1 character or a byte[] that stores all
 * characters in their byte sequences including the NULL terminator
 * in the middle of the value.
 * 
 * @param[in] string  The string object parameter.
 * @param[in] value   The array that is the source of the caracteres.
 * @param[in] vlen    The size of value.
 * @param[in] off     The initial offset.
 * @param[in] len     The length.
 * 
 * @return {@code true} if the string object was initialized with success;
 *         {@code false} otherwise. 
 */
bool __string_init(string_t *string, char *value, int vlen, int off, int len)
{
    if (__check_bounds_off_count(off, len, vlen))
        return false;

    string->value  = NULL;
    string->length = 0;

    if (len > 0) {
        string->length = len;
        string->value  = (byte *) calloc(string->length + 1, sizeof(byte));

        if (!string->value) return false;

        memcpy(string->value, value, string->length);
    }

    string->hash         = 0;
    string->hash_is_zero = false;

    if (!object_init((struct object_t *) string)) goto error;

    string->parent_free_func = string->object.free_func;
    strcpy(string->object.class_type, CLASS_TYPE);

    string->object.hash_code_func = __string_hash_code;
    string->object.equals_func    = __string_equals;
    string->object.get_class_func = __string_get_class;
    string->object.clone_of_func  = __string_clone;
    string->object.free_func      = __string_free;

    return true;

error:
    if (string->value) free(string->value);

    return false;
}

/**
 * Constructs a new string object.
 * 
 * @return On success, functions return a pointer to the string object
 *         allocated. It returns NULL if insufficent memory was available.
 */
string_t *string_new()
{
    return (string_t *) calloc(1, sizeof(string_t));
}

/**
 * Allocates a new {@code string_t} so that it represents the sequence of
 * characters currently contained in the character array argument. The
 * contents of the character array are copied; subsequent modification of
 * the character array does not affect the newly created string.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The initial value of the string.
 * 
 * @return {@code true} if the string object was initialized with success;
 *         {@code false} otherwise. 
 */
bool string_init(struct string_t *string, char *s)
{
    return __string_init(string, s, strlen(s), 0, strlen(s));
}

/**
 * Initializes a newly created {@code string_t} object so that it represents
 * the same sequence of characters as the argument; in other words, the
 * newly created string is a copy of the argument string.
 * 
 * @param[in] string   The {@code string_t} object parameter.
 * @param[in] original A {@code string_t} orignal parameter.
 * 
 * @return {@code true} if the string object was initialized with success;
 *         {@code false} otherwise. 
 */
bool string_init_v2(string_t *string, string_t *original)
{
    memcpy(string, original, sizeof(string_t));
}

/**
 * Allocates a new {@code string_t} that contains characters from a subarray
 * of the character array argument. The contents of the subarray are copied;
 * subsequent modification of the character array does not affect the newly
 * created string.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       Array that is the source of characters.
 * @param[in] offset  The initial offset.
 * @param[in] count   The length of the subarray.
 * 
 * @return {@code true} if the string object was initialized with success;
 *         {@code false} otherwise. 
 */
bool string_init_v3(string_t *string, char *s, int offset, int count)
{
    return __string_init(string, s, strlen(s), offset, count);
}

/**
 * Allocates a new {@code string_t} that contains bytes from a subarray
 * of the byte array argument. The contents of the subarray are copied;
 * subsequent modification of the byte array does not affect the newly
 * created string.
 * 
 * @param[in] string     The {@code string_t} object parameter.
 * @param[in] bytes      Array that is the source of bytes.
 * @param[in] bytes_len  The length of bytes.
 * @param[in] offset     The initial offset.
 * @param[in] count      The length of the subarray.
 * 
 * @return {@code true} if the string object was initialized with success;
 *         {@code false} otherwise. 
 */
bool string_init_v4(struct string_t *string, byte *bytes, int bytes_len,
                    int offset, int count)
{
    return __string_init(string, (char *) bytes, bytes_len, offset, count);
}

/**
 * Initializes a newly created {@code string_t} object so that it represents
 * an empty character sequence.  Note that use of this constructor is
 * unnecessary since strings_t is immutable.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * 
 * @return {@code true} if the string object was initialized with success;
 *         {@code false} otherwise. 
 */
bool string_init_v5(string_t *string)
{
    return __string_init(string, NULL, 0, 0, 0);
}

/**
 * This function deletes a {@code string_t} object. The string handle cannot
 * be used anymore when this function has been called.
 * 
 * Passing in a NULL pointer in {@code string} will make this function return
 * immediately with no action.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * 
 * @return none.
 */
void string_free(string_t *string)
{
    if (!string) return;

    string->object.free_func(string);
}

/**
 * Returns the length of this string. 
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * 
 * @return the length of the sequence of characters represented by this
 *         object. 
 */
int string_length(string_t *string)
{
    return string->length;
}

/**
 * Returns {@code true} if, and only if, {@link #length()} is {@code 0}. 
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * 
 * @return {@code true} if {@link #length()} is {@code 0}, otherwise
 *         {@code false}
 */
bool string_is_empty(string_t *string)
{
    return string->length == 0;
}

/**
 * Returns the {@code char} value at the specified index. An index ranges
 * from {@code 0} to {@code length() - 1}. The first {@code char} value of
 * the sequence is at index {@code 0}, the next at index {@code 1}, and so
 * on, as for array indexing.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] index   The index of the {@code char} value. If index is less
 *                    than {@code 0}, the first position is used and if index
 *                    is greater then {@code length() - 1}, the last position
 *                    is used.
 * 
 * @return the {@code char} value at the specified index of this string.
 *             The first {@code char} value is at index {@code 0}.
 */
char string_char_at(string_t *string, int index)
{
    if (index < 0)
        index = 0;
    else if (index >= string->length)
        index = string->length - 1;
    
    return string->value[index];
}

/**
 * Copy a sequence of characters from a subarray to {@code dst}. The copy is
 * done until the length is reached or a '\0' is found. Therefore, if in the
 * middle of the string there is a '\0', the characters copied from
 * {@code src_begin} until this '\0'.
 * 
 * @param[in] string     The {@code string_t} object parameter.
 * @param[in] src_begin  The source offset.
 * @param[in] src_end    The length of the source.
 * @param[in] dst        The destination buffer.
 * @param[in] dst_begin  The destination offset.
 * @param[in] dst_len    The length of the destination.
 * 
 * @return the number of characters copied or -1 if the index is out of bounds.
 */
int string_get_chars(string_t *string, int src_begin, int src_end, char *dst,
                     int dst_begin, int dst_len)
{
    if (__check_bounds_off_count(src_begin, src_end, dst_len)) return -1;

    int len = src_end - src_begin;

    if (len == 0) return 0;

    strcpy(dst + dst_begin, string->value + src_begin);

    return len;
}

/**
 * Copy a sequence of bytes from a subarray to {@code dst}. The copy is
 * done until the length is reached and can include as many as '\0' exists
 * in the middle of the sequence of bytes.
 * 
 * @param[in] string     The {@code string_t} object parameter.
 * @param[in] src_begin  The source offset.
 * @param[in] src_end    The length of the source.
 * @param[in] dst        The destination buffer.
 * @param[in] dst_begin  The destination offset.
 * @param[in] dst_len    The length of the destination.
 * 
 * @return the number of characters copied or -1 if the index is out of bounds.
 */
int string_get_bytes(string_t *string, int src_begin, int src_end, byte *dst,
                     int dst_begin, int dst_len)
{
    if (__check_bounds_off_count(src_begin, src_end, dst_len)) return -1;

    int len = src_end - src_begin;

    if (len == 0) return 0;

    memcpy(dst + dst_begin, string->value + src_begin, len);

    return len;
}

/**
 * Compares this string to the specified object. The result is {@code true}
 * if and only if the argument is not {@code NULL} and is a {@code string_t}
 * object that represents the same sequence of characters as this object.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] obj     The object to compare this {@code string_t} against.
 * 
 * @return {@code true} if the given object represents a {@code String}
 *         equivalent to this string, {@code false} otherwise
 */
bool string_equals(string_t *string, struct object_t *obj)
{
    string->object.equals_func(string, obj);
}

/**
 * Compares this {@code string_t} to another {@code string_t}, ignoring case
 * considerations. Two strings are considered equal ignoring case if they
 * are of the same length and the corresponding characters in the two strings
 * are equal ignoring case.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The {@code string_t} to compare this {@code string_t}
 *                    against
 * 
 * @return {@code true} if the arguments are not {@code NULL} and they
 *         represents an equivalent {@code string_t} ignoring case; {@code
 *         false} otherwise.
 */
bool string_equals_ignore_case(string_t *string, string_t *s)
{
    if (string == s) return true;

    if (!s) return false;

    if (string->length != s->length) return false;

    return strcasecmp((char *)string->value, (char *) s->value) == 0;
}

/**
 * Compares the two strings {@code string} and {@code s}. It returns an
 * integer less than, equal to, or greater than zero if {@code string} is
 * found, respectively, to be less than, to match, or be greater than {@code s}.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The string to be compared.
 * 
 * @return the value {@code 0} if the argument {@code s} is equal to
 *         {@code string}; a value less than {@code 0} if this {@code string}
 *         is less than the {@code string} argument; and a value greater than
 *         {@code 0} if this {@code string} is greater than the string argument.
 */
int string_compare_to(string_t *string, char *s)
{
    return strcmp((char *) string->value, s);
}

/**
 * Compares the two strings {@code string} and {@code s}. It returns an
 * integer less than, equal to, or greater than zero if {@code string} is
 * found, respectively, to be less than, to match, or be greater than {@code s}.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The string to be compared.
 * 
 * @return the value {@code 0} if the argument {@code s} is equal to
 *         {@code string}; a value less than {@code 0} if this {@code string}
 *         is less than the {@code string} argument; and a value greater than
 *         {@code 0} if this {@code string} is greater than the string argument.
 */
int string_compare_to_v2(string_t *string, string_t *s)
{
    return strcmp((char *) string->value, (char *) s->value);
}

/**
 * Performs a byte-by-byte comparison of the strings {@code string} and
 * {@code s}. It returns an integer less than, equal to, or greater than zero if
 * {@code string} is found, respectively, to be less than, to match, or be
 * greater than {@code s}.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The string to be compared.
 * 
 * @return the value {@code 0} if the argument {@code s} is equal to
 *         {@code string}; a value less than {@code 0} if this {@code string}
 *         is less than the {@code string} argument; and a value greater than
 *         {@code 0} if this {@code string} is greater than the string argument.
 */
int string_compare_to_ignore_case(string_t *string, char *s)
{
    return strcasecmp((char *) string->value, s);
}

/**
 * Performs a byte-by-byte comparison of the strings {@code string} and
 * {@code s}. It returns an integer less than, equal to, or greater than zero if
 * {@code string} is found, respectively, to be less than, to match, or be
 * greater than {@code s}.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The string to be compared.
 * 
 * @return the value {@code 0} if the argument {@code s} is equal to
 *         {@code string}; a value less than {@code 0} if this {@code string}
 *         is less than the {@code string} argument; and a value greater than
 *         {@code 0} if this {@code string} is greater than the string argument.
 */
int string_compare_to_ignore_case_v2(string_t *string, string_t *s)
{
    return strcasecmp((char *) string->value, (char *) s->value);
}

/**
 * Tests if {@code string} starts with the specified prefix.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] prefix  The prefix.
 * 
 * @return {@code true} if the character sequence represented by the
 *         argument is a prefix of the character sequence represented by
 *         string; {@code false} otherwise.
 */
bool string_starts_with(string_t *string, char *prefix)
{
    return strncmp((char *) string->value, prefix, strlen(prefix)) == 0;
}

/**
 * Tests if the substring of {@code string} beginning at the specified index
 * starts with the specified prefix.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] prefix  The prefix.
 * @param[in] offset  Where to begin looking in this string.
 * 
 * @return {@code true} if the character sequence represented by the
 *         argument is a prefix of the substring of this object starting
 *         at index {@code offset}; {@code false} otherwise.
 */
bool string_starts_with_v2(string_t *string, char *prefix, int offset)
{
    if (offset < 0 || offset > string->length - strlen(prefix))
        return false;
    
    return strncmp((char *)string->value + offset, prefix, strlen(prefix)) == 0;
}

/**
 * Tests if {@code string} starts with the specified prefix.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] prefix  The prefix.
 * 
 * @return {@code true} if the character sequence represented by the
 *         argument is a prefix of the character sequence represented by
 *         string; {@code false} otherwise.
 */
bool string_starts_with_v3(string_t *string, string_t *prefix)
{
    return strncmp((char *)string->value, (char *)prefix->value,
                   prefix->length) == 0;
}

/**
 * Tests if the substring of {@code string} beginning at the specified index
 * starts with the specified prefix.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] prefix  The prefix.
 * @param[in] offset  Where to begin looking in this string.
 * 
 * @return {@code true} if the character sequence represented by the
 *         argument is a prefix of the substring of this object starting
 *         at index {@code offset}; {@code false} otherwise.
 */
bool string_starts_with_v4(string_t *string, string_t *prefix, int offset)
{
    return strncmp((char *)string->value + offset, (char *)prefix->value,
                   prefix->length) == 0;
}

/**
 * Tests if {@code string} ends with the specified suffix.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] prefix  The prefix.
 * 
 * @return {@code true} if the character sequence represented by the
 *         argument is a prefix of the character sequence represented by
 *         string; {@code false} otherwise.
 */
bool string_ends_with(string_t *string, char *suffix)
{
    int index = string->length - strlen(suffix);

    if (index < 0) return false;

    return strcmp((char *)string->value + index, suffix) == 0;
}

/**
 * Tests if {@code string} ends with the specified suffix.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] prefix  The prefix.
 * 
 * @return {@code true} if the character sequence represented by the
 *         argument is a prefix of the character sequence represented by
 *         string; {@code false} otherwise.
 */
bool string_ends_with_v2(string_t *string, string_t *suffix)
{
    int index = string->length - suffix->length;

    if (index < 0) return false;

    return strcmp((char *)string->value + index, (char *)suffix->value) == 0;
}

/**
 * Returns a hash code for this string.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * 
 * @return a hash code value for this object.
 */
int string_hash_code(string_t *string)
{
    return string->object.hash_code_func(string);
}

/**
 * Returns the index within this string of the first occurrence ofthe specified
 * character.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] c       A character.
 * 
 * @return the index of the first occurrence of the character in the character
 *         sequence represented by this object, or {@code -1} if the character
 *         does not occur.
 */
int string_index_of(string_t *string, int c)
{
    char *p = index((char *)string->value, c);

    return p ? p - string->value : -1;
}

/**
 * Returns the index within this string of the first occurrence of the
 * specified character, starting the search at the specified index.
 * 
 * @param[in] string      The {@code string_t} object parameter.
 * @param[in] c           A character.
 * @param[in] from_index  The index to start the search from.
 * 
 * @return the index of the first occurrence of the character in the character
 *         sequence represented by this object that is greater than or equal to
 *         {@code from_index}, or {@code -1} if the character does not occur.
 */
int string_index_of_v2(string_t *string, int c, int from_index)
{
    if (from_index < 0 || from_index >= string->length)
        return -1;
    
    char *p = index((char *)string->value + from_index, c);

    return p ? p - string->value : -1;
}

/**
 * Returns the index within this string of the first occurrence of the
 * specified substring.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The substring to search for.
 * 
 * @return the index of the first occurrence of the specified substring, or
 *         {@code -1} if there is no such occurrence.
 */
int string_index_of_v3(string_t *string, char *s)
{
    char *p = strstr((char *)string->value, s);

    return p ? p - string->value : -1;
}

/**
 * Returns the index within this string of the first occurrence of the
 * specified substring, starting at the specified index.
 * 
 * @param[in] string      The {@code string_t} object parameter.
 * @param[in] s           The substring to search for.
 * @param[in] from_index  The index from which to start the search.
 * 
 * @return the index of the first occurrence of the specified substring,
 *         starting at the specified index, or {@code -1} if there is no
 *         such occurrence.
 */
int string_index_of_v4(string_t *string, char *s, int from_index)
{
    if (from_index < 0 || from_index >= string->length)
        return -1;

    char *p = strstr((char *)string->value + from_index, s);

    return p ? p - string->value : -1;
}

/**
 * Returns the index within this string of the first occurrence of the
 * specified substring.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The substring to search for.
 * 
 * @return the index of the first occurrence of the specified substring, or
 *         {@code -1} if there is no such occurrence.
 */
int string_index_of_v5(string_t *string, string_t *s)
{
    char *p = strstr((char *)string->value, (char *)s->value);

    return p ? p - string->value : -1;
}

/**
 * Returns the index within this string of the first occurrence of the
 * specified substring, starting at the specified index.
 * 
 * @param[in] string      The {@code string_t} object parameter.
 * @param[in] s           The substring to search for.
 * @param[in] from_index  The index from which to start the search.
 * 
 * @return the index of the first occurrence of the specified substring,
 *         starting at the specified index, or {@code -1} if there is no
 *         such occurrence.
 */
int string_index_of_v6(string_t *string, string_t *s, int from_index)
{
    if (from_index < 0 || from_index >= string->length)
        return -1;

    char *p = strstr((char *)string->value + from_index, (char *)s->value);

    return p ? p - string->value : -1;
}

/**
 * Returns the index within this string of the last occurrence of
 * the specified character.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] c       A character.
 * 
 * @return the index of the last occurrence of the character in the
 *         character sequence represented by this object, or
 *         {@code -1} if the character does not occur.
 */
int string_last_index_of(string_t *string, int c)
{
    char *p = rindex((char *)string->value, c);

    return p ? p - string->value : -1;
}

/**
 * Returns the index within this string of the last occurrence of the specified
 * character, searching backward starting at the specified index.
 * 
 * @param[in] string      The {@code string_t} object parameter.
 * @param[in] c           A character.
 * @param[in] from_index  The index to start the search from. There is no
 *                        restriction on the value of {@code from_index}. If it
 *                        is greater than or equal to the length of this string,
 *                        it has the same effect as if it were equal to one less
 *                        than the length of this string: this entire string may
 *                        be searched. If it is negative, it has the same effect
 *                        as if it were -1: -1 is returned.
 * 
 * @return the index of the last occurrence of the character in the character
 *         sequence represented by this object that is less than or equal to
 *         {@code from_index}, or {@code -1} if the character does not occur
 *         before that point 
 */
int string_last_index_of_v2(string_t *string, int c, int from_index)
{
    int offset = from_index >= string->value ? string->value - 1 : from_index;

    for (; offset >= 0; offset--) {
        if (string->value[offset] == (byte)c)
            return offset;
    }

    return -1;
}

/**
 * Returns the index within this string of the last occurrence of the
 * specified substring. 
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The substring to search for.
 * 
 * @return the index of the last occurrence of the specified substring, or
 *         {@code -1} if there is no such occurrence.
 */
int string_last_index_of_v3(string_t *string, char *s)
{
    return __last_index_of(string->value, string->length, s, strlen(s), 0);
}

/**
 * Returns the index within this string of the last occurrence of the specified
 * substring, searching backward starting at the specified index.
 * 
 * @param[in] string      The {@code string_t} object parameter.
 * @param[in] s           The substring to search for.
 * @param[in] from_index  The index to start the search from.
 * 
 * @return the index of the last occurrence of the specified substring,
 *         searching backward from the specified index, or {@code -1} if there
 *         is no such occurrence.
 */
int string_last_index_of_v4(string_t *string, char *s, int from_index)
{
    return __last_index_of(string->value, string->length, s, strlen(s),
                           from_index);
}

/**
 * Returns the index within this string of the last occurrence of the
 * specified substring. 
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The substring to search for.
 * 
 * @return the index of the last occurrence of the specified substring, or
 *         {@code -1} if there is no such occurrence.
 */
int string_last_index_of_v5(string_t *string, string_t *s)
{
    return __last_index_of(string->value, string->length, s->value, s->length,
                           0);
}

/**
 * Returns the index within this string of the last occurrence of the specified
 * substring, searching backward starting at the specified index.
 * 
 * @param[in] string      The {@code string_t} object parameter.
 * @param[in] s           The substring to search for.
 * @param[in] from_index  The index to start the search from.
 * 
 * @return the index of the last occurrence of the specified substring,
 *         searching backward from the specified index, or {@code -1} if there
 *         is no such occurrence.
 */
int string_last_index_of_v6(string_t *string, string_t *s, int from_index)
{
    return __last_index_of(string->value, string->length, s->value, s->length,
                           from_index);
}

/**
 * Returns a string that is a substring of this string. The substring begins
 * with the character at the specified index and extends to the end of this
 * string.
 * 
 * @param[in] string       The {@code string_t} object parameter.
 * @param[in] begin_index  The beginning index, inclusive.
 * 
 * @return the specified substring or NULL if wasn't possible to allocate
 *         memory for the new string or {@code begin_index} is out of bound.
 */
string_t *string_substring(string_t *string, int begin_index)
{
    int count = string->length - begin_index;

    return string_value_of_v2(string->value, begin_index, count);
}

/**
 * Returns a string that is a substring of this string. The
 * substring begins at the specified {@code begin_index} and
 * extends to the character at index {@code end_index}.
 * 
 * @param[in] string       The {@code string_t} object parameter.
 * @param[in] begin_index  The beginning index, inclusive.
 * @param[in] end_index    The ending index, exclusive.
 * 
 * @return the specified substring or NULL if wasn't possible to allocate
 *         memory for the new string or {@code begin_index} is out of bound.
 */
string_t *string_substring_v2(string_t *string, int begin_index, int end_index)
{
    if (__check_bounds_off_count(begin_index, end_index, string->length))
        return -1;
    
    int count = end_index - begin_index + 1;

    char *scpy = (char *)calloc(count, sizeof(char));
    if (!scpy) return NULL;

    memcpy(scpy, string->value + begin_index, count);

    string_t *new_string = string_value_of_v2(scpy, 0, count);

    free(scpy);

    return new_string;
}

/**
 * Concatenates the specified string to the end of this string.
 * <p>
 * If the length of the argument string is {@code 0}, then a copy of
 * {@code string_t} object is returned. Otherwise, a {@code string_t}
 * object is returned that represents a character sequence that is the
 * concatenation of the character sequence represented by this {@code string_t}
 * object and the character sequence represented by the argument string.
 * </p>
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The {@code s} that is concatenated to the end
 *                    of this {@code string}.
 * 
 * @return a string that represents the concatenation of this object's
 *         characters followed by the string argument's characters.
 */
string_t *string_concat(string_t *string, char *s)
{
    string_t *new_string = NULL;
    int length = string->length + strlen(s) + 1;

    char *p = (char *)calloc(length, sizeof(char));
    if (!p) return NULL;

    strcat(p, (char *)string->value);
    strcat(p, s);

    new_string = string_value_of(p);

    free(p);

    return new_string;
}

/**
 * Concatenates the specified string to the end of this string.
 * <p>
 * If the length of the argument string is {@code 0}, then a copy of
 * {@code string_t} object is returned. Otherwise, a {@code string_t}
 * object is returned that represents a character sequence that is the
 * concatenation of the character sequence represented by this {@code string_t}
 * object and the character sequence represented by the argument string.
 * </p>
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The {@code s} that is concatenated to the end
 *                    of this {@code string}.
 * 
 * @return a string that represents the concatenation of this object's
 *         characters followed by the string argument's characters.
 */
string_t *string_concat_v2(string_t *string, string_t *s)
{
    return string_concat(string, (char *)s->value);
}

/**
 * Returns a string resulting from replacing all occurrences of {@code oldchar}
 * in this string with {@code newchar}.
 * <p>
 * If the character {@code oldchar} does not occur in the character sequence
 * represented by this {@code string} object, then a NULL reference is returned.
 * Otherwise, a new {@code string} object is returned that represents a
 * character sequence identical to the character sequence represented by this
 * {@code string} object, except that every occurrence of {@code oldchar} is
 * replaced by an occurrence of {@code newchar}.
 * <p>
 * 
 * @param[in] string   The {@code string_t} object parameter.
 * @param[in] oldchar  The old character.
 * @param[in] newchar  The new character.
 * 
 * @return a new string derived from this string by replacing every occurrence
 *         of {@code oldchar} with {@code newchar} or NULL if the occurrences
 *         does not occur.
 */
string_t *string_replace(string_t *string, char oldchar, char newchar)
{
    string_t *new_string = NULL;
    char     *buffer     = NULL;
    int       len        = string->length;
    int       i          = -1;

    while (++i < len) {
        if (string->value[i] == (byte)oldchar) break;
    }

    if (i < len) {
        buffer = (char *)calloc(len, sizeof(char));
        if (!buffer) return NULL;

        memcpy(buffer, string->value, len);

        while(i < len) {
            char c = (char)string->value[i];
            buffer[i] = (c == oldchar ? newchar : c);
            i++;
        }

        new_string = string_value_of_v2(buffer, 0, len);

        free(buffer);

        return new_string;
    }

    return NULL;
}

/**
 * Returns true if and only if this string contains the specified
 * sequence of char values.
 * 
 * @param[in] string  The {@code string_t} object parameter.
 * @param[in] s       The sequence to search for.
 * 
 * @return true if this string contains {@code s}, false otherwise.
  */
bool string_contains(string_t *string, char *s)
{
    return string_index_of_v3(string, s) >= 0;
}

/**
 * Splits this string around matches of the given delim.
 * 
 * @param[in]  string   The {@code string_t} object parameter.
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
int string_split(string_t *string, const char *delim, string_t **strings)
{
    int        count = 0, pos = 0;
    char      *token;
    char      *s = strdup((char *)string->value);
    char      *saveptr = s;
    string_t **strlist = NULL;

    if (!s) return -1;

    while ((token = strtok_r(saveptr, delim, &saveptr))) {
        count++;
    }

    if (count == 0 || !strings) goto end;

    free(s);

    s = strdup((char *)string->value);
    if (!s) return -1;

    saveptr = s;

    *strlist = (string_t *)calloc(count, sizeof(string_t *));
    if (!strlist) {
        count = -1;
        goto error;
    }

    pos = count;
    while ((token = strtok_r(saveptr, delim, &saveptr)) && pos < count) {
        strlist[pos] = string_new();
        if (!strlist[pos]) goto cleanup;

        if (!string_init(strlist[pos], token)) goto cleanup;

        pos++;
    }

    *strings = *strlist;

end:
    free(s);
    return count;

cleanup:
    for (pos = 0; pos < count && strlist[pos]; pos++)
        string_free(strlist[pos]);

error:
    free(s);
    return -1;
}

/**
 * Splits this string around matches of the given delim.
 * 
 * @param[in]  string   The {@code string_t} object parameter.
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
int string_split_v2(string_t *string, string_t *delim, string_t **strings)
{
    string_split(string, (char *)delim->value, strings);
}

string_t *string_to_lower_case(string_t *string)
{
    int len;
    char *buf = strdup((char *)string->value);
    if (!buf) return NULL;

    
    for (int i = 0; i < strlen(buf); buf++) 
        *tmp = toupper((unsigned char) *tmp);
    
    return s;

}

string_t *string_format(const char *format, ...)
{
    int       size = 0;
    char     *p = NULL;
    va_list   args;
    string_t *new_string = NULL;

    va_start(args, format);
    size = vsnprintf(p, size, format, args);
    va_end(args);

    if (size < 0) return NULL;

    size++;

    p = (char *)calloc(size, sizeof(char));
    if (!p) return NULL;

    va_start(args, format);
    size = vsnprintf(p, size, format, args);
    va_end(args);

    if (size < 0) {
        free(p);
        return NULL;
    }

    new_string = string_value_of(p);

    free(p);

    return new_string;
}

string_t *string_value_of(char *s)
{
    string_t *new_string = string_new();
    if (!new_string) return NULL;

    if (!string_init(new_string, s)) return NULL;

    return new_string;
}

string_t *string_value_of_v2(char *s, int offset, int count)
{
    string_t *new_string = string_new();
    if (!new_string) return NULL;

    if (!string_init_v3(new_string, s, offset, count)) {
        string_free(new_string);
        return NULL;
    }

    return new_string;
}

string_t *string_value_of_v8(object_t *object)
{
    if (strcmp(CLASS_TYPE, object->class_type) == 0)
        return (string_t *) object;
    else
        return NULL;
}
