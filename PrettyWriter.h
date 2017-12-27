//
// Created by frank on 17-12-27.
//

#ifndef TJSON_PRETTYWRITER_H
#define TJSON_PRETTYWRITER_H

#include "Writer.h"

namespace tjson
{


template <typename WriteStream>
class PrettyWriter: noncopyable
{
public:
    explicit PrettyWriter(WriteStream& os):
            writer_(os),
            os_(os)
    {}

    bool Null()
    {
        writer_.Null();
        keepIndent();
        return true;
    }

    bool Bool(bool b)
    {
        writer_.Bool(b);
        keepIndent();
        return true;
    }

    bool Int32(int32_t i32)
    {
        writer_.Int32(i32);
        keepIndent();
        return true;
    }

    bool Int64(int64_t i64)
    {
        writer_.Int64(i64);
        keepIndent();
        return true;
    }

    bool Double(double d)
    {
        writer_.Double(d);
        keepIndent();
        return true;
    }

    bool String(std::string_view s)
    {
        writer_.String(s);
        keepIndent();
        return true;
    }

    bool StartObject()
    {
        writer_.StartObject();
        incrIndent();
        return true;
    }

    bool Key(std::string_view s)
    {
        expectObjectValue = true;
        writer_.Key(s);
        return true;
    }

    bool EndObject()
    {
        decrIndent();
        writer_.EndObject();
        return true;
    }

    bool StartArray()
    {
        writer_.StartArray();
        incrIndent();
        return true;
    }

    bool EndArray()
    {
        decrIndent();
        writer_.EndArray();
        return true;
    }

private:
    void incrIndent()
    {
        indent_++;
        putNewline();
        putIndent();
        if (expectObjectValue)
            expectObjectValue = false;
    }
    void decrIndent()
    {
        assert(indent_ > 0);
        indent_--;
        putNewline();
        putIndent();
        if (expectObjectValue)
            expectObjectValue = false;
    }
    void keepIndent()
    {
        if (indent_ > 0) {
            putNewline();
            putIndent();
        }
        if (expectObjectValue)
            expectObjectValue = false;
    }
    void putIndent()
    {
        for (int i = 0; i < indent_; i++)
            os_.put(indentChar);
    }
    void putNewline()
    {
        os_.put('\n');
    }
    void putSpace()
    {
        os_.put(' ');
    }

private:
    static constexpr const char* indentChar = "    ";
    int indent_ = 0;
    bool expectObjectValue = false;
    bool expectArrayValue = false;
private:
    Writer<WriteStream> writer_;
    WriteStream& os_;
};


}

#endif //TJSON_PRETTYWRITER_H
