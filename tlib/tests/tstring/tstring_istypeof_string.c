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


static void __test_a_non_object_string(void** state)
{
        assert_false(tstring_istypeof_string((tobject*)"A"));
}

static void __test_an_object_string(void** state)
{
        tstring* s = tstring_new("A");
        assert_true(tstring_istypeof_string((tobject*)s));
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_a_non_object_string),
                cmocka_unit_test(__test_an_object_string),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
