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


static void __test_string_free(void** state)
{
        tstring* s = tstring_new("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
        assert_non_null(s);

        tstring_free(s);
        s = NULL;
        
        tstring_free(s);
        assert_null(s);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_free),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
