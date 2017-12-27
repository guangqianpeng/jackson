//
// Created by frank on 17-12-26.
//

#include <jackson/StringReadStream.h>
#include <jackson/FileWriteStream.h>
#include <jackson/Writer.h>
#include <jackson/Reader.h>

using namespace json;

template <typename Handler>
class AddOne: noncopyable
{
public:
    explicit AddOne(Handler& handler): handler_(handler) {}

    bool Null()
    { return handler_.Null(); }
    bool Bool(bool b)
    { return handler_.Bool(b); }
    bool Int32(int32_t i32)
    { return handler_.Int32(i32 + 1); }
    bool Int64(int64_t i64)
    { return handler_.Int64(i64 + 1); }
    bool Double(double d)
    { return handler_.Double(d + 1); }
    bool String(std::string&& s)
    { return handler_.String(std::move(s)); }
    bool StartObject()
    { return handler_.StartObject(); }
    bool Key(std::string&& s)
    { return handler_.Key(std::move(s)); }
    bool EndObject()
    { return handler_.EndObject(); }
    bool StartArray()
    { return handler_.StartArray(); }
    bool EndArray()
    { return handler_.EndArray(); }

private:
    Handler& handler_;
};

int main()
{
    StringReadStream is("[\n"
                                "  {\n"
                                "    \"precision\": \"zip\",\n"
                                "    \"Latitude\": 37.766800000000003,\n"
                                "    \"Longitude\": -122.3959,\n"
                                "    \"Address\": \"\",\n"
                                "    \"City\": \"SAN FRANCISCO\",\n"
                                "    \"State\": \"CA\",\n"
                                "    \"Zip\": \"94107\",\n"
                                "    \"Country\": \"US\"\n"
                                "  },\n"
                                "  {\n"
                                "    \"precision\": \"zip\",\n"
                                "    \"Latitude\": 37.371991000000001,\n"
                                "    \"Longitude\": -122.02602,\n"
                                "    \"Address\": \"\",\n"
                                "    \"City\": \"SUNNYVALE\",\n"
                                "    \"State\": \"CA\",\n"
                                "    \"Zip\": \"94085\",\n"
                                "    \"Country\": \"US\"\n"
                                "  }\n"
                                "]");
    FileWriteStream os(stdout);
    Writer writer(os);
    AddOne addOne(writer);

    ParseError err = Reader::parse(is, addOne);
    if (err != PARSE_OK) {
        printf(parseErrorStr(err));
    }
}