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
        {"abcdefghij",    "a",          9,  0},
        {"abcdefghij",    "b",          9,  1},
        {"abcdefghij",    "c",          9,  2},
        {"abcdefghij",    "d",          9,  3},
        {"abcdefghij",    "e",          9,  4},
        {"abcdefghij",    "f",          9,  5},
        {"abcdefghij",    "g",          9,  6},
        {"abcdefghij",    "h",          9,  7},
        {"abcdefghij",    "i",          9,  8},
        {"abcdefghij",    "j",          9,  9},
        {"abcdefghij",   "bc",          1,  1},
        {"abcdefghij",   "cd",          2,  2},
        {"abcdefghij",   "de",          3,  3},
        {"abcdefghij",   "ef",          4,  4},
        {"abcdefghij",   "fg",          5,  5},
        {"abcdefghij",   "gh",          6,  6},
        {"abcdefghij",   "hi",          7,  7},
        {"abcdefghij",   "ij",          8,  8},
        {"abcdefghij",   "jl",          9, -1},
        {"abcdefghij",   "bc",          9,  1},
        {"abcdefghij",   "cd",          9,  2},
        {"abcdefghij",   "de",          7,  3},
        {"abcdefghij",   "ef",          8,  4},
        {"abcdefghij",   "fg",          9,  5},
        {"abcdefghij",   "gh",          9,  6},
        {"abcdefghij",   "hi",          8,  7},
        {"abcdefghij",   "ij",          7, -1},
        {"abcdefghij",   "jl",          6, -1},
        {"abcdefghij",  "abc",          9,  0},
        {"abcdefghij",  "abc",          1,  0},
        {"abcabcghij",  "abc",          2,  0},
        {"abcdefghij",  "bcd",          9,  1},
        {"abcdefghij",  "bcd",          2,  1},
        {"abcdefghij",  "cde",          9,  2},
        {"abcdefghij",  "def",          8,  3},
        {"abcdefghij",  "efg",          6,  4},
        {"abcdefghij",  "fgh",          9,  5},
        {"abcdefghij",  "ghi",          8,  6},
        {"abcdefghij",  "hij",          7,  7},
        {"abcdefghij",  "ijl",         10, -1},
        {"abcdefghij ", " ",           10, 10},
        {"abcdefghij ", " ",            9, -1},
        {" abcdefghij", " ",           10,  0},
        {"abcdefgh   ", "   ",         10,  8},
        {"abcdefghij",  "abcdefghij",   9,  0},
        {"abcdefghij",  "abcdefghij",   5,  0},
        {"abcdefghij",  "abcdefghijk",  9, -1},
        {"abcdefghij",  "wxyz",        -1, -1},
        {"abcdefghij",  "wxyz",        10, -1},
        {"abcabcabcj",  "abc",          9,  6},
};

static void __validate_string(const tstring* s, const testcase* tc)
{
        assert_int_equal(tstring_last_indexof_v4(s, tc->comparison,
                         tc->from_index), tc->expected);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->given);
        assert_non_null(s);

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_last_indexof_v4(void** state)
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
                cmocka_unit_test(__test_string_last_indexof_v4),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
