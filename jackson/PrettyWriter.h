//
// Created by frank on 17-12-27.
//

#ifndef TJSON_PRETTYWRITER_H
#define TJSON_PRETTYWRITER_H

#include <jackson/Writer.h>

namespace json
{


template <typename WriteStream>
class PrettyWriter: noncopyable
{
public:
    explicit PrettyWriter(WriteStream& os, std::string_view indent = "    "):
            indent_(indent),
            indentDepth_(0),
            expectObjectValue_(false),
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
        expectObjectValue_ = true;
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
        indentDepth_++;
        putNewline();
        putIndent();
        if (expectObjectValue_)
            expectObjectValue_ = false;
    }
    void decrIndent()
    {
        assert(indentDepth_ > 0);
        indentDepth_--;
        putNewline();
        putIndent();
        if (expectObjectValue_)
            expectObjectValue_ = false;
    }
    void keepIndent()
    {
        if (indentDepth_ > 0) {
            putNewline();
            putIndent();
        }
        if (expectObjectValue_)
            expectObjectValue_ = false;
    }
    void putIndent()
    {
        for (int i = 0; i < indentDepth_; i++)
            os_.put(indent_);
    }
    void putNewline()
    {
        os_.put('\n');
    }

private:
    std::string_view indent_;
    int indentDepth_ = 0;
    bool expectObjectValue_ = false;

private:
    Writer<WriteStream> writer_;
    WriteStream& os_;
};


}

#endif //TJSON_PRETTYWRITER_H
