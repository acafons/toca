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
        char initstr[BUFFER_SIZE];
        char concat[BUFFER_SIZE];
        char teststr[BUFFER_SIZE];
        bool isequal;
} testcase;

testcase tc[] = {
        {"abcdefghijklmnopqrstuvwxyz", "a",                          "abcdefghijklmnopqrstuvwxyza",                           true},
        {"abcdefghijklmnopqrstuvwxyz", "ab",                         "abcdefghijklmnopqrstuvwxyzab",                          true},
        {"abcdefghijklmnopqrstuvwxyz", "abc",                        "abcdefghijklmnopqrstuvwxyzabc",                         true},
        {"abcdefghijklmnopqrstuvwxyz", "abcd",                       "abcdefghijklmnopqrstuvwxyzabcd",                        true},
        {"abcdefghijklmnopqrstuvwxyz", "abcde",                      "abcdefghijklmnopqrstuvwxyzabcde",                       true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdef",                     "abcdefghijklmnopqrstuvwxyzabcdef",                      true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefg",                    "abcdefghijklmnopqrstuvwxyzabcdefg",                     true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefgh",                   "abcdefghijklmnopqrstuvwxyzabcdefgh",                    true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghi",                  "abcdefghijklmnopqrstuvwxyzabcdefghi",                   true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghij",                 "abcdefghijklmnopqrstuvwxyzabcdefghij",                  true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijk",                "abcdefghijklmnopqrstuvwxyzabcdefghijk",                 true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijkl",               "abcdefghijklmnopqrstuvwxyzabcdefghijkl",                true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklm",              "abcdefghijklmnopqrstuvwxyzabcdefghijklm",               true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmn",             "abcdefghijklmnopqrstuvwxyzabcdefghijklmn",              true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmno",            "abcdefghijklmnopqrstuvwxyzabcdefghijklmno",             true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnop",           "abcdefghijklmnopqrstuvwxyzabcdefghijklmnop",            true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopq",          "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopq",           true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqr",         "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqr",          true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrs",        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrs",         true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrst",       "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrst",        true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstu",      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstu",       true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuv",     "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuv",      true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvw",    "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvw",     true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwx",   "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwx",    true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwxy",  "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxy",   true},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",  true},
        {"abcdefghijklmnopqrstuvwxyz", "a",                          "abcdefghijklmnopqrstuvwxyzab",                          false},
        {"abcdefghijklmnopqrstuvwxyz", "ab",                         "abcdefghijklmnopqrstuvwxyzabc",                         false},
        {"abcdefghijklmnopqrstuvwxyz", "abc",                        "abcdefghijklmnopqrstuvwxyzabcd",                        false},
        {"abcdefghijklmnopqrstuvwxyz", "abcd",                       "abcdefghijklmnopqrstuvwxyzabcde",                       false},
        {"abcdefghijklmnopqrstuvwxyz", "abcde",                      "abcdefghijklmnopqrstuvwxyzabcdef",                      false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdef",                     "abcdefghijklmnopqrstuvwxyzabcdefg",                     false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefg",                    "abcdefghijklmnopqrstuvwxyzabcdefgh",                    false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefgh",                   "abcdefghijklmnopqrstuvwxyzabcdefghi",                   false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghi",                  "abcdefghijklmnopqrstuvwxyzabcdefghij",                  false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghij",                 "abcdefghijklmnopqrstuvwxyzabcdefghijk",                 false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijk",                "abcdefghijklmnopqrstuvwxyzabcdefghijkl",                false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijkl",               "abcdefghijklmnopqrstuvwxyzabcdefghijklm",               false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklm",              "abcdefghijklmnopqrstuvwxyzabcdefghijklmn",              false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmn",             "abcdefghijklmnopqrstuvwxyzabcdefghijklmno",             false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmno",            "abcdefghijklmnopqrstuvwxyzabcdefghijklmnop",            false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnop",           "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopq",           false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopq",          "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqr",          false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqr",         "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrs",         false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrs",        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrst",        false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrst",       "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstu",       false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstu",      "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuv",      false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuv",     "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvw",     false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvw",    "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwx",    false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwx",   "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxy",   false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwxy",  "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz",  false},
        {"abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza", false},
        {"0123456789",                 "9876543210",                 "01234567899876543210",                                  true},
        {"",                           "",                           "",                                                      true},
};

static void __validate_string(const tstring* sorigem, const tstring* sappend,
                              const testcase* tc)
{
        tstring* newstr = tstring_concat_v2(sorigem, sappend);
        assert_non_null(newstr);

        if (tc->isequal)
                assert_int_equal(tstring_compare(newstr, tc->teststr), 0);
        else
                assert_int_not_equal(tstring_compare(newstr, tc->teststr), 0);

        tstring_free(newstr);
}

static void __run_test_case(const testcase* tc)
{
        tstring* sorigem = tstring_new(tc->initstr);
        tstring* sappend = tstring_new(tc->concat);
        assert_non_null(sorigem && sappend);
       
        __validate_string(sorigem, sappend, tc);

        tstring_free(sorigem);
        tstring_free(sappend);
}

static void __test_string_concat_v2(void** state)
{
        for (size_t i = 0; i < sizeof(tc)/sizeof(tc[0]); i++)
        {
                printf("Test (%zu): initstr: %s, concat: %s, teststr: %s\n",
                       i + 1, tc[i].initstr, tc[i].concat, tc[i].teststr);

                __run_test_case(&tc[i]);
        }        
}

int main(void)
{
        const struct CMUnitTest tests[] = {
                cmocka_unit_test(__test_string_concat_v2),
        };
        return cmocka_run_group_tests(tests, NULL, NULL);
}
