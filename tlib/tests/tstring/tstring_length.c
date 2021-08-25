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

typedef struct
{
        char given[BUFFER_SIZE];
        int expected;
} testcase;

testcase tc[] = {
        {"", 0},
        {"a", 1},
        {"ab", 2},
        {"abc", 3},
        {"abcd", 4},
        {"abcde", 5},
        {"abcdef", 6},
        {"abcdefg", 7},
        {"abcdefgh", 8},
        {"abcdefghi", 9},
        {"abcdefghij", 10},
        {"abcdefghijk", 11},
        {"abcdefghijkl", 12},
        {"abcdefghijklm", 13},
        {"abcdefghijklmn", 14},
        {"abcdefghijklmno", 15},
        {"abcdefghijklmnop", 16},
        {"abcdefghijklmnopq", 17},
        {"abcdefghijklmnopqr", 18},
        {"abcdefghijklmnopqrs", 19},
        {"abcdefghijklmnopqrst", 20},
        {"abcdefghijklmnopqrstu", 21},
        {"abcdefghijklmnopqrstuv", 22},
        {"abcdefghijklmnopqrstuvw", 23},
        {"abcdefghijklmnopqrstuvwx", 24},
        {"abcdefghijklmnopqrstuvwxy", 25},
        {"abcdefghijklmnopqrstuvwxyz", 26},
        {"klmnopqrstuvxwyabcdefghijklmnopqrstuvxwyabcdefghij" \
         "klmnopqrstuvxwyabcdefghijklmnopqrstuvxwyabcdefghij",
         100},
};

static void __validate_string(const tstring* s, const testcase* tc)
{
        assert_int_equal(tstring_length(s), tc->expected);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->given);
        assert_non_null(s);

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_length(void** state)
{
        for (size_t i = 0; i < sizeof(tc)/sizeof(tc[0]); i++)
        {
                printf("Test (%zu): given: %s, expected: %i\n", i + 1,
                       tc[i].given, tc[i].expected);

                __run_test_case(&tc[i]);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_length),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
