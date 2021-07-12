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


static void __test_string_not_empty(void** state)
{
        char* str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        tstring* s = tstring_new(str);

        assert_non_null(s);
        assert_int_equal(tstring_length(s), strlen(str));
}

static void __test_string_empty(void** state)
{
        tstring* s = tstring_new_v4();

        assert_non_null(s);
        assert_int_equal(tstring_length(s), 0);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_not_empty),
                cmocka_unit_test(__test_string_empty),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
