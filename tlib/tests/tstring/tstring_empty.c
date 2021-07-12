/* SPDX-License-Identifier: BSD-2-Clause */
/*******************************************************************************
 * Copyright 2020, acafons
 * All rights reserved.
 *******************************************************************************/

#include <tlib/tlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <cmocka.h>


static void __test_string_not_empty(void** state)
{
        tstring* s = tstring_new("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

        assert_non_null(s);
        assert_false(tstring_isempty(s));

        tstring_free(s);
}

static void __test_string_empty(void** state)
{
        tstring* s = tstring_new_v4();

        assert_non_null(s);
        assert_true(tstring_isempty(s));

        tstring_free(s);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_not_empty),
                cmocka_unit_test(__test_string_empty),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
