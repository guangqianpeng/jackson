//
// Created by frank on 17-12-25.
//

#ifndef TJSON_VALUE_H
#define TJSON_VALUE_H

#include <cassert>
#include <string>
#include <vector>
#include <variant>
#include <memory>

#include "noncopyable.h"

namespace tjson
{


enum ValueType {
    TYPE_NULL,
    TYPE_TRUE,
    TYPE_FALSE,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_OBJECT,
};


class Value: noncopyable
{
    friend class Document;
public:
    using ValuePtr = std::unique_ptr<Value>;
    using String   = std::string;
    using Array    = std::vector<ValuePtr>;
    struct Member {
        ValuePtr key; ValuePtr value;
        Member(Value* key_, Value* value_):
                key(key_), value(value_)
        {}
    };
    using Object   = std::vector<Member>;
    using Variant  = std::variant<int32_t, int64_t, double, String, Array, Object>;

public:
    explicit Value(ValueType type = TYPE_NULL);
    explicit Value(int32_t i32): type_(TYPE_INT32), i32_(i32) {}
    explicit Value(int64_t i64): type_(TYPE_INT64), i64_(i64) {}
    explicit Value(double d): type_(TYPE_DOUBLE), d_(d) {}
    explicit Value(String&& s): type_(TYPE_STRING), var_(new Variant(std::move(s))) {}
    explicit Value(Array&& a): type_(TYPE_ARRAY), var_(new Variant(std::move(a))) {}
    explicit Value(Object&& o): type_(TYPE_OBJECT), var_(new Variant(std::move(o))) {}

    Value(Value&& rhs) noexcept :
            type_(rhs.type_),
            var_(rhs.var_)
    {
        rhs.type_ = TYPE_NULL;
        rhs.var_ = nullptr;
    }

    ~Value();

#define CALL(expr) if (!(expr)) return false;

    template <typename Handler>
    bool writeTo(Handler& handler)
    {
        switch (type_)
        {
            case TYPE_NULL:
                CALL(handler.Null());
                break;
            case TYPE_TRUE:
                CALL(handler.Bool(true));
                break;
            case TYPE_FALSE:
                CALL(handler.Bool(false));
                break;
            case TYPE_INT32:
                CALL(handler.Int32(i32_));
                break;
            case TYPE_INT64:
                CALL(handler.Int64(i64_));
                break;
            case TYPE_DOUBLE:
                CALL(handler.Double(d_));
                break;
            case TYPE_STRING:
                CALL(handler.String(std::string(getString())));
                break;
            case TYPE_ARRAY:
                CALL(handler.StartArray());
                for (auto& val: getArray()) {
                    CALL(val->writeTo(handler));
                }
                CALL(handler.EndArray());
                break;
            case TYPE_OBJECT:
                CALL(handler.StartObject());
                for (auto& val: getObject()) {
                    handler.Key(std::string(val.key->getString()));
                    CALL(val.value->writeTo(handler));
                }
                CALL(handler.EndObject());
                break;
            default:
                assert(false && "bad type");
        }
        return true;
    }

#undef CALL

    ValueType getType() const { return type_; }

    size_t getSize() const
    {
        if (type_ == TYPE_ARRAY)
            return getArray().size();
        if (type_ == TYPE_OBJECT)
            return getObject().size();
        return 1;
    }

    // getter && setter
    bool getBool() const
    {
        assert(type_ == TYPE_TRUE || type_ == TYPE_FALSE);
        return type_ == TYPE_TRUE;
    }
    void setBool(bool b)
    {
        assert(type_ == TYPE_TRUE || type_ == TYPE_FALSE);
        type_ = b ? TYPE_TRUE : TYPE_FALSE;
    }

    int32_t getInt32() const
    {
        assert(type_ == TYPE_INT32);
        return i32_;
    }
    void setInt32(int32_t i32)
    {
        assert(type_ == TYPE_INT32);
        i32_ = i32;
    }

    int64_t getInt64() const
    {
        assert(type_ == TYPE_INT64);
        return i64_;
    }
    void setInt64(int64_t i64)
    {
        assert(type_ == TYPE_INT64);
        i64_ = i64;
    }

    double getDouble() const
    {
        assert(type_ == TYPE_DOUBLE);
        return d_;
    }
    void setDouble(double d)
    {
        assert(type_ == TYPE_DOUBLE);
        d_ = d;
    }

    const String& getString() const
    {
        assert(type_ == TYPE_STRING);
        return std::get<String>(*var_);
    }
    void setString(const std::string& str)
    {
        assert(type_ == TYPE_STRING);
        std::get<String>(*var_) = str;
    }

    const Array& getArray() const
    {
        assert(type_ == TYPE_ARRAY);
        return std::get<Array>(*var_);
    }

    const Object& getObject() const
    {
        assert(type_ == TYPE_OBJECT);
        return std::get<Object>(*var_);
    }

    String& getString()
    {
        assert(type_ == TYPE_STRING);
        return std::get<String>(*var_);
    }

    Array& getArray()
    {
        assert(type_ == TYPE_ARRAY);
        return std::get<Array>(*var_);
    }

    Object& getObject()
    {
        assert(type_ == TYPE_OBJECT);
        return std::get<Object>(*var_);
    }

    Value& operator[] (const char* key)
    {
        auto& obj = getObject();
        for (auto& m: obj) {
            if (m.key->getString() == key) {
                return *m.value;
            }
        }
        auto value = new Value(TYPE_OBJECT);
        obj.emplace_back(new Value(key), value);
        return *value;
    }

    const Value& operator[] (size_t i) const
    { return *getArray()[i]; }

    Value& operator[] (size_t i)
    { return *getArray()[i]; }

private:
    ValueType type_;
    union {
        Variant* var_;
        int32_t  i32_;
        int64_t  i64_;
        double   d_;
    };
};


}

#endif //TJSON_VALUE_H
