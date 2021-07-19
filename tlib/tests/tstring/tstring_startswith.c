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

static void __test_string_startwith(void** state)
{
        for (size_t i = 0; i < sizeof(st)/sizeof(st[0]); i++)
        {
                tstring* s = tstring_new(st[i].given);
                assert_non_null(s);

                printf("Test string: given: %s, comparison: %s\n", st[i].given,
                       st[i].comparison);

                assert_int_equal(tstring_startswith(s, st[i].comparison),
                                 st[i].expected);
                tstring_free(s);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_startwith),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
