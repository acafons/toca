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
        {"abcdefghij", "a",           1},
        {"abcdefghij", "ab",          1},
        {"abcdefghij", "abc",         1},
        {"abcdefghij", "abcd",        1},
        {"abcdefghij", "abcdefghi",   1},
        {"abcdefghij", "abcdefghij",  1},
        {"ababababab", "ab",          5},
        {"abcdefgcde", "cde",         2},
        {"abcdefghcd", "cde",         1},
        {"aaaaaaaaaa", "a",          10},
        {"abcdefghij", "a",           1},
        {"abcdefghij", "j",           1},
        {"abcdefghij", "qr",          0},
};

static void __validate_string(const tstring* s, const testcase* tc)
{
        assert_int_equal(tstring_count_ocurrences_of(s, tc->comparison),
                                                     tc->expected);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->given);
        assert_non_null(s);

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_count_ocurrences_of(void** state)
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
                cmocka_unit_test(__test_string_count_ocurrences_of),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
