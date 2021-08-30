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
        int  comparison;
        int  expected;
} testcase;

testcase tc[] = {
        {"abcdefghij",    'a',  0},
        {"abcdefghij",    'b',  1},
        {"abcdefghij",    'c',  2},
        {"abcdefghij",    'd',  3},
        {"abcdefghij",    'e',  4},
        {"abcdefghij",    'f',  5},
        {"abcdefghij",    'g',  6},
        {"abcdefghij",    'h',  7},
        {"abcdefghij",    'i',  8},
        {"abcdefghij",    'j',  9},
        {"abcdefghij",    'k', -1},
        {"abcdefghij",    'l', -1},
        {"abcdefghij",    'm', -1},
        {"abcdefghij",    'n', -1},
        {"abcdefghij",    'o', -1},
        {"abcdefghij",    'p', -1},
        {"abcdefghij",    'q', -1},
        {"abcdefghij",      1, -1},
        {"abcdefghij",     10, -1},
        {"abcdefghij",    100,  3},
        {"abcdefghij",   1000, -1},
        {"abcdefghij",  10000, -1},
        {"abcdefghij", 100000, -1},
};

static void __validate_string(const tstring* s, const testcase* tc)
{
        assert_int_equal(tstring_indexof(s, tc->comparison), tc->expected);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->given);
        assert_non_null(s);

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_indexof(void** state)
{
        for (size_t i = 0; i < sizeof(tc)/sizeof(tc[0]); i++)
        {
                printf("Test (%zu): given: %s, comparison: %i\n", i + 1,
                       tc[i].given, tc[i].comparison);
                
                __run_test_case(&tc[i]);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_indexof),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
