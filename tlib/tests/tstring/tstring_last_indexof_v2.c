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
        {"abcdefghij",    'a',  9,  0},
        {"abcdefghij",    'b',  9,  1},
        {"abcdefghij",    'c',  9,  2},
        {"abcdefghij",    'd',  9,  3},
        {"abcdefghij",    'e',  9,  4},
        {"abcdefghij",    'f',  9,  5},
        {"abcdefghij",    'g',  9,  6},
        {"abcdefghij",    'h',  9,  7},
        {"abcdefghij",    'i',  9,  8},
        {"abcdefghij",    'j',  9,  9},
        {"abcdefghij",    'b',  1,  1},
        {"abcdefghij",    'c',  2,  2},
        {"abcdefghij",    'd',  3,  3},
        {"abcdefghij",    'e',  4,  4},
        {"abcdefghij",    'f',  5,  5},
        {"abcdefghij",    'g',  6,  6},
        {"abcdefghij",    'h',  7,  7},
        {"abcdefghij",    'i',  8,  8},
        {"abcdefghij",    'j',  9,  9},
        {"abcdefghij",    'b',  2,  1},
        {"abcdefghij",    'c',  2,  2},
        {"abcdefghij",    'd',  5,  3},
        {"abcdefghij",    'e',  6,  4},
        {"abcdefghij",    'f',  7,  5},
        {"abcdefghij",    'g',  7,  6},
        {"abcdefghij",    'h',  8,  7},
        {"abcdefghij",    'i',  9,  8},
        {"abcdefghij",    'j', 10,  9},
        {"abcdefghij",    'k',  9, -1},
        {"abcdefghij",    'l',  8, -1},
        {"abcdefghij",    'm',  7, -1},
        {"abcdefghij",    'n',  6, -1},
        {"abcdefghij",    'o',  5, -1},
        {"abcdefghij",    'p',  4, -1},
        {"abcdefghij",    'q',  3, -1},
        {"aaaaaaaaaa",    'a',  9,  9},
        {"baaaaaaaaj",    'a',  8,  8},
        {"bcaaaaaaij",    'a',  7,  7},
        {"bcdaaaahij",    'a',  6,  6},
        {"bcdeaaghij",    'a',  5,  5},
        {"bcdeafghij",    'a',  4,  4},
        {"aaaaaaaaaa",    'a',  9,  9},
        {"baaaaaaaaj",    'a',  9,  8},
        {"bcaaaaaaij",    'a',  9,  7},
        {"bcdaaaahij",    'a',  9,  6},
        {"bcdeaaghij",    'a',  9,  5},
        {"bcdeafghij",    'a',  9,  4},
        {"abcdefghij",      1,  0, -1},
        {"abcdefghij",     10,  1, -1},
        {"abcdefghij",    100,  2, -1},
        {"abcdefghij",   1000,  3, -1},
        {"abcdefghij",  10000,  4, -1},
        {"abcdefghij", 100000,  5, -1},
        {"abcdefghij",    'x', -1, -1},
        {"abcdefghij",    'q', 10, -1},
};

static void __validate_string(const tstring* s, const testcase* tc)
{
        assert_int_equal(tstring_last_indexof_v2(s, tc->comparison,
                         tc->from_index), tc->expected);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->given);
        assert_non_null(s);

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_last_indexof_v2(void** state)
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
                cmocka_unit_test(__test_string_last_indexof_v2),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
