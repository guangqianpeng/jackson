//
// Created by frank on 17-12-27.
//

#include <gtest/gtest.h>

#include <jackson/Document.h>

using namespace json;

#define TEST_ERROR(err, json) do { \
    Document doc; \
    EXPECT_EQ(err, doc.parse(json)); \
} while(false)

TEST(json_error, expect_value) {
    ParseError err = PARSE_EXPECT_VALUE;
    TEST_ERROR(err, "");
    TEST_ERROR(err, " ");
    TEST_ERROR(err, " \r\n");
}

TEST(json_error, bad_value) {
    ParseError err = PARSE_BAD_VALUE;
    TEST_ERROR(err, "tRUE");
    TEST_ERROR(err, "falss");
    TEST_ERROR(err, "nulr");

    TEST_ERROR(err, "1.");
    TEST_ERROR(err, "001");
    TEST_ERROR(err, "10e");
    TEST_ERROR(err, "10.e");
    TEST_ERROR(err, "10e.");
    TEST_ERROR(err, "10.0e+");
    TEST_ERROR(err, "-00");
    TEST_ERROR(err, "[null,]");
    TEST_ERROR(err, "1.0i32");
    TEST_ERROR(err, "0.0i64");
}

TEST(json_error, root_not_singular) {
    ParseError err = PARSE_ROOT_NOT_SINGULAR;
    TEST_ERROR(err, "true false");
    TEST_ERROR(err, "null f");
    TEST_ERROR(err, "1.0.1");
    TEST_ERROR(err, "1.0e1e");
    TEST_ERROR(err, "9527 9528");
    TEST_ERROR(err, "\"9527\" \"9528\"");
    TEST_ERROR(err, "[] []");
    TEST_ERROR(err, "{} []");
}

TEST(json_error, num_too_big) {
    ParseError err = PARSE_NUMBER_TOO_BIG;
    TEST_ERROR(err, "1e309");
    TEST_ERROR(err, "-1e309");
    TEST_ERROR(err, "9223372036854775808");
    TEST_ERROR(err, "-9223372036854775809");
    TEST_ERROR(err, "9223372036854775808i64");
    TEST_ERROR(err, "-9223372036854775809i64");
    TEST_ERROR(err, "12345678901i32");
    TEST_ERROR(err, "-12345678901i32i32");
}

TEST(json_error, bad_string_char)
{
    ParseError err = PARSE_BAD_STRING_CHAR;
    TEST_ERROR(err, "\"abcd\1efg\"");
    TEST_ERROR(err, "\"\b\"");
}

TEST(json_error, bad_string_escape)
{
    ParseError err = PARSE_BAD_STRING_ESCAPE;
    TEST_ERROR(err, "\"xxx\\x\"");
    TEST_ERROR(err, "\"xx\\a\"");
}

TEST(json_error, bad_unicode_hex)
{
    ParseError err = PARSE_BAD_UNICODE_HEX;
    TEST_ERROR(err, "\"\\u123\"");
    TEST_ERROR(err, "\"\\ux123\"");
    TEST_ERROR(err, "\"\\uDBFF\\uxxxx\"");
}

TEST(json_error, bad_unicode_surrogate)
{
    ParseError err = PARSE_BAD_UNICODE_SURROGATE;
    TEST_ERROR(err, "\"\\ud800\"");
    TEST_ERROR(err, "\"\\uDBFF\\\"");
    TEST_ERROR(err, "\"\\uDBFF\\x\"");
    TEST_ERROR(err, "\"\\uDBFF\\uDBFF\"");
    TEST_ERROR(err, "\"\\uDBFF\\uFFFF\"");
}

TEST(json_error, miss_quotation_mark)
{
    ParseError err = PARSE_MISS_QUOTATION_MARK;
    TEST_ERROR(err, "\"wtf");
    TEST_ERROR(err, "\"wtf""");
    TEST_ERROR(err, "\"xx\\\"");
}

TEST(json_error, miss_comma_or_square_bracket)
{
    ParseError err = PARSE_MISS_COMMA_OR_SQUARE_BRACKET;
    TEST_ERROR(err, "[1, 2");
    TEST_ERROR(err, "[truefalse]");
}

TEST(json_error, miss_key)
{
    ParseError err = PARSE_MISS_KEY;
    TEST_ERROR(err, "{");
    TEST_ERROR(err, "{:null}");
    TEST_ERROR(err, "{\"hehe\":null, }");
}

TEST(json_error, miss_colon)
{
    ParseError err = PARSE_MISS_COLON;
    TEST_ERROR(err, "{\"hehe\"}");
    TEST_ERROR(err, "{\"hehe\":null, \"data\"}");
}

TEST(json_error, miss_comma_or_curly_bracket)
{
    ParseError err = PARSE_MISS_COMMA_OR_CURLY_BRACKET;
    TEST_ERROR(err, "{\"hehe\":false");
    TEST_ERROR(err, "{\"hehe\":false, \"\":\"蛤\"");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}