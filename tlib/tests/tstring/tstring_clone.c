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
#include <stdlib.h>


typedef struct
{
        tstring* original;
        tstring* clone;
} stringtest;

static stringtest* __create_strings_to_test(stringtest* st)
{
        if (!(st->original = tstring_new("FGHIJKLMNOPQRSTUVWXYZ")))
                return NULL;

        if (!(st->clone = tstring_clone(st->original)))
        {
                tstring_free(st->original);
                return NULL;
        }

        return st;
}

static int __test_setup(void** state)
{
        stringtest* st = (stringtest*)malloc(sizeof(stringtest));
        if (!st) return -1;

        if (!(*state = __create_strings_to_test(st)))
        {
                free(st);
                return -1;
        }

        return 0;
}

static int __test_teardown(void** state)
{
        stringtest *st = (stringtest*)*state;

        tstring_free(st->original);
        tstring_free(st->clone);
        free(st);

        return 0;
}

static void __test_string_length(void** state)
{
        stringtest *st = (stringtest*)*state;
        assert_int_equal(tstring_length(st->original),
                         tstring_length(st->clone));
}

static void __test_string_equals(void** state)
{
        stringtest *st = (stringtest*)*state;
        assert_true(tstring_equals(st->original, st->clone));
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
                        __test_string_equals,
                        __test_setup,
                        __test_teardown
                ),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
