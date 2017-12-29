//
// Created by frank on 17-12-26.
//

#include <jackson/StringReadStream.h>
#include <jackson/FileWriteStream.h>
#include <jackson/Writer.h>
#include <jackson/Reader.h>

#include "sample.h"

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
    bool String(std::string_view  s)
    { return handler_.String(s); }
    bool StartObject()
    { return handler_.StartObject(); }
    bool Key(std::string_view s)
    { return handler_.Key(s); }
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
    StringReadStream is(sample[0]);
    FileWriteStream os(stdout);
    Writer writer(os);
    AddOne addOne(writer);

    ParseError err = Reader::parse(is, addOne);
    if (err != PARSE_OK) {
        printf("%s\n", parseErrorStr(err));
    }
}