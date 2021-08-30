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
        int  expected;
} testcase;

testcase tc[] = {
        {"abcdefghij",  "a",            0},
        {"abcdefghij",  "b",            1},
        {"abcdefghij",  "c",            2},
        {"abcdefghij",  "d",            3},
        {"abcdefghij",  "e",            4},
        {"abcdefghij",  "f",            5},
        {"abcdefghij",  "g",            6},
        {"abcdefghij",  "h",            7},
        {"abcdefghij",  "i",            8},
        {"abcdefghij",  "j",            9},
        {"abcdefghij",  "ab",           0},
        {"abcdefghij",  "bc",           1},
        {"abcdefghij",  "cd",           2},
        {"abcdefghij",  "de",           3},
        {"abcdefghij",  "ef",           4},
        {"abcdefghij",  "fg",           5},
        {"abcdefghij",  "gh",           6},
        {"abcdefghij",  "hi",           7},
        {"abcdefghij",  "ij",           8},
        {"abcdefghij",  "jl",          -1},
        {"abcdefghij",  "abc",          0},
        {"abcdefghij",  "bcd",          1},
        {"abcdefghij",  "cde",          2},
        {"abcdefghij",  "def",          3},
        {"abcdefghij",  "efg",          4},
        {"abcdefghij",  "fgh",          5},
        {"abcdefghij",  "ghi",          6},
        {"abcdefghij",  "hij",          7},
        {"abcdefghij",  "ijl",         -1},
        {"abcdefghij",  "jlm",         -1},
        {"abcdefghij",  "mno",         -1},
        {"ab cdefghij", "abc",         -1},
        {"abc defghij", "bcd",         -1},
        {"abcd efghij", "cde",         -1},
        {"abcde fghij", "def",         -1},
        {"abcdef ghij", "efg",         -1},
        {"abcdefg hij", "fgh",         -1},
        {"abcdefgh ij", "ghi",         -1},
        {"abcdefghi j", "hij",         -1},
        {"abcdefghij ", " ",           10},
        {" abcdefghij", " ",            0},
        {"abcdefgh   ", "   ",          8},
        {"abcdefghij",  "abcdefghij",   0},
        {"abcdefghij",  "abcdefghijk", -1},
};

static void __validate_string(const tstring* s, const testcase* tc)
{
        assert_int_equal(tstring_indexof_v3(s, tc->comparison), tc->expected);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->given);
        assert_non_null(s);

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_indexof_v3(void** state)
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
                cmocka_unit_test(__test_string_indexof_v3),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
