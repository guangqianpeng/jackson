//
// Created by frank on 17-12-26.
//

#ifndef TJSON_WRITER_H
#define TJSON_WRITER_H

#include <string>
#include <cstdint>
#include <vector>
#include <cassert>

#include "Value.h"

namespace tjson
{


template <typename WriteStream>
class Writer: noncopyable
{
public:
    explicit Writer(WriteStream& os): os_(os), seeValue_(false) {}

    bool Null()
    {
        prefix(TYPE_NULL);
        os_.put("null");
        return true;
    }
    bool Bool(bool b)
    {
        prefix(b ? TYPE_TRUE : TYPE_FALSE);
        os_.put(b ? "true" : "false");
        return true;
    }
    bool Int32(int32_t i32)
    {
        prefix(TYPE_INT32);
        os_.put(std::to_string(i32));
        return true;
    }
    bool Int64(int64_t i64)
    {
        prefix(TYPE_INT64);
        os_.put(std::to_string(i64));
        return true;
    }
    bool Double(double d)
    {
        prefix(TYPE_DOUBLE);
        os_.put(std::to_string(d));
        return true;
    }
    bool String(std::string&& s)
    {
        prefix(TYPE_STRING);
        os_.put('"');
        // todo: unicode and escape
        os_.put(std::move(s));
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
    bool Key(std::string&& s)
    {
        prefix(TYPE_STRING);
        os_.put('"');
        os_.put(std::move(s));
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
        } else {
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
                inArray(inArray_), valueCount(0) {}
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
