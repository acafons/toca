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
        int expected;
} stringtest;

stringtest st[] = {
        {"abcdefghij", "a",            98},
        {"abcdefghij", "ab",           99},
        {"abcdefghij", "abc",         100},
        {"abcdefghij", "abcd",        101},
        {"abcdefghij", "abcdefghij",    0},
        {"abcdefghij", "abcdffghij",   -1},
        {"abcdefghij", "abc ffghij",   68},
        {"a",          "abcdefghij",  -98},
        {"ab",         "abcdefghij",  -99},
        {"abc",        "abcdefghij", -100},
        {"abcd",       "abcdefghij", -101},
        {"abcdefghij", "A",            32},
        {"abcdefghij", "aB",           32},
        {"abcdefghij", "ABC",          32},
        {"abcdefghij", "abcD",         32},
        {"abcdefghij", "ABCDEFGHIJ",   32},
        {"abcdefghij", "ABCDFFGHIJ",   32},
        {"abcdefghij", "ABC FFGHIJ",   32},
        {"A",          "abcdefghij",  -32},
        {"aB",         "abcdefghij",  -32},
        {"abC",        "abcdefghij",  -32},
        {"abcD",       "abcdefghij",  -32},
};

static void __test_string_compare(void** state)
{
        for (size_t i = 0; i < sizeof(st)/sizeof(st[0]); i++)
        {
                tstring* s = tstring_new(st[i].given);
                assert_non_null(s);

                printf("Test string: given: %s, comparison: %s\n", st[i].given,
                       st[i].comparison);

                assert_int_equal(tstring_compare(s, st[i].comparison),
                                 st[i].expected);

                tstring_free(s);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_compare),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
