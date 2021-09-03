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
        char initstr[BUFFER_SIZE];
        char oldchar;
        char newchar;
        void (*assert_str)(tstring*);
        char teststr[BUFFER_SIZE];
        bool isequal;
} testcase;

testcase tc[] = {
        {"abcdefghijklmnopqrstuvwxyz", 'a', 'c', __assert_str_non_null, "cbcdefghijklmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'b', 'd', __assert_str_non_null, "adcdefghijklmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'c', 'e', __assert_str_non_null, "abedefghijklmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'd', 'f', __assert_str_non_null, "abcfefghijklmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'e', 'g', __assert_str_non_null, "abcdgfghijklmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'f', 'h', __assert_str_non_null, "abcdehghijklmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'g', 'i', __assert_str_non_null, "abcdefihijklmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'h', 'j', __assert_str_non_null, "abcdefgjijklmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'i', 'k', __assert_str_non_null, "abcdefghkjklmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'j', 'l', __assert_str_non_null, "abcdefghilklmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'k', 'm', __assert_str_non_null, "abcdefghijmlmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'l', 'n', __assert_str_non_null, "abcdefghijknmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'm', 'o', __assert_str_non_null, "abcdefghijklonopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'n', 'p', __assert_str_non_null, "abcdefghijklmpopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'o', 'q', __assert_str_non_null, "abcdefghijklmnqpqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'p', 'r', __assert_str_non_null, "abcdefghijklmnorqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'q', 's', __assert_str_non_null, "abcdefghijklmnopsrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'r', 't', __assert_str_non_null, "abcdefghijklmnopqtstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 's', 'u', __assert_str_non_null, "abcdefghijklmnopqrutuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 't', 'v', __assert_str_non_null, "abcdefghijklmnopqrsvuvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'u', 'w', __assert_str_non_null, "abcdefghijklmnopqrstwvwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'v', 'x', __assert_str_non_null, "abcdefghijklmnopqrstuxwxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'w', 'y', __assert_str_non_null, "abcdefghijklmnopqrstuvyxyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'x', 'z', __assert_str_non_null, "abcdefghijklmnopqrstuvwzyz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'y', 'a', __assert_str_non_null, "abcdefghijklmnopqrstuvwxaz", true},
        {"abcdefghijklmnopqrstuvwxyz", 'z', 'b', __assert_str_non_null, "abcdefghijklmnopqrstuvwxyb", true},
        {"aaaaefghijklmnopqrstuvwxyz", 'a', '1', __assert_str_non_null, "1111efghijklmnopqrstuvwxyz", true},
        {"abcdefghijklmnopqrstuvwaaa", 'a', '1', __assert_str_non_null, "1bcdefghijklmnopqrstuvw111", true},
        {"aaaaefghijklmnopqrstuvwaaa", 'a', '1', __assert_str_non_null, "1111efghijklmnopqrstuvw111", true},
        {"aaaaefghijkaaaopqrstuvwaaa", 'a', '1', __assert_str_non_null, "1111efghijk111opqrstuvw111", true},
        {"aaaaaaaaaaaaaaaaaaaaaaaaaa", 'a', 'z', __assert_str_non_null, "zzzzzzzzzzzzzzzzzzzzzzzzzz", true},
        {"aaaaaaaaaaaaaaaaaaaaaaaaaa", 'k', 'c', __assert_str_null,     "",                           true},
        {"00011122233344455566677788", '2', '3', __assert_str_non_null, "00011133333344455566677788", true},
        {"abcdefghijklmnopqrstuvwxyz", 'a', 'c', __assert_str_non_null, "cbcdefghijklmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'b', 'd', __assert_str_non_null, "adcdefghijklmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'c', 'e', __assert_str_non_null, "abedefghijklmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'd', 'f', __assert_str_non_null, "abcfefghijklmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'e', 'g', __assert_str_non_null, "abcdgfghijklmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'f', 'h', __assert_str_non_null, "abcdehghijklmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'g', 'i', __assert_str_non_null, "abcdefihijklmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'h', 'j', __assert_str_non_null, "abcdefgjijklmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'i', 'k', __assert_str_non_null, "abcdefghkjklmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'j', 'l', __assert_str_non_null, "abcdefghilklmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'k', 'm', __assert_str_non_null, "abcdefghijmlmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'l', 'n', __assert_str_non_null, "abcdefghijknmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'm', 'o', __assert_str_non_null, "abcdefghijklonopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'n', 'p', __assert_str_non_null, "abcdefghijklmpopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'o', 'q', __assert_str_non_null, "abcdefghijklmnqpqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'p', 'r', __assert_str_non_null, "abcdefghijklmnorqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'q', 's', __assert_str_non_null, "abcdefghijklmnopsrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'r', 't', __assert_str_non_null, "abcdefghijklmnopqtstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 's', 'u', __assert_str_non_null, "abcdefghijklmnopqrutuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 't', 'v', __assert_str_non_null, "abcdefghijklmnopqrsvuvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'u', 'w', __assert_str_non_null, "abcdefghijklmnopqrstwvwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'v', 'x', __assert_str_non_null, "abcdefghijklmnopqrstuxwxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'w', 'y', __assert_str_non_null, "abcdefghijklmnopqrstuvyxy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'x', 'z', __assert_str_non_null, "abcdefghijklmnopqrstuvwzy2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'y', 'a', __assert_str_non_null, "abcdefghijklmnopqrstuvwxa2", false},
        {"abcdefghijklmnopqrstuvwxyz", 'z', 'b', __assert_str_non_null, "abcdefghijklmnopqrstuvwxy2", false},
        {"aaaaefghijklmnopqrstuvwxyz", 'a', '1', __assert_str_non_null, "1111efghijklmnopqrstuvwxy2", false},
        {"abcdefghijklmnopqrstuvwaaa", 'a', '1', __assert_str_non_null, "1bcdefghijklmnopqrstuvw112", false},
        {"aaaaefghijklmnopqrstuvwaaa", 'a', '1', __assert_str_non_null, "1111efghijklmnopqrstuvw112", false},
        {"aaaaefghijkaaaopqrstuvwaaa", 'a', '1', __assert_str_non_null, "1111efghijk111opqrstuvw112", false},
        {"aaaaaaaaaaaaaaaaaaaaaaaaaa", 'a', 'z', __assert_str_non_null, "zzzzzzzzzzzzzzzzzzzzzzzzz2", false},
        {"00011122233344455566677788", '2', '3', __assert_str_non_null, "00011133333344455566677782", false},
        {""                          , 'a', 'b', __assert_str_null,     "",                           true},
};

static void __assert_str_non_null(tstring* s)
{
        assert_non_null(s);
}

static void __assert_str_null(tstring* s)
{
        assert_null(s);
}

static void __validate_string(const tstring* s, const testcase* tc)
{
        tstring* newstr = tstring_replace(s, tc->oldchar, tc->newchar);
        tc->assert_str(newstr);

        if (!newstr) return;

        if (tc->isequal)
                assert_int_equal(tstring_compare(newstr, tc->teststr), 0);
        else
                assert_int_not_equal(tstring_compare(newstr, tc->teststr), 0);

        tstring_free(newstr);
}

static void __run_test_case(const testcase* tc)
{
        tstring* s = tstring_new(tc->initstr);
        assert_non_null(s);

        if (!s) return;

        __validate_string(s, tc);

        tstring_free(s);
}

static void __test_string_concat(void** state)
{
        for (size_t i = 0; i < sizeof(tc)/sizeof(tc[0]); i++)
        {
                printf("Test (%zu): initstr: %s, oldchar: %c, newchar: %c\n",
                       i + 1, tc[i].initstr, tc[i].oldchar, tc[i].newchar);

                __run_test_case(&tc[i]);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_concat),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
