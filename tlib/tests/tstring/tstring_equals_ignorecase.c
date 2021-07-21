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
} stringtest;

stringtest st[] = {
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
        {"abcdefghij", "ABCDEFGHIJ", true},
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
         true},
};

static void __test_string_equals_ignorecase(void** state)
{
        for (size_t i = 0; i < sizeof(st)/sizeof(st[0]); i++)
        {
                tstring* s1 = tstring_new(st[i].given);
                tstring* s2 = tstring_new(st[i].comparison);
                assert_non_null(s1 && s2);

                printf("Test string: given: %s, comparison: %s\n", st[i].given,
                       st[i].comparison);

                assert_int_equal(tstring_equals_ignorecase(s1, s2),
                                 st[i].expected);

                tstring_free(s1);
                tstring_free(s2);
        }        
}

static void __test_equal_string_pointers(void** state)
{
        tstring* s1 = tstring_new("abcdef");

        assert_true(tstring_equals_ignorecase(s1, s1));

        tstring_free(s1);
}

static void __test_string_and_null(void** state)
{
        tstring* s1 = tstring_new("abcdef");

        assert_false(tstring_equals_ignorecase(s1, NULL));

        tstring_free(s1);
}

static void __test_null_and_string(void** state)
{
        tstring* s1 = tstring_new("abcdef");

        assert_false(tstring_equals_ignorecase(NULL, s1));

        tstring_free(s1);
}

static void __test_null_and_null(void** state)
{
        assert_true(tstring_equals_ignorecase(NULL, NULL));
}

static void __test_with_non_object_string(void** state)
{
        tstring* s1 = tstring_new("abcdef");
        char* s2 = "abcdef";

        assert_false(tstring_equals_ignorecase(s1, (tstring*)s2));

        tstring_free(s1);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_equals_ignorecase),
                cmocka_unit_test(__test_equal_string_pointers),
                cmocka_unit_test(__test_string_and_null),
                cmocka_unit_test(__test_null_and_string),
                cmocka_unit_test(__test_null_and_null),
                cmocka_unit_test(__test_with_non_object_string),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
