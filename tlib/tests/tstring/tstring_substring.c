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
        char expected[BUFFER_SIZE];
        void (*assert_str)(const tstring*);
        void (*assert_cmp)(const tstring*, const char*);
} testcase;

testcase tc[] = {
        {"abcdefghijklmnopqrstuvwxyz",       0, "abcdefghijklmnopqrstuvwxyz", __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       1, "bcdefghijklmnopqrstuvwxyz",  __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       2, "cdefghijklmnopqrstuvwxyz",   __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       3, "defghijklmnopqrstuvwxyz",    __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       4, "efghijklmnopqrstuvwxyz",     __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       5, "fghijklmnopqrstuvwxyz",      __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       6, "ghijklmnopqrstuvwxyz",       __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       7, "hijklmnopqrstuvwxyz",        __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       8, "ijklmnopqrstuvwxyz",         __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       9, "jklmnopqrstuvwxyz",          __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      10, "klmnopqrstuvwxyz",           __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      11, "lmnopqrstuvwxyz",            __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      12, "mnopqrstuvwxyz",             __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      13, "nopqrstuvwxyz",              __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      14, "opqrstuvwxyz",               __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      15, "pqrstuvwxyz",                __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      16, "qrstuvwxyz",                 __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      17, "rstuvwxyz",                  __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      18, "stuvwxyz",                   __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      19, "tuvwxyz",                    __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      20, "uvwxyz",                     __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      21, "vwxyz",                      __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      22, "wxyz",                       __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      23, "xyz",                        __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      24, "yz",                         __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",      25, "z",                          __assert_str_non_null, __assert_cmp_true},
        {"abcdefghijklmnopqrstuvwxyz",       0, "bbcdefghijklmnopqrstuvwxyz", __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       1, "ccdefghijklmnopqrstuvwxyz",  __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       2, "ddefghijklmnopqrstuvwxyz",   __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       3, "eefghijklmnopqrstuvwxyz",    __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       4, "ffghijklmnopqrstuvwxyz",     __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       5, "gghijklmnopqrstuvwxyz",      __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       6, "hhijklmnopqrstuvwxyz",       __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       7, "iijklmnopqrstuvwxyz",        __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       8, "jjklmnopqrstuvwxyz",         __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",       9, "kklmnopqrstuvwxyz",          __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      10, "llmnopqrstuvwxyz",           __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      11, "mmnopqrstuvwxyz",            __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      12, "nnopqrstuvwxyz",             __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      13, "oopqrstuvwxyz",              __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      14, "ppqrstuvwxyz",               __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      15, "qqrstuvwxyz",                __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      16, "rrstuvwxyz",                 __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      17, "sstuvwxyz",                  __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      18, "ttuvwxyz",                   __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      19, "uuvwxyz",                    __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      20, "vvwxyz",                     __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      21, "wwxyz",                      __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      22, "xxyz",                       __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      23, "yyz",                        __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      24, "zz",                         __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      25, "a",                          __assert_str_non_null, __assert_cmp_false},
        {"abcdefghijklmnopqrstuvwxyz",      26, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",     100, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",    1000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",   10000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",  100000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",      -1, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",     -10, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",    -100, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",   -1000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz",  -10000, "",                           __assert_str_null,     NULL},
        {"abcdefghijklmnopqrstuvwxyz", -100000, "",                           __assert_str_null,     NULL},
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
        tstring *subs = tstring_substring(s, tc->begin_index);
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
