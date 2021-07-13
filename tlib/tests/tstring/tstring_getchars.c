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

static int __test_setup(void** state)
{
        tstring* s = tstring_new(str);
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

static void __test_string_negative_src_begin(void** state)
{
        char dst[10] = {0};
        tstring* s = (tstring*)*state;

        int r = tstring_getchars(s, -1, 9, dst, 0, 9);
        assert_int_equal(r, -1);
}

static void __test_string_negative_src_end(void** state)
{
        char dst[10] = {0};
        tstring* s = (tstring*)*state;

        int r = tstring_getchars(s, 0, -1, dst, 0, 9);
        assert_int_equal(r, -1);
}

static void __test_string_negative_dst_begin(void** state)
{
        char dst[10] = {0};
        tstring* s = (tstring*)*state;

        int r = tstring_getchars(s, 0, 9, dst, -1, 9);
        assert_int_equal(r, -1);
}

static void __test_string_negative_dst_end(void** state)
{
        char dst[10] = {0};
        tstring* s = (tstring*)*state;

        int r = tstring_getchars(s, 0, 9, dst, 0, -1);
        assert_int_equal(r, -1);
}

static void __test_string_src_begin_greater_end(void** state)
{
        char dst[10] = {0};
        tstring* s = (tstring*)*state;

        int r = tstring_getchars(s, 5, 2, dst, 0, 9);
        assert_int_equal(r, -1);
}

static void __test_string_src_begin_equal_end(void** state)
{
        char dst[16] = {0};
        tstring* s = (tstring*)*state;

        int r = tstring_getchars(s, 5, 5, dst, 0, 15);
        assert_int_equal(r, 0);
}

static void __test_string_get_all_chars(void** state)
{
        char dst[27] = {0};
        tstring* s = (tstring*)*state;

        int r = tstring_getchars(s, 0, 26, dst, 0, 26);
        assert_int_equal(r, 26);
        assert_int_equal(strcmp(str, dst), 0);
}

static void __test_string_get_first_chars(void** state)
{
        char dst[6] = {0};
        tstring* s = (tstring*)*state;

        int r = tstring_getchars(s, 0, 5, dst, 0, 5);
        assert_int_equal(r, 5);
        assert_int_equal(strncmp(str, dst, 5), 0);
}

static void __test_string_get_middle_chars(void** state)
{
        char dst[6] = {0};
        tstring* s = (tstring*)*state;

        int r = tstring_getchars(s, 10, 15, dst, 0, 5);
        assert_int_equal(r, 5);
        assert_int_equal(strncmp(str + 10, dst, 5), 0);
}

static void __test_string_get_last_chars(void** state)
{
        char dst[6] = {0};
        tstring* s = (tstring*)*state;

        int r = tstring_getchars(s, 21, 26, dst, 0, 5);
        assert_int_equal(r, 5);
        assert_int_equal(strncmp(str + 21, dst, 5), 0);
}

static void __test_string_get_chopped_chars(void** state)
{
        char dst[16] = {0};
        tstring* s = (tstring*)*state;

        int r = tstring_getchars(s, 0, 5, dst, 0, 15);
        assert_int_equal(r, 5);
        assert_string_equal("ABCDE", dst);

        r = tstring_getchars(s, 10, 15, dst, 5, 15);
        assert_int_equal(r, 5);
        assert_string_equal("ABCDEKLMNO", dst);

        r = tstring_getchars(s, 20, 25, dst, 10, 15);
        assert_int_equal(r, 5);
        assert_string_equal("ABCDEKLMNOUVWXY", dst);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test_setup_teardown(
                        __test_string_negative_src_begin,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_string_negative_src_end,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_string_negative_dst_begin,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_string_negative_dst_end,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_string_src_begin_greater_end,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_string_src_begin_equal_end,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_string_get_all_chars,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_string_get_first_chars,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_string_get_middle_chars,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_string_get_last_chars,
                        __test_setup,
                        __test_teardown
                ),
                cmocka_unit_test_setup_teardown(
                        __test_string_get_chopped_chars,
                        __test_setup,
                        __test_teardown
                ),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
