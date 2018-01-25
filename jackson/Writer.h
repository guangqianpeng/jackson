//
// Created by frank on 17-12-26.
//

#ifndef TJSON_WRITER_H
#define TJSON_WRITER_H

#include <string>
#include <cstdint>
#include <vector>
#include <cassert>
#include <cmath>

#include <jackson/Value.h>

namespace json
{

namespace detail
{

//
// fast int to string conversion
// buffer is NOT null terminated!!!
//
unsigned itoa(int32_t val, char* buf);
unsigned itoa(int64_t val, char* buf);

}

template <typename WriteStream>
class Writer: noncopyable
{
public:
    explicit Writer(WriteStream& os):
            os_(os), seeValue_(false)
    {}

    bool Null()
    {
        prefix(TYPE_NULL);
        os_.put("null");
        return true;
    }
    bool Bool(bool b)
    {
        prefix(TYPE_BOOL);
        os_.put(b ? "true" : "false");
        return true;
    }
    bool Int32(int32_t i32)
    {
        prefix(TYPE_INT32);

        char buf[11];
        unsigned cnt = detail::itoa(i32, buf);
        os_.put(std::string_view(buf, cnt));
        return true;
    }
    bool Int64(int64_t i64)
    {
        prefix(TYPE_INT64);

        char buf[20];
        unsigned cnt = detail::itoa(i64, buf);
        os_.put(std::string_view(buf, cnt));
        return true;
    }
    bool Double(double d)
    {
        prefix(TYPE_DOUBLE);

        // fixme: faster conversion please
        char buf[32];


        if (std::isinf(d)) {
            strcpy(buf, "Infinity");
        }
        else if (std::isnan(d)) {
            strcpy(buf, "NaN");
        }
        else {

            int n = sprintf(buf, "%.17g", d);

            // type information loss if ".0" not added
            // "1.0" -> double 1 -> "1"
            assert(n > 0 && n < 32);
            auto it = std::find_if_not(buf, buf + n, isdigit);
            if (it == buf + n) {
                strcat(buf, ".0");
            }
        }

        os_.put(buf);
        return true;
    }
    bool String(std::string_view s)
    {
        prefix(TYPE_STRING);
        os_.put('"');
        for (auto c: s) {
            auto u = static_cast<unsigned>(c);
            switch (u) {
                case '\"': os_.put("\\\""); break;
                case '\b': os_.put("\\b");  break;
                case '\f': os_.put("\\f");  break;
                case '\n': os_.put("\\n");  break;
                case '\r': os_.put("\\r");  break;
                case '\t': os_.put("\\t");  break;
                case '\\': os_.put("\\\\"); break;
                default:
                    if (u < 0x20) {
                        char buf[7];
                        snprintf(buf, 7, "\\u%04X", u);
                        os_.put(buf);
                    }
                    else os_.put(c);
                    break;
            }
        }

        os_.put('"');
        return true;
    }
    bool StartObject()
    {
        prefix(TYPE_OBJECT);
        stack_.emplace_back(false);
        os_.put('{');
        return true;
    }
    bool Key(std::string_view s)
    {
        prefix(TYPE_STRING);
        os_.put('"');
        os_.put(s);
        os_.put('"');
        return true;
    }
    bool EndObject()
    {
        assert(!stack_.empty());
        assert(!stack_.back().inArray);
        stack_.pop_back();
        os_.put('}');
        return true;
    }
    bool StartArray()
    {
        prefix(TYPE_ARRAY);
        stack_.emplace_back(true);
        os_.put('[');
        return true;
    }
    bool EndArray()
    {
        assert(!stack_.empty());
        assert(stack_.back().inArray);
        stack_.pop_back();
        os_.put(']');
        return true;
    }

private:
    void prefix(ValueType type)
    {
        if (seeValue_)
            assert(!stack_.empty() && "root not singular");
        else
            seeValue_ = true;

        if (stack_.empty())
            return;

        auto& top = stack_.back();
        if (top.inArray) {
            if (top.valueCount > 0)
                os_.put(',');
        }
        else {
            if (top.valueCount % 2 == 1)
                os_.put(':');
            else {
                assert(type == TYPE_STRING && "miss quotation mark");
                if (top.valueCount > 0)
                    os_.put(',');
            }
        }
        top.valueCount++;
    }

private:
    struct Level {
        explicit Level(bool inArray_):
                inArray(inArray_), valueCount(0)
        {}
        bool inArray; // in array or object
        int valueCount;
    };

private:
    std::vector<Level> stack_;
    WriteStream& os_;
    bool seeValue_;
};


}

#endif //TJSON_HANDLER_H
