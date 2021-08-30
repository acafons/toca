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
        int  from_index;
        int  expected;
} testcase;

testcase tc[] = {
        {"abcdefghij",    'a',  0,  0},
        {"abcdefghij",    'b',  0,  1},
        {"abcdefghij",    'c',  0,  2},
        {"abcdefghij",    'd',  0,  3},
        {"abcdefghij",    'e',  0,  4},
        {"abcdefghij",    'f',  0,  5},
        {"abcdefghij",    'g',  0,  6},
        {"abcdefghij",    'h',  0,  7},
        {"abcdefghij",    'i',  0,  8},
        {"abcdefghij",    'j',  0,  9},
        {"abcdefghij",    'b',  1,  1},
        {"abcdefghij",    'c',  2,  2},
        {"abcdefghij",    'd',  3,  3},
        {"abcdefghij",    'e',  4,  4},
        {"abcdefghij",    'f',  5,  5},
        {"abcdefghij",    'g',  6,  6},
        {"abcdefghij",    'h',  7,  7},
        {"abcdefghij",    'i',  8,  8},
        {"abcdefghij",    'j',  9,  9},
        {"abcdefghij",    'b',  0,  1},
        {"abcdefghij",    'c',  0,  2},
        {"abcdefghij",    'd',  1,  3},
        {"abcdefghij",    'e',  2,  4},
        {"abcdefghij",    'f',  3,  5},
        {"abcdefghij",    'g',  0,  6},
        {"abcdefghij",    'h',  1,  7},
        {"abcdefghij",    'i',  2,  8},
        {"abcdefghij",    'j',  3,  9},
        {"abcdefghij",    'k',  0, -1},
        {"abcdefghij",    'l',  1, -1},
        {"abcdefghij",    'm',  2, -1},
        {"abcdefghij",    'n',  3, -1},
        {"abcdefghij",    'o',  4, -1},
        {"abcdefghij",    'p',  5, -1},
        {"abcdefghij",    'q',  6, -1},
        {"abcdefghij",      1,  0, -1},
        {"abcdefghij",     10,  1, -1},
        {"abcdefghij",    100,  2,  3},
        {"abcdefghij",   1000,  3, -1},
        {"abcdefghij",  10000,  4, -1},
        {"abcdefghij", 100000,  5, -1},
        {"abcdefghij",    'x', -1, -1},
        {"abcdefghij",    'q', 10, -1},
};

static void __validate_string(const tstring* s, const testcase* tc)
{
        assert_int_equal(tstring_indexof_v2(s, tc->comparison, tc->from_index),
                         tc->expected);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->given);
        assert_non_null(s);

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_indexof_v2(void** state)
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
                cmocka_unit_test(__test_string_indexof_v2),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
