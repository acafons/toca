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
        {"abcdefghij", "a",          true},
        {"abcdefghij", "ab",         true},
        {"abcdefghij", "abc",        true},
        {"abcdefghij", "abcd",       true},
        {"abcdefghij", "abcdefghij", true},
        {"abcdefghij", "k",          false},
        {"abcdefghij", "kl",         false},
        {"abcdefghij", "klm",        false},
        {"mnopqrstuv", "a",          false},
        {"mnopqrstuv", "ab",         false},
        {"mnopqrstuv", "abc",        false}
};

static void __validate_string(const tstring* s1, const tstring* s2,
                              const testcase* tc)
{
        assert_int_equal(tstring_startswith_v3(s1, s2), tc->expected);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s1 = tstring_new(tc->given);
        tstring* s2 = tstring_new(tc->comparison);
        assert_non_null(s1 && s2);

        __validate_string(s1, s2, tc);
        
        tstring_free(s1);
        tstring_free(s2);
}

static void __test_string_startswith_v3(void** state)
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
                cmocka_unit_test(__test_string_startswith_v3),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
