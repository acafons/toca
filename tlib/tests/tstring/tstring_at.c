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
        char comparison;
        int index;
        bool expected;
} stringtest;

stringtest st[] = {
        {"abcdefghij", 'a', -10, true},
        {"abcdefghij", 'a',  -1, true},
        {"abcdefghij", 'a',   0, true},
        {"abcdefghij", 'b',   1, true},
        {"abcdefghij", 'c',   2, true},
        {"abcdefghij", 'd',   3, true},
        {"abcdefghij", 'e',   4, true},
        {"abcdefghij", 'f',   5, true},
        {"abcdefghij", 'g',   6, true},
        {"abcdefghij", 'h',   7, true},
        {"abcdefghij", 'i',   8, true},
        {"abcdefghij", 'j',   9, true},
        {"abcdefghij", 'j',  10, true},
        {"abcdefghij", 'j',  20, true},
        {"abcdefghij", 'A',   0, false},
        {"abcdefghij", 'B',   1, false},
        {"abcdefghij", 'C',   2, false},
        {"abcdefghij", 'D',   3, false},
        {"abcdefghij", 'J',   9, false},
        {"abcdefghij", 'I',   8, false},
        {"abcdefghij", 'H',   7, false},
        {"abcdefghij", 'G',   6, false},
};

static void __test_string_at(void** state)
{
        for (size_t i = 0; i < sizeof(st)/sizeof(st[0]); i++)
        {
                tstring* s = tstring_new(st[i].given);
                assert_non_null(s);

                printf("Test string: given: %s, comparison: %c\n", st[i].given,
                       st[i].comparison);

                assert_int_equal(tstring_at(s, st[i].index) == st[i].comparison,
                                 st[i].expected);

                tstring_free(s);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_at),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
