//
// Created by frank on 17-12-26.
//

#ifndef TJSON_DOCUMENT_H
#define TJSON_DOCUMENT_H

#include <string>

#include <jackson/Value.h>
#include <jackson/Reader.h>
#include <jackson/StringReadStream.h>


namespace json
{


class Document: public Value
{
public:
    ParseError parse(const char* json, size_t len)
    {
        return parse(std::string_view(json, len));
    }

    ParseError parse(std::string_view json)
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
        addValue(Value(TYPE_NULL));
        return true;
    }
    bool Bool(bool b)
    {
        addValue(Value(b));
        return true;
    }
    bool Int32(int32_t i32)
    {
        addValue(Value(i32));
        return true;
    }
    bool Int64(int64_t i64)
    {
        addValue(Value(i64));
        return true;
    }
    bool Double(double d)
    {
        addValue(Value(d));
        return true;
    }
    bool String(std::string_view s)
    {
        addValue(Value(s));
        return true;
    }
    bool StartObject()
    {
        auto value = addValue(Value(TYPE_OBJECT));
        stack_.emplace_back(value);
        return true;
    }
    bool Key(std::string_view s)
    {
        addValue(Value(s));
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
        auto value = addValue(Value(TYPE_ARRAY));
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

private:
    Value* addValue(Value&& value)
    {
        ValueType type = value.getType();
        (void)type;
        if (seeValue_)
            assert(!stack_.empty() && "root not singular");
        else {
            assert(type_ == TYPE_NULL);
            seeValue_ = true;
            type_ = value.type_;
            a_ = value.a_;
            value.type_ = TYPE_NULL;
            value.a_ = nullptr;
            return this;
        }


        auto& top = stack_.back();
        if (top.type() == TYPE_ARRAY) {
            top.value->addValue(std::move(value));
            top.valueCount++;
            return top.lastValue();
        }
        else {
            assert(top.type() == TYPE_OBJECT);

            if (top.valueCount % 2 == 0) {
                assert(type == TYPE_STRING && "miss quotation mark");
                key_ = std::move(value);
                top.valueCount++;
                return &key_;
            }
            else {
                top.value->addMember(std::move(key_), std::move(value));
                top.valueCount++;
                return top.lastValue();
            }
        }
    }

private:
    struct Level
    {
        explicit Level(Value* value_):
                value(value_), valueCount(0)
        {}

        ValueType type() const
        {
            return value->getType();
        }

        Value* lastValue()
        {
            if (type() == TYPE_ARRAY) {
                return &value->a_->data.back();
            } else {
                return &value->o_->data.back().value;
            }
        }

        Value* value;
        int valueCount;
    };

private:
    std::vector<Level> stack_;
    Value key_;
    bool seeValue_ = false;
};


}



#endif //TJSON_DOCUMENT_H
