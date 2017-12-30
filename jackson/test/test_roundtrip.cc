//
// Created by frank on 17-12-27.
//

#include <gtest/gtest.h>

#include <jackson/Document.h>
#include <jackson/StringWriteStream.h>
#include <jackson/Writer.h>

using namespace json;

#define TEST_ROUNDTRIP(json) do { \
    Document doc; \
    ParseError err = doc.parse(json); \
    EXPECT_EQ(err, PARSE_OK); \
    StringWriteStream os; \
    Writer writer(os); \
    doc.writeTo(writer); \
    EXPECT_EQ(json, os.get()); \
} while(false)



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