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
} stringtest;

stringtest st[] = {
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

static tstring* __create_string(char* data)
{
        tstring* s1 = tstring_new(data);
        assert_non_null(s1);

        tstring* s2 = tstring_new_v2(s1);

        tstring_free(s1);

        return s2;
}

static void __validate_string(tstring* s, stringtest* st)
{
        assert_int_equal(tstring_length(s), strlen(st->str));
        assert_int_equal(tstring_compare(s, st->str), 0);
}

static void __test_string_new_v2(void** state)
{
        for (size_t i = 0; i < sizeof(st)/sizeof(st[0]); i++)
        {
                printf("Test (%li): given: %s\n", i + 1, st[i].str);

                tstring* s = __create_string(st[i].str);
                assert_non_null(s);

                __validate_string(s, &st[i]);

                tstring_free(s);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_new_v2),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
