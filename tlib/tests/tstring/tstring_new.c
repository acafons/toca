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
        char str[BUFFER_SIZE];
} testcase;

testcase tc[] = {
        {""},
        {"a"},
        {"ab"},
        {"abc"},
        {"abcd"},
        {"abcde"},
        {"abcdef"},
        {"abcdefg"},
        {"abcdefgh"},
        {"abcdefghi"},
        {"abcdefghij"},
        {"abcdefghijk"},
        {"abcdefghijkl"},
        {"abcdefghijklm"},
        {"abcdefghijklmn"},
        {"abcdefghijklmno"},
        {"abcdefghijklmnop"},
        {"abcdefghijklmnopq"},
        {"abcdefghijklmnopqr"},
        {"abcdefghijklmnopqrs"},
        {"abcdefghijklmnopqrst"},
        {"abcdefghijklmnopqrstu"},
        {"abcdefghijklmnopqrstuv"},
        {"abcdefghijklmnopqrstuvw"},
        {"abcdefghijklmnopqrstuvwx"},
        {"abcdefghijklmnopqrstuvwxy"},
        {"abcdefghijklmnopqrstuvwxyz"},
        {"klmnopqrstuvxwyabcdefghijklmnopqrstuvxwyabcdefghij" \
         "klmnopqrstuvxwyabcdefghijklmnopqrstuvxwyabcdefghij"},
};

static void __validate_string(tstring* s, const char* data)
{
        assert_int_equal(tstring_length(s), strlen(data));
        assert_int_equal(tstring_compare(s, data), 0);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->str);
        assert_non_null(s);

        __validate_string(s, tc->str);

        tstring_free(s);
}

static void __test_string_new(void** state)
{
        for (size_t i = 0; i < sizeof(tc)/sizeof(tc[0]); i++)
        {
                printf("Test (%zu): given: %s\n", i + 1, tc[i].str);
                __run_test_case(&tc[i]);
        }        
}

static void __test_null_argument(void** state)
{
        tstring* s = tstring_new(NULL);
        assert_non_null(s);

        __validate_string(s, "");

        tstring_free(s);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_new),
                cmocka_unit_test(__test_null_argument),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
