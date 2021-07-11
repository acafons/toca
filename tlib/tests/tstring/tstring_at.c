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


char *str = "ABCDEFGH";

static int __test_setup(void** state)
{
        tstring *s = tstring_new(str);

        if (!s) return -1;

        *state = s;
        return 0;
}

static int __test_teardown(void** state)
{
        tstring* s = (tstring*)*state;
        tstring_free(s);

        return 0;
}

static void __test_string_length(void** state)
{
        tstring* s = (tstring*)*state;
        assert_int_equal(tstring_length(s), strlen(str));
}

static void __test_equal_letters(void** state)
{
        tstring* s = (tstring*)*state;

        for (int i = 0; str[i] != '\0'; i++)
                assert_true(tstring_at(s, i) == str[i]);
}

static void __test_different_letters(void** state)
{
        tstring* s = (tstring*)*state;
        char *strdiff = "GHIJKL";

        for (int i = 0; strdiff[i] != '\0'; i++)
                assert_true(tstring_at(s, i) != strdiff[i]);
}

static void __test_access_negative_index(void** state)
{
        tstring* s = (tstring*)*state;

        assert_true(tstring_at(s, -1) == str[0]);
}

static void __test_access_index_out_of_bounds(void** state)
{
        tstring* s = (tstring*)*state;

        assert_true(tstring_at(s, strlen(str) + 1) == str[strlen(str) - 1]);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        __test_string_length,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_equal_letters,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_different_letters,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_access_negative_index,
                        __test_setup,
                        __test_teardown),
                cmocka_unit_test_setup_teardown(
                        __test_access_index_out_of_bounds,
                        __test_setup,
                        __test_teardown
                ),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
