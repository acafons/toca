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


static void __test_equal_strings(void** state)
{
        tstring* s1 = tstring_new("abcdef");
        tstring* s2 = tstring_new("abcdef");

        assert_true(tstring_equals(s1, s2));

        tstring_free(s1);
        tstring_free(s2);
}

static void __test_equal_string_pointers(void** state)
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

static void __test_strings_case_sensitive(void** state)
{
        tstring* s1 = tstring_new("abcdef");
        tstring* s2 = tstring_new("AbCdEf");

        assert_false(tstring_equals(s1, s2));

        tstring_free(s1);
        tstring_free(s2);
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
                cmocka_unit_test(__test_equal_strings),
                cmocka_unit_test(__test_equal_string_pointers),
                cmocka_unit_test(__test_string_and_null),
                cmocka_unit_test(__test_null_and_null),
                cmocka_unit_test(__test_strings_case_sensitive),
                cmocka_unit_test(__test_with_non_object_string),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
