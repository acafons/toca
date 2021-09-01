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
#include <stdio.h>


#define BUFFER_SIZE 101

static void __assert_str_non_null(tstring* s);
static void __assert_str_null(tstring* s);

typedef struct
{
        char given[BUFFER_SIZE];
        int offset;
        int count;
        void (*assert_str)(tstring*);
        char expected[BUFFER_SIZE];
} testcase;


testcase tc[] = {
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 12, 14, __assert_str_non_null, "MNOPQRSTUVWXYZ"},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 15,  5, __assert_str_non_null, "PQRST"},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", -1,  5, __assert_str_null,     ""},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  5, -1, __assert_str_null,     ""},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 27,  5, __assert_str_null,     ""},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 15, 26, __assert_str_null,     ""},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  0, 26, __assert_str_non_null, "ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  0, 27, __assert_str_null,     ""},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",  1, 26, __assert_str_null,     ""},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 21,  6, __assert_str_null,     ""}
};

static void __assert_str_non_null(tstring* s)
{
        assert_non_null(s);
}

static void __assert_str_null(tstring* s)
{
        assert_null(s);
}

static void __validate_string(const tstring* s, const char* data)
{
        assert_int_equal(tstring_length(s), strlen(data));
        assert_int_equal(tstring_compare(s, data), 0);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new_v3(tc->given, tc->offset, tc->count);
        tc->assert_str(s);

        if (!s) return;

        __validate_string(s, tc->expected);

        tstring_free(s);
}

static void __test_string_new_v3(void** state)
{
        for (size_t i = 0; i < sizeof(tc)/sizeof(tc[0]); i++)
        {
                printf("Test (%zu): given: %s, expected: %s\n", i + 1,
                       tc[i].given, tc[i].expected);

                __run_test_case(&tc[i]);
        }        
}

static void __test_null_argument(void** state)
{
        tstring* s = tstring_new_v3(NULL, 0, 0);
        assert_non_null(s);

        __validate_string(s, "");

        tstring_free(s);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_new_v3),
                cmocka_unit_test(__test_null_argument),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
