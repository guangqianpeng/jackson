//
// Created by frank on 17-12-26.
//

#ifndef TJSON_DOCUMENT_H
#define TJSON_DOCUMENT_H

#include <string>

#include "Value.h"
#include "Reader.h"
#include "StringReadStream.h"


namespace tjson
{


class Document: public Value
{
public:
    ParseError parse(const std::string& json)
    {
        return parse(json.c_str());
    }

    ParseError parse(const char* json)
    {
        StringReadStream is(json);
        return parseStream(is);
    }

    template <typename ReadStream>
    ParseError parseStream(ReadStream& is)
    {
        return Reader::parse(is, *this);
    }

public: // handler
    bool Null()
    {
        addValue(new Value(TYPE_NULL));
        return true;
    }
    bool Bool(bool b)
    {
        addValue(new Value(b ? TYPE_TRUE : TYPE_FALSE));
        return true;
    }
    bool Int32(int32_t i32)
    {
        addValue(new Value(i32));
        return true;
    }
    bool Int64(int64_t i64)
    {
        addValue(new Value(i64));
        return true;
    }
    bool Double(double d)
    {
        addValue(new Value(d));
        return true;
    }
    bool String(std::string&& s)
    {
        addValue(new Value(std::move(s)));
        return true;
    }
    bool StartObject()
    {
        auto value = addValue(new Value(TYPE_OBJECT));
        stack_.emplace_back(value);
        return true;
    }
    bool Key(std::string&& s)
    {
        addValue(new Value(std::move(s)));
        return true;
    }
    bool EndObject()
    {
        assert(!stack_.empty());
        assert(stack_.back().type() == TYPE_OBJECT);
        stack_.pop_back();
        return true;
    }
    bool StartArray()
    {
        auto value = addValue(new Value(TYPE_ARRAY));
        stack_.emplace_back(value);
        return true;
    }
    bool EndArray()
    {
        assert(!stack_.empty());
        assert(stack_.back().type() == TYPE_ARRAY);
        stack_.pop_back();
        return true;
    }
    ~Document()
    {
        delete key_;
    }

private:
    Value* addValue(Value *value)
    {
        ValueType type = value->getType();
        if (seeValue_)
            assert(!stack_.empty() && "root not singular");
        else {
            assert(type_ == TYPE_NULL);
            seeValue_ = true;
            type_ = value->type_;
            var_ = value->var_;
            value->type_ = TYPE_NULL;
            delete value;
            return this;
        }


        auto& top = stack_.back();
        if (top.type() == TYPE_ARRAY) {
            top.value->getArray().emplace_back(value);
        }
        else {
            assert(top.type() == TYPE_OBJECT);
            if (top.valueCount % 2 == 0) {
                assert(type == TYPE_STRING && "miss quotation mark");
                key_ = value;
            }
            else {
                top.value->getObject().emplace_back(key_, value);
                key_ = nullptr;
            }
        }
        top.valueCount++;
        return value;
    }

private:
    struct Level {
        explicit Level(Value* value_):
                value(value_), valueCount(0){}
        ValueType type() const
        { return value->getType(); }
        Value* value;
        int valueCount;
    };

private:
    std::vector<Level> stack_;
    Value* key_ = nullptr;
    bool seeValue_ = false;
};


}



#endif //TJSON_DOCUMENT_H
