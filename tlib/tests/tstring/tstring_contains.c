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
} testcase;

testcase tc[] = {
        {"abcdefghij",  "a",           true},
        {"abcdefghij",  "b",           true},
        {"abcdefghij",  "c",           true},
        {"abcdefghij",  "d",           true},
        {"abcdefghij",  "e",           true},
        {"abcdefghij",  "f",           true},
        {"abcdefghij",  "g",           true},
        {"abcdefghij",  "h",           true},
        {"abcdefghij",  "i",           true},
        {"abcdefghij",  "j",           true},
        {"abcdefghij",  "ab",          true},
        {"abcdefghij",  "bc",          true},
        {"abcdefghij",  "cd",          true},
        {"abcdefghij",  "de",          true},
        {"abcdefghij",  "ef",          true},
        {"abcdefghij",  "fg",          true},
        {"abcdefghij",  "gh",          true},
        {"abcdefghij",  "hi",          true},
        {"abcdefghij",  "ij",          true},
        {"abcdefghij",  "jl",          false},
        {"abcdefghij",  "abc",         true},
        {"abcdefghij",  "bcd",         true},
        {"abcdefghij",  "cde",         true},
        {"abcdefghij",  "def",         true},
        {"abcdefghij",  "efg",         true},
        {"abcdefghij",  "fgh",         true},
        {"abcdefghij",  "ghi",         true},
        {"abcdefghij",  "hij",         true},
        {"abcdefghij",  "ijl",         false},
        {"abcdefghij",  "jlm",         false},
        {"abcdefghij",  "mno",         false},
        {"ab cdefghij", "abc",         false},
        {"abc defghij", "bcd",         false},
        {"abcd efghij", "cde",         false},
        {"abcde fghij", "def",         false},
        {"abcdef ghij", "efg",         false},
        {"abcdefg hij", "fgh",         false},
        {"abcdefgh ij", "ghi",         false},
        {"abcdefghi j", "hij",         false},
        {"abcdefghij ", " ",           true},
        {" abcdefghij", " ",           true},
        {"abcdefgh   ", "   ",         true},
        {"abcdefghij",  "abcdefghij",  true},
        {"abcdefghij",  "abcdefghijk", false},
        {"defabcabcj",  "abc",         true},
};

static void __validate_string(const tstring* s, const testcase* tc)
{
        assert_int_equal(tstring_contains(s, tc->comparison), tc->expected);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->given);
        assert_non_null(s);

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_contains(void** state)
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
                cmocka_unit_test(__test_string_contains),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
