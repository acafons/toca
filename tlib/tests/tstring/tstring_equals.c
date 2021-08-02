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


#define BUFFER_SIZE 51

typedef struct
{
        char given[BUFFER_SIZE];
        char comparison[BUFFER_SIZE];
        bool expected;
} testcase;

testcase tc[] = {
        {"abcdefghij", "a",          false},
        {"abcdefghij", "ab",         false},
        {"abcdefghij", "abc",        false},
        {"abcdefghij", "abcd",       false},
        {"abcdefghij", "abcdefghij", true},
        {"abcdefghij", "abcdffghij", false},
        {"abcdefghij", "abc ffghij", false},
        {"a",          "abcdefghij", false},
        {"ab",         "abcdefghij", false},
        {"abc",        "abcdefghij", false},
        {"abcd",       "abcdefghij", false},
        {"abcdefghij", "A",          false},
        {"abcdefghij", "aB",         false},
        {"abcdefghij", "ABC",        false},
        {"abcdefghij", "abcD",       false},
        {"abcdefghij", "ABCDEFGHIJ", false},
        {"abcdefghij", "ABCDFFGHIJ", false},
        {"abcdefghij", "ABC FFGHIJ", false},
        {"A",          "abcdefghij", false},
        {"aB",         "abcdefghij", false},
        {"abC",        "abcdefghij", false},
        {"abcD",       "abcdefghij", false},
        {"klmnopqrstuvxwyabcdefghijklmnopqrstuvxwyabcdefghij",
         "klmnopqrstuvxwyabcdefghijklmnopqrstuvxwyabcdefghij",
         true},
        {"klmnopqrstuvxwyaBcdefghijklmnopqrstuvxwyabcdefghij",
         "klmnopqrstuvxwyabcdefghijKlmnopqrstuvxwyabcDefghij",
         false},
};

static void __validate_string(const tstring* s1, const tstring* s2,
                              const testcase* tc)
{
        assert_int_equal(tstring_equals(s1, s2), tc->expected);
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

static void __test_string_equals(void** state)
{
        for (size_t i = 0; i < sizeof(tc)/sizeof(tc[0]); i++)
        {
                printf("Test (%li): given: %s, comparison: %s\n", i + 1,
                       tc[i].given, tc[i].comparison);

                __run_test_case(&tc[i]);
        }        
}

static void __test_string_equals_pointers(void** state)
{
        tstring* s1 = tstring_new("abcdef");

        assert_true(tstring_equals(s1, s1));

        tstring_free(s1);
}

static void __test_string_and_null(void** state)
{
        tstring* s1 = tstring_new("abcdef");

        assert_false(tstring_equals(s1, NULL));

        tstring_free(s1);
}

static void __test_null_and_null(void** state)
{
        assert_true(tstring_equals(NULL, NULL));
}

static void __test_with_non_object_string(void** state)
{
        tstring* s1 = tstring_new("abcdef");
        char* s2 = "abcdef";

        assert_false(tstring_equals(s1, (tstring*)s2));

        tstring_free(s1);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_equals),
                cmocka_unit_test(__test_string_equals_pointers),
                cmocka_unit_test(__test_string_and_null),
                cmocka_unit_test(__test_null_and_null),
                cmocka_unit_test(__test_with_non_object_string),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
