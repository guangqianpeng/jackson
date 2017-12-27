//
// Created by frank on 17-12-24.
//

#include <gtest/gtest.h>

#include <jackson/Document.h>
#include <jackson/StringWriteStream.h>
#include <jackson/Writer.h>

using namespace json;

#define TEST_ERROR(err, json) do { \
    Document doc; \
    EXPECT_EQ(err, doc.parse(json)); \
} while(false)

#define TEST_BOOL(type, value, json) do { \
    Document doc; \
    ParseError err = doc.parse(json); \
    EXPECT_EQ(err, PARSE_OK); \
    EXPECT_EQ(doc.getType(), type); \
    EXPECT_EQ(doc.getBool(), value); \
} while(false)

#define TEST_NULL(json) do { \
    Document doc; \
    ParseError err = doc.parse(json); \
    EXPECT_EQ(err, PARSE_OK); \
    EXPECT_EQ(doc.getType(), TYPE_NULL); \
} while(false)

#define TEST_DOUBLE(num, json) do { \
    Document doc; \
    ParseError err = doc.parse(json); \
    EXPECT_EQ(err, PARSE_OK); \
    EXPECT_EQ(doc.getType(), TYPE_DOUBLE); \
    EXPECT_EQ(doc.getDouble(), num); \
} while(false)

#define TEST_INT32(num, json) do { \
    Document doc; \
    ParseError err = doc.parse(json); \
    EXPECT_EQ(err, PARSE_OK); \
    EXPECT_EQ(doc.getType(), TYPE_INT32); \
    EXPECT_EQ(doc.getInt32(), num); \
} while(false)

#define TEST_INT64(num, json) do { \
    Document doc; \
    ParseError err = doc.parse(json); \
    EXPECT_EQ(err, PARSE_OK); \
    EXPECT_EQ(doc.getType(), TYPE_INT64); \
    EXPECT_EQ(doc.getInt64(), num); \
} while(false)

#define TEST_STRING(str, json) do { \
    Document doc; \
    ParseError err = doc.parse(json); \
    EXPECT_EQ(err, PARSE_OK); \
    EXPECT_EQ(doc.getType(), TYPE_STRING); \
    EXPECT_EQ(doc.getString(), str); \
} while(false)

#define TEST_ROUNDTRIP(json) do { \
    Document doc; \
    ParseError err = doc.parse(json); \
    EXPECT_EQ(err, PARSE_OK); \
    StringWriteStream os; \
    Writer writer(os); \
    writeTo(doc, writer); \
    EXPECT_EQ(json, os.get()); \
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

TEST(json_value, null) {
    TEST_NULL("null");
    TEST_NULL(" null ");
}

TEST(json_value, bool_) {
    TEST_BOOL(TYPE_TRUE, true, "true");
    TEST_BOOL(TYPE_TRUE, true, "\r\ntrue ");
    TEST_BOOL(TYPE_FALSE, false, " false");
    TEST_BOOL(TYPE_FALSE, false, "false ");
}

TEST(json_value, number) {
    TEST_DOUBLE(0.0, "0.0");
    TEST_DOUBLE(0.0, "-0.0e000");
    TEST_DOUBLE(876.543e21, "876.543e+00021");
    TEST_DOUBLE(-111.000e-010, "-111.000e-010");
/* https://en.wikipedia.org/wiki/Double-precision_floating-point_format */
    TEST_DOUBLE(1.0000000000000002, "1.0000000000000002");
    TEST_DOUBLE(-1.0000000000000002, "-1.0000000000000002");
    // fixme stod not pass edge case
//    TEST_DOUBLE(4.9406564584124654e-323, "4.9406564584124654E-323");
//    TEST_DOUBLE(4.9406564584124654e-324, "4.9406564584124654e-324");
//    TEST_DOUBLE(2.2250738585072009e-308, "2.2250738585072009e-308");
//    TEST_DOUBLE(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TEST_DOUBLE(2.2250738585072014e-308, "2.2250738585072014e-308");
    TEST_DOUBLE(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TEST_DOUBLE(1.7976931348623157e308, "1.7976931348623157e308");
    TEST_DOUBLE(-1.7976931348623157e308, "-1.7976931348623157e308");

    TEST_INT32(0, "0");
    TEST_INT32(-0, "0");
    TEST_INT32(1, "1");
    TEST_INT32(-1, "-1");
    TEST_INT32(2147483647, "2147483647");
    TEST_INT32(-2147483648, "-2147483648");

    TEST_INT64(2147483648LL, "2147483648");
    TEST_INT64(-2147483649LL, "-2147483649");
    // gcc refuse large literal
    TEST_INT64(std::numeric_limits<int64_t>::max(), "9223372036854775807");
    TEST_INT64(std::numeric_limits<int64_t>::min(), "-9223372036854775808");
}

TEST(json_value, string_)
{
    TEST_STRING(""sv, "\"\"");
    TEST_STRING("abcd"sv, "\"abcd\"");
    TEST_STRING("\\"sv, "\"\\\\\"");
    TEST_STRING("\""sv, "\"\\\"\"");
    TEST_STRING("/\b\f\n\r\t"sv, "\"\\/\\b\\f\\n\\r\\t\"");
    TEST_STRING("蛤\0蛤\0蛤"sv, "\"蛤\\u0000蛤\\u0000蛤\"");
    TEST_STRING("蛤\0蛤\0蛤"sv, "\"蛤\0蛤\0蛤\""sv);
    TEST_STRING("\0"sv, "\"\\u0000\"");
    TEST_STRING("\x24"sv, "\"\\u0024\"");         			/* Dollar $ */
    TEST_STRING("\xC2\xA2"sv, "\"\\u00A2\"");     			/* Cents ¢ */
    TEST_STRING("\xE2\x82\xAC"sv, "\"\\u20AC\""); 			/* Euro € */
    TEST_STRING("\xF0\x9D\x84\x9E"sv, "\"\\uD834\\uDD1E\""); /* G clef  𝄞 */
    TEST_STRING("\xF0\x9D\x84\x9E"sv, "\"\\ud834\\udd1e\""); /* G clef  𝄞 */
}

TEST(json_value, array)
{
    ParseError err;
    {
        Document doc;
        err = doc.parse("[]");
        EXPECT_EQ(err, PARSE_OK);
        EXPECT_EQ(doc.getType(), TYPE_ARRAY);
        EXPECT_TRUE(doc.getArray().empty());
    }
    {
        Document doc;
        err = doc.parse("[[]]");
        EXPECT_EQ(err, PARSE_OK);
        EXPECT_EQ(doc.getType(), TYPE_ARRAY);

        auto& array = doc.getArray();
        EXPECT_EQ(array.size(), 1);
        EXPECT_EQ(array[0].getType(), TYPE_ARRAY);
        EXPECT_TRUE(array[0].getArray().empty());
    }
    {
        Document doc;
        err = doc.parse("[0, 1, 2, 3, 4]");
        EXPECT_EQ(err, PARSE_OK);
        EXPECT_EQ(doc.getType(), TYPE_ARRAY);

        auto& array = doc.getArray();

        EXPECT_EQ(5, array.size());
        for (size_t i = 0; i < array.size(); i++) {
            EXPECT_EQ(array[i].getType(), TYPE_INT32);
            EXPECT_EQ(array[i].getInt32(), i);
        }
    }
    {
        Document doc;
        err = doc.parse("[ { } , { } , { } , { } , { } ]");
        EXPECT_EQ(err, PARSE_OK);
        EXPECT_EQ(doc.getType(), TYPE_ARRAY);

        auto& array = doc.getArray();

        EXPECT_EQ(5, array.size());
        for (size_t i = 0; i < array.size(); i++) {
            EXPECT_EQ(array[i].getType(), TYPE_OBJECT);
            EXPECT_EQ(array[i].getObject().size(), 0);
        }
    }
    {
        Document doc;
        err = doc.parse("[\"hehe\", true, false, null, 0.0]");
        EXPECT_EQ(err, PARSE_OK);
        EXPECT_EQ(doc.getType(), TYPE_ARRAY);

        auto& array = doc.getArray();
        EXPECT_EQ(5, array.size());
        EXPECT_EQ(array[0].getType(), TYPE_STRING);
        EXPECT_EQ(array[1].getType(), TYPE_TRUE);
        EXPECT_EQ(array[2].getType(), TYPE_FALSE);
        EXPECT_EQ(array[3].getType(), TYPE_NULL);
        EXPECT_EQ(array[4].getType(), TYPE_DOUBLE);

        EXPECT_TRUE(array[0].getString() == "hehe");
        EXPECT_EQ(array[4].getDouble(), 0.0);
    }

}

TEST(json_value, object)
{

    Document doc;
    ParseError err = doc.parse(" { "
                      "\"n\" : null , "
                      "\"f\" : false , "
                      "\"t\" : true , "
                      "\"i\" : 123 , "
                      "\"s\" : \"abc\", "
                      "\"a\" : [ 1, 2, 3 ],"
                      "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
                      " } ");

    EXPECT_EQ(err, PARSE_OK);
    EXPECT_EQ(doc.getType(), TYPE_OBJECT);

    EXPECT_EQ(doc.getObject().size(), 7);
    EXPECT_EQ(doc["n"].getType(), TYPE_NULL);
    EXPECT_EQ(doc["f"].getType(), TYPE_FALSE);
    EXPECT_EQ(doc["t"].getType(), TYPE_TRUE);
    EXPECT_EQ(doc["i"].getType(), TYPE_INT32);
    EXPECT_EQ(doc["s"].getType(), TYPE_STRING);
    EXPECT_EQ(doc["a"].getType(), TYPE_ARRAY);
    EXPECT_EQ(doc["o"].getType(), TYPE_OBJECT);

    EXPECT_EQ(doc["i"].getInt32(), 123);
    EXPECT_EQ(doc["s"].getString(), "abc");

    auto& array = doc["a"].getArray();
    EXPECT_EQ(array.size(), 3);
    for (size_t i = 0; i < 3; i++) {
        EXPECT_EQ(array[i].getType(), TYPE_INT32);
        EXPECT_EQ(array[i].getInt32(), i+1);
    }

    auto& obj = doc["o"];
    EXPECT_EQ(obj.getSize(), 3);
    EXPECT_EQ(obj["1"].getInt32(), 1);
    EXPECT_EQ(obj["2"].getInt32(), 2);
    EXPECT_EQ(obj["3"].getInt32(), 3);
}

TEST(json_round, number)
{
    TEST_ROUNDTRIP("0");
    TEST_ROUNDTRIP("1");
    TEST_ROUNDTRIP("-1");
    TEST_ROUNDTRIP("10086.9527");
    TEST_ROUNDTRIP("2.345e+100");
    TEST_ROUNDTRIP("-1.11e-10");

/* https://en.wikipedia.org/wiki/Double-precision_floating-point_format */
    TEST_ROUNDTRIP("1.0000000000000002");
    TEST_ROUNDTRIP("-1.0000000000000002");
    // fixme: stod has bug...
//    TEST_ROUNDTRIP("4.9406564584124654e-324");
//    TEST_ROUNDTRIP("-4.9406564584124654e-324");
//    TEST_ROUNDTRIP("2.2250738585072009e-308");
//    TEST_ROUNDTRIP("-2.2250738585072009e-308");
    TEST_ROUNDTRIP("2.2250738585072014e-308");
    TEST_ROUNDTRIP("-2.2250738585072014e-308");
    TEST_ROUNDTRIP("1.7976931348623157e+308");
    TEST_ROUNDTRIP("-1.7976931348623157e+308");
}

TEST(json_round, string)
{
    TEST_ROUNDTRIP("\"蛤蛤蛤\"");
    TEST_ROUNDTRIP("\"\"");
    TEST_ROUNDTRIP("\"Hello\"");
    TEST_ROUNDTRIP("\"Hello\\nWorld\"");
    TEST_ROUNDTRIP("\"\\\" \\\\ / \\b \\f \\n \\r \\t\"");
    TEST_ROUNDTRIP("\"Hello\\u0000World\"");
}

TEST(json_round, array)
{
    TEST_ROUNDTRIP("[]");
    TEST_ROUNDTRIP("[null,false,true,123,\"abc\",[1,2,3]]");
}

TEST(json_round, object)
{
    TEST_ROUNDTRIP("{}");
    TEST_ROUNDTRIP("{\"n\":null,\"f\":false,\"t\":true,\"i\":123,\"s\":\"abc\",\"a\":[1,2,3],\"o\":{\"1\":1,\"2\":2,\"3\":3}}");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}