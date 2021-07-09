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

char *str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

static int __test_setup(void** state)
{
        tstring* s = tstring_new(str);

        if (!s) return -1;

        *state = s;
        return 0;
}

static int __test_teardown(void **state)
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

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        __test_string_length,
                        __test_setup,
                        __test_teardown
                ),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
