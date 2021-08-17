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
#include <stdint.h>


#define BUFFER_SIZE 101

typedef struct
{
        char str[BUFFER_SIZE];
        int  hashcode;
        bool expect;
} testcase;

testcase tc[] = {
        {"",                                     0, true },
        {"",                                     1, false},
        {"a",                                   97, true },
        {"a",                                   98, false},
        {"ab",                                3105, true },
        {"ab",                                3106, false},
        {"abc",                              96354, true },
        {"abc",                              96355, false},
        {"abcd",                           2987074, true },
        {"abcd",                           2987075, false},
        {"abcde",                         92599395, true },
        {"abcde",                         92599396, false},
        {"abcdef",                      2870581347, true },
        {"abcdef",                      2870581348, false},
        {"abcdefg",                    -1206291356, true },
        {"abcdefg",                    -1206291355, false},
        {"abcdefgh",                    1259673732, true },
        {"abcdefgh",                    1259673733, false},
        {"abcdefghi",                    395180133, true },
        {"abcdefghi",                    395180134, false},
        {"abcdefghij",                  -634317659, true },
        {"abcdefghij",                  -634317658, false},
        {"abcdefghijk",                 1810989158, true },
        {"abcdefghijk",                 1810989159, false},
        {"abcdefghijkl",                 306089158, true },
        {"abcdefghijkl",                 306089159, false},
        {"abcdefghijklm",                898829415, true },
        {"abcdefghijklm",                898829414, false},
        {"abcdefghijklmn",              2093908199, true },
        {"abcdefghijklmn",              2093908198, false},
        {"abcdefghijklmno",              486644840, true },
        {"abcdefghijklmno",              486644841, false},
        {"abcdefghijklmnop",           -2093879032, true },
        {"abcdefghijklmnop",           -2093879033, false},
        {"abcdefghijklmnopq",           -485740439, true },
        {"abcdefghijklmnopq",           -485740438, false},
        {"abcdefghijklmnopqr",          2121915689, true },
        {"abcdefghijklmnopqr",          2121915688, false},
        {"abcdefghijklmnopqrs",         1354877034, true },
        {"abcdefghijklmnopqrs",         1354877035, false},
        {"abcdefghijklmnopqrst",        -948484790, true },
        {"abcdefghijklmnopqrst",        -948484791, false},
        {"abcdefghijklmnopqrstu",        661742699, true },
        {"abcdefghijklmnopqrstu",        661742690, false},
        {"abcdefghijklmnopqrstuv",      -960812693, true },
        {"abcdefghijklmnopqrstuv",      -960812692, false},
        {"abcdefghijklmnopqrstuvw",      279577708, true },
        {"abcdefghijklmnopqrstuvw",      279577707, false},
        {"abcdefghijklmnopqrstuvwx",      76974476, true },
        {"abcdefghijklmnopqrstuvwx",      76974477, false},
        {"abcdefghijklmnopqrstuvwxy",  -1908758419, true },
        {"abcdefghijklmnopqrstuvwxy",  -1908758410, false},
        {"abcdefghijklmnopqrstuvwxyz",   958031277, true },
        {"abcdefghijklmnopqrstuvwxyz",   958031278, false},
        {"klmnopqrstuvxwyabcdefghijklmnopqrstuvxwyabcdefghij" \
         "klmnopqrstuvxwyabcdefghijklmnopqrstuvxwyabcdefghij", -1915261888, true},
        {"klmnopqrstuvxwyabcdefghijklmnopqrstuvxwyabcdefghij" \
         "klmnopqrstuvxwyabcdefghijklmnopqrstuvxwyabcdefghij", -1915261880, false},
};

static void __validate_string(tstring* s, const testcase* tc)
{
        assert_int_equal((tstring_hashcode(s)) == tc->hashcode, tc->expect);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->str);
        assert_non_null(s);

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_hashcode(void** state)
{
        for (size_t i = 0; i < sizeof(tc)/sizeof(tc[0]); i++)
        {
                printf("Test (%li): given: %s\n", i + 1, tc[i].str);
                __run_test_case(&tc[i]);
        }        
}

static void __test_null_argument(void** state)
{
        tstring* s = tstring_new(NULL);
        assert_non_null(s);

        __validate_string(s, &tc[0]);

        tstring_free(s);
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_hashcode),
                cmocka_unit_test(__test_null_argument),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
