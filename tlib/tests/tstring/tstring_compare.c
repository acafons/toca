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
        char* s2 = "abcdef";

        assert_int_equal(tstring_compare(s1, s2), 0);

        tstring_free(s1);
}

static void __test_strings_case_sensitive(void** state)
{
        tstring* s1 = tstring_new("abcdef");
        char* s2 = "AbCdEf";

        assert_int_equal(tstring_compare(s1, s2), 32);

        tstring_free(s1);
}

static void __test_string_greater_than_other(void** state)
{
        tstring* s1 = tstring_new("abcdef");
        char* s2 = "abcdefyz";

        assert_int_equal(tstring_compare(s1, s2), -121);

        tstring_free(s1);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_equal_strings),
                cmocka_unit_test(__test_strings_case_sensitive),
                cmocka_unit_test(__test_string_greater_than_other),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}