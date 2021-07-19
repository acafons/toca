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


#define BUFFER_SIZE 20

typedef struct
{
        char given[BUFFER_SIZE];
        char comparison[BUFFER_SIZE];
        int offset;
        bool expected;
} stringtest;

stringtest st[] = {
        {"abcdefghijk", "a",          0, true},
        {"abcdefghijk", "ab",         0, true},
        {"abcdefghijk", "abc",        0, true},
        {"abcdefghijk", "abcd",       0, true},
        {"abcdefghijk", "abcdefghij", 0, true},
        {"abcdefghijk", "k",          0, false},
        {"abcdefghijk", "kl",         0, false},
        {"abcdefghijk", "klm",        1, false},
        {"mnopqrstuvx", "a",          2, false},
        {"mnopqrstuvx", "ab",         3, false},
        {"mnopqrstuvx", "abc",        4, false},
        {"abcdefghijk", "b",          1, true},
        {"abcdefghijk", "cd",         2, true},
        {"abcdefghijk", "efg",        4, true},
        {"abcdefghijk", "hijk",       7, true},
};

static void __test_string_startswith_v4(void** state)
{
        for (size_t i = 0; i < sizeof(st)/sizeof(st[0]); i++)
        {
                tstring* s1 = tstring_new(st[i].given);
                tstring* s2 = tstring_new(st[i].comparison);
                assert_non_null(s1 && s2);

                printf("Test string: given: %s, comparison: %s\n", st[i].given,
                       st[i].comparison);

                assert_int_equal(tstring_startswith_v4(s1, s2, st[i].offset),
                                 st[i].expected);
                tstring_free(s1);
                tstring_free(s2);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_startswith_v4),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
