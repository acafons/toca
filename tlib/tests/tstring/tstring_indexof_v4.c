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
        int  from_index;
        int  expected;
} testcase;

testcase tc[] = {
        {"abcdefghij",    "a",          0,  0},
        {"abcdefghij",    "b",          0,  1},
        {"abcdefghij",    "c",          0,  2},
        {"abcdefghij",    "d",          0,  3},
        {"abcdefghij",    "e",          0,  4},
        {"abcdefghij",    "f",          0,  5},
        {"abcdefghij",    "g",          0,  6},
        {"abcdefghij",    "h",          0,  7},
        {"abcdefghij",    "i",          0,  8},
        {"abcdefghij",    "j",          0,  9},
        {"abcdefghij",   "bc",          1,  1},
        {"abcdefghij",   "cd",          2,  2},
        {"abcdefghij",   "de",          3,  3},
        {"abcdefghij",   "ef",          4,  4},
        {"abcdefghij",   "fg",          5,  5},
        {"abcdefghij",   "gh",          6,  6},
        {"abcdefghij",   "hi",          7,  7},
        {"abcdefghij",   "ij",          8,  8},
        {"abcdefghij",   "jl",          9, -1},
        {"abcdefghij",   "bc",          0,  1},
        {"abcdefghij",   "cd",          0,  2},
        {"abcdefghij",   "de",          1,  3},
        {"abcdefghij",   "ef",          2,  4},
        {"abcdefghij",   "fg",          3,  5},
        {"abcdefghij",   "gh",          0,  6},
        {"abcdefghij",   "hi",          1,  7},
        {"abcdefghij",   "ij",          2,  8},
        {"abcdefghij",   "jl",          3, -1},
        {"abcdefghij",  "abc",          0,  0},
        {"abcdefghij",  "abc",          1, -1},
        {"abcdefghij",  "bcd",          0,  1},
        {"abcdefghij",  "bcd",          2, -1},
        {"abcdefghij",  "cde",          1,  2},
        {"abcdefghij",  "def",          2,  3},
        {"abcdefghij",  "efg",          3,  4},
        {"abcdefghij",  "fgh",          0,  5},
        {"abcdefghij",  "ghi",          1,  6},
        {"abcdefghij",  "hij",          2,  7},
        {"abcdefghij",  "ijl",          3, -1},
        {"abcdefghij ", " ",            0, 10},
        {"abcdefghij ", " ",            5, 10},
        {" abcdefghij", " ",            0,  0},
        {"abcdefgh   ", "   ",          6,  8},
        {"abcdefghij",  "abcdefghij",   0,  0},
        {"abcdefghij",  "abcdefghij",   1, -1},
        {"abcdefghij",  "abcdefghijk",  0, -1},
        {"abcdefghij",  "wxyz",        -1, -1},
        {"abcdefghij",  "wxyz",        10, -1},
};

static void __validate_string(const tstring* s, const testcase* tc)
{
        assert_int_equal(tstring_indexof_v4(s, tc->comparison, tc->from_index),
                         tc->expected);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->given);
        assert_non_null(s);

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_indexof_v4(void** state)
{
        for (size_t i = 0; i < sizeof(tc)/sizeof(tc[0]); i++)
        {
                printf("Test (%zu): given: %s, comparison: %s\n", i + 1,
                       tc[i].given, tc[i].comparison);
                
                __run_test_case(&tc[i]);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_indexof_v4),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
