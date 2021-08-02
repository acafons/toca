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
        int expected;
} testcase;

testcase tc[] = {
        {"abcdefghij", "a",            98},
        {"abcdefghij", "ab",           99},
        {"abcdefghij", "abc",         100},
        {"abcdefghij", "abcd",        101},
        {"abcdefghij", "abcdefghij",    0},
        {"abcdefghij", "abcdffghij",   -1},
        {"abcdefghij", "abc ffghij",   68},
        {"a",          "abcdefghij",  -98},
        {"ab",         "abcdefghij",  -99},
        {"abc",        "abcdefghij", -100},
        {"abcd",       "abcdefghij", -101},
        {"abcdefghij", "A",            98},
        {"abcdefghij", "AB",           99},
        {"abcdefghij", "ABC",         100},
        {"abcdefghij", "ABCD",        101},
        {"abcdefghij", "ABCDEFGHIJ",    0},
        {"abcdefghij", "ABCDFFGHIJ",   -1},
        {"abcdefghij", "ABC FFGHIJ",   68},
        {"a",          "ABCDEFGHIJ",  -98},
        {"ab",         "ABCDEFGHIJ",  -99},
        {"abc",        "ABCDEFGHIJ", -100},
        {"abcd",       "ABCDEFGHIJ", -101},
};

static void __validate_string(const tstring* s, const testcase* tc)
{
        assert_int_equal(tstring_compare_ignorecase(s, tc->comparison),
                         tc->expected);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->given);
        assert_non_null(s);

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_compare_ignorecase(void** state)
{
        for (size_t i = 0; i < sizeof(tc)/sizeof(tc[0]); i++)
        {
                printf("Test (%li): given: %s, comparison: %s\n", i + 1,
                       tc[i].given, tc[i].comparison);

                __run_test_case(&tc[i]);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_compare_ignorecase),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
