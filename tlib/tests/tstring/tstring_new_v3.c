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

char* str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static void __test_string_offset(void** state)
{
        char *expected = "MNOPQRSTUVWXYZ";

        tstring* s = tstring_new_v3(str, 12, 14);

        assert_non_null(s);
        assert_int_equal(tstring_length(s), strlen(expected));
        assert_int_equal(tstring_compare(s, expected), 0);

        tstring_free(s);
}

static void __test_string_offset_count(void** state)
{
        char* expected = "PQRST";

        tstring* s = tstring_new_v3(str, 15, 5);

        assert_non_null(s);
        assert_int_equal(tstring_length(s), strlen(expected));
        assert_int_equal(tstring_compare(s, expected), 0);

        tstring_free(s);
}

static void __test_string_offset_negative(void** state)
{
        tstring* s = tstring_new_v3(str, -1, 5);
        assert_null(s);
}

static void __test_string_count_negative(void** state)
{
        tstring* s = tstring_new_v3(str, 5, -1);
        assert_null(s);
}

static void __test_string_offset_out_of_bound(void** state)
{
        tstring* s = tstring_new_v3(str, 27, 5);
        assert_null(s);
}

static void __test_string_count_out_of_bound(void** state)
{
        tstring* s = tstring_new_v3(str, 15, 26);
        assert_null(s);
}

static void __test_string_full(void** state)
{
        tstring* s = tstring_new_v3(str, 0, 26);

        assert_non_null(s);
        assert_int_equal(tstring_length(s), strlen(str));
        assert_int_equal(tstring_compare(s, str), 0);

        tstring_free(s);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_offset),
                cmocka_unit_test(__test_string_offset_count),
                cmocka_unit_test(__test_string_offset_negative),
                cmocka_unit_test(__test_string_count_negative),
                cmocka_unit_test(__test_string_offset_out_of_bound),
                cmocka_unit_test(__test_string_count_out_of_bound),
                cmocka_unit_test(__test_string_full),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
