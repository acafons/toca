/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2020, acafons
 * All rights reserved.
 *******************************************************************************/

#include <tlib/tlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>


#define BUFFER_SIZE 101

static void __assert_str_non_null(const tstring* s);
static void __assert_str_null(const tstring* s);
static void __assert_cmp_true(const tstring*, const char*);
static void __assert_cmp_false(const tstring*, const char*);

typedef struct
{
        char given[BUFFER_SIZE];
        int  begin_index;
        int  end_index;
        char expected[BUFFER_SIZE];
        void (*assert_str)(const tstring*);
        void (*assert_cmp)(const tstring*, const char*);
} testcase;

testcase tc[] = {
        {"abcdefghijklmnopqrstuvwxyz",       0,      26, "abcdefghijklmnopqrstuvwxyz", __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       1,      26, "bcdefghijklmnopqrstuvwxyz",  __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       2,      26, "cdefghijklmnopqrstuvwxyz",   __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       3,      26, "defghijklmnopqrstuvwxyz",    __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       4,      26, "efghijklmnopqrstuvwxyz",     __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       5,      26, "fghijklmnopqrstuvwxyz",      __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       6,      26, "ghijklmnopqrstuvwxyz",       __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       7,      26, "hijklmnopqrstuvwxyz",        __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       8,      26, "ijklmnopqrstuvwxyz",         __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       9,      26, "jklmnopqrstuvwxyz",          __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      10,      26, "klmnopqrstuvwxyz",           __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      11,      26, "lmnopqrstuvwxyz",            __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      12,      26, "mnopqrstuvwxyz",             __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      13,      26, "nopqrstuvwxyz",              __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      14,      26, "opqrstuvwxyz",               __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      15,      26, "pqrstuvwxyz",                __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      16,      26, "qrstuvwxyz",                 __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      17,      26, "rstuvwxyz",                  __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      18,      26, "stuvwxyz",                   __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      19,      26, "tuvwxyz",                    __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      20,      26, "uvwxyz",                     __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      21,      26, "vwxyz",                      __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      22,      26, "wxyz",                       __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      23,      26, "xyz",                        __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      24,      26, "yz",                         __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      25,      26, "z",                          __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       0,      26, "bbcdefghijklmnopqrstuvwxyz", __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       1,      26, "ccdefghijklmnopqrstuvwxyz",  __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       2,      26, "ddefghijklmnopqrstuvwxyz",   __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       3,      26, "eefghijklmnopqrstuvwxyz",    __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       4,      26, "ffghijklmnopqrstuvwxyz",     __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       5,      26, "gghijklmnopqrstuvwxyz",      __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       6,      26, "hhijklmnopqrstuvwxyz",       __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       7,      26, "iijklmnopqrstuvwxyz",        __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       8,      26, "jjklmnopqrstuvwxyz",         __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       9,      26, "kklmnopqrstuvwxyz",          __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      10,      26, "llmnopqrstuvwxyz",           __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      11,      26, "mmnopqrstuvwxyz",            __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      12,      26, "nnopqrstuvwxyz",             __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      13,      26, "oopqrstuvwxyz",              __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      14,      26, "ppqrstuvwxyz",               __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      15,      26, "qqrstuvwxyz",                __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      16,      26, "rrstuvwxyz",                 __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      17,      26, "sstuvwxyz",                  __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      18,      26, "ttuvwxyz",                   __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      19,      26, "uuvwxyz",                    __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      20,      26, "vvwxyz",                     __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      21,      26, "wwxyz",                      __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      22,      26, "xxyz",                       __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      23,      26, "yyz",                        __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      24,      26, "zz",                         __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      25,      26, "a",                          __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       0,       3, "abc",                        __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       3,       6, "def",                        __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       6,       9, "ghi",                        __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       9,      12, "jkl",                        __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      12,      15, "mno",                        __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      15,      18, "pqr",                        __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      18,      21, "stu",                        __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      21,      24, "vwx",                        __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      24,      26, "yz",                         __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      26,       0, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",      26,      25, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",       0,      27, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",      25,      27, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",      26,      27, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",     100,       0, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",    1000,       0, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",   10000,       0, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",  100000,       0, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",       0,     100, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",       0,    1000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",       0,   10000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",       0,  100000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",     100,     100, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",    1000,    1000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",   10000,   10000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",  100000,  100000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",      -1,       0, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",     -10,       0, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",    -100,       0, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",   -1000,       0, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",  -10000,       0, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz", -100000,       0, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",       0,      -1, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",       0,     -10, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",       0,    -100, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",       0,   -1000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",       0,  -10000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",       0, -100000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",      -1,      -1, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",     -10,     -10, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",    -100,    -100, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",   -1000,   -1000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",  -10000,  -10000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz", -100000, -100000, "",                           __assert_str_null,     NULL},
};

static void __assert_str_non_null(const tstring* s)
{
        assert_non_null(s);
}

static void __assert_str_null(const tstring* s)
{
        assert_null(s);
}

static void __assert_cmp_true(const tstring* subs, const char* expected)
{
        assert_int_equal(tstring_compare(subs, expected), 0);
}

static void __assert_cmp_false(const tstring* subs, const char* expected)
{
        assert_int_not_equal(tstring_compare(subs, expected), 0);
}

static void __validate_string(const tstring* s, const testcase* tc)
{
        tstring *subs = tstring_substring_v2(s, tc->begin_index, tc->end_index);
        tc->assert_str(subs);

        if (subs) tc->assert_cmp(subs, tc->expected);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->given);
        assert_non_null(s);

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_substring(void** state)
{
        for (size_t i = 0; i < sizeof(tc)/sizeof(tc[0]); i++)
        {
                printf("Test (%zu): given: %s, expected: %s\n", i + 1,
                       tc[i].given, tc[i].expected);

                __run_test_case(&tc[i]);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_substring),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
