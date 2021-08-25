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
#include <stdio.h>


#define BUFFER_SIZE 51

typedef struct
{
        char given[BUFFER_SIZE];
        int  src_begin;
        int  src_end;
        int  dst_begin;
        int  dst_len;
        bool reuse_dst;
        char str_expected[BUFFER_SIZE];
        int  ret_expected;
} testcase;

testcase tc[] = {
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", -1,  9,  0,  9, false, "", -1},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  0, -1,  0,  9, false, "", -1},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  0,  9, -1,  9, false, "", -1},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  0,  9,  0, -1, false, "", -1},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  5,  2,  0,  9, false, "", -1},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  5,  5,  0, 15, false, "",  0},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  0, 26,  0, 26, false, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  0,  5,  0,  5, false, "ABCDE",  5},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 10, 15,  0,  5, false, "KLMNO",  5},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 21, 26,  0,  5, false, "VWXYZ",  5},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  0,  4,  0, 26, false, "ABCD",  4},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  4,  8,  4, 26, true,  "ABCDEFGH",  4},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  8, 12,  8, 26, true,  "ABCDEFGHIJKL",  4},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 12, 16, 12, 26, true,  "ABCDEFGHIJKLMNOP",  4},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 16, 20, 16, 26, true,  "ABCDEFGHIJKLMNOPQRST",  4},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 20, 24, 20, 26, true,  "ABCDEFGHIJKLMNOPQRSTUVWX",  4},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 24, 26, 24, 26, true,  "ABCDEFGHIJKLMNOPQRSTUVWXYZ",  2},
        {"abcdefghijklmnopqrstuvwxyz", -1,  9,  0,  9, false, "", -1},
        {"abcdefghijklmnopqrstuvwxyz",  0, -1,  0,  9, false, "", -1},
        {"abcdefghijklmnopqrstuvwxyz",  0,  9, -1,  9, false, "", -1},
        {"abcdefghijklmnopqrstuvwxyz",  0,  9,  0, -1, false, "", -1},
        {"abcdefghijklmnopqrstuvwxyz",  5,  2,  0,  9, false, "", -1},
        {"abcdefghijklmnopqrstuvwxyz",  5,  5,  0, 15, false, "",  0},
        {"abcdefghijklmnopqrstuvwxyz",  0, 26,  0, 26, false, "abcdefghijklmnopqrstuvwxyz", 26},
        {"abcdefghijklmnopqrstuvwxyz",  0,  5,  0,  5, false, "abcde",  5},
        {"abcdefghijklmnopqrstuvwxyz", 10, 15,  0,  5, false, "klmno",  5},
        {"abcdefghijklmnopqrstuvwxyz", 21, 26,  0,  5, false, "vwxyz",  5},
        {"abcdefghijklmnopqrstuvwxyz",  0,  4,  0, 26, false, "abcd",  4},
        {"abcdefghijklmnopqrstuvwxyz",  4,  8,  4, 26, true,  "abcdefgh",  4},
        {"abcdefghijklmnopqrstuvwxyz",  8, 12,  8, 26, true,  "abcdefghijkl",  4},
        {"abcdefghijklmnopqrstuvwxyz", 12, 16, 12, 26, true,  "abcdefghijklmnop",  4},
        {"abcdefghijklmnopqrstuvwxyz", 16, 20, 16, 26, true,  "abcdefghijklmnopqrst",  4},
        {"abcdefghijklmnopqrstuvwxyz", 20, 24, 20, 26, true,  "abcdefghijklmnopqrstuvwx",  4},
        {"abcdefghijklmnopqrstuvwxyz", 24, 26, 24, 26, true,  "abcdefghijklmnopqrstuvwxyz",  2},
};

/* is_dst_reusable: returns true if the previous dst should be reused. */
static bool __is_dst_reusable(const testcase* tc, char* dst)
{
        return tc->reuse_dst && dst;
}

static char* __create_dst(const testcase* tc, char* dst)
{
        if (__is_dst_reusable(tc, dst)) return dst;
        if (dst) free(dst);

        return (char*)calloc(sizeof(char), tc->dst_len + 1);
}

static void __validate_string_getchars(const tstring* s, const testcase* tc,
                                       char* dst)
{
        int r = tstring_getchars(s, tc->src_begin, tc->src_end, dst,
                                 tc->dst_begin, tc->dst_len);
        assert_int_equal(r, tc->ret_expected);
        assert_int_equal(strcmp(dst, tc->str_expected), 0);
}

static void __run_test_case(const testcase* tc, char** dst)
{
        tstring* s = tstring_new(tc->given);
        *dst = __create_dst(tc, *dst);

        assert_non_null(s && *dst);

        __validate_string_getchars(s, tc, *dst);

        tstring_free(s);
}

static void __test_string_getchars(void** state)
{
        char* dst = NULL;

        for (size_t i = 0; i < sizeof(tc)/sizeof(tc[0]); i++)
        {
                printf("Test (%zu): given: %s, comparison: %s\n", i + 1,
                       tc[i].given, tc[i].str_expected);

                __run_test_case(&tc[i], &dst);
        }

        free(dst);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_getchars),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
