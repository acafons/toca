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
        bool expected;
} stringtest;

stringtest st[] = {
        {"abcdefghij", "a",          true},
        {"abcdefghij", "ab",         true},
        {"abcdefghij", "abc",        true},
        {"abcdefghij", "abcd",       true},
        {"abcdefghij", "abcdefghij", true},
        {"abcdefghij", "k",          false},
        {"abcdefghij", "kl",         false},
        {"abcdefghij", "klm",        false},
        {"mnopqrstuv", "a",          false},
        {"mnopqrstuv", "ab",         false},
        {"mnopqrstuv", "abc",        false}
};

static void __test_string_startswith_v3(void** state)
{
        for (size_t i = 0; i < sizeof(st)/sizeof(st[0]); i++)
        {
                tstring* s1 = tstring_new(st[i].given);
                tstring* s2 = tstring_new(st[i].comparison);
                assert_non_null(s1 && s2);

                printf("Test string: given: %s, comparison: %s\n", st[i].given,
                       st[i].comparison);

                assert_int_equal(tstring_startswith_v3(s1, s2), st[i].expected);
                tstring_free(s1);
                tstring_free(s2);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_startswith_v3),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
