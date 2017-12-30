//
// Created by frank on 17-12-25.
//

#ifndef TJSON_VALUE_H
#define TJSON_VALUE_H

#include <cassert>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <algorithm>

#include <jackson/noncopyable.h>

namespace json
{

using namespace std::string_view_literals;

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

struct Member;
class Document;

class Value: noncopyable
{
    friend class Document;
public:
    using String   = std::vector<char>;
    using Array    = std::vector<Value>;
    using Object   = std::vector<Member>;
    using MemberIterator = Object::iterator;
    using ConstMemberIterator = Object::const_iterator;

public:
    explicit Value(ValueType type = TYPE_NULL);

    explicit Value(int32_t i32):
            type_(TYPE_INT32),
            i32_(i32)
    {}

    explicit Value(int64_t i64):
            type_(TYPE_INT64),
            i64_(i64)
    {}

    explicit Value(double d):
            type_(TYPE_DOUBLE),
            d_(d)
    {}

    explicit Value(std::string_view s):
            type_(TYPE_STRING),
            s_(new String(s.begin(), s.end()))
    {}

    Value(Value&& rhs) noexcept :
            type_(rhs.type_),
            a_(rhs.a_)
    {
        rhs.type_ = TYPE_NULL;
        rhs.a_ = nullptr;
    }

    ~Value();

    // never copy or construct from Document
    Value(Document&& rhs) = delete;
    void operator=(Document&& rhs) = delete;

    Value& operator=(Value&& rhs) noexcept
    {
        assert(this != &rhs);
        this->~Value();
        type_ = rhs.type_;
        a_ = rhs.a_;
        rhs.type_ = TYPE_NULL;
        rhs.a_ = nullptr;
        return *this;
    }

    ValueType getType() const { return type_; }

    size_t getSize() const
    {
        if (type_ == TYPE_ARRAY)
            return a_->size();
        else if (type_ == TYPE_OBJECT)
            return o_->size();
        return 1;
    }

    // getter && setter
    bool getBool() const
    {
        assert(type_ == TYPE_TRUE || type_ == TYPE_FALSE);
        return type_ == TYPE_TRUE;
    }

    int32_t getInt32() const
    {
        assert(type_ == TYPE_INT32);
        return i32_;
    }

    int64_t getInt64() const
    {
        assert(type_ == TYPE_INT64);
        return i64_;
    }

    double getDouble() const
    {
        assert(type_ == TYPE_DOUBLE);
        return d_;
    }

    std::string_view getString() const
    {
        assert(type_ == TYPE_STRING);
        return std::string_view(&*s_->begin(), s_->size());
    }

    const Array& getArray() const
    {
        assert(type_ == TYPE_ARRAY);
        return *a_;
    }

    const Object& getObject() const
    {
        assert(type_ == TYPE_OBJECT);
        return *o_;
    }

    Value& setBool(bool b)
    {
        this->~Value();
        return *new (this) Value(b);
    }

    Value& setInt32(int32_t i32)
    {
        this->~Value();
        return *new (this) Value(i32);
    }

    Value& setInt64(int64_t i64)
    {
        this->~Value();
        return *new (this) Value(i64);
    }

    Value& setDouble(double d)
    {
        this->~Value();
        return *new (this) Value(d);
    }

    Value& setString(std::string_view s)
    {
        this->~Value();
        return *new (this) Value(s);
    }

    Value& setArray()
    {
        this->~Value();
        return *new (this) Value(TYPE_ARRAY);
    }

    Value& setObject()
    {
        this->~Value();
        return *new (this) Value(TYPE_OBJECT);
    }

    Value& operator[] (std::string_view key);
    const Value& operator[] (std::string_view key) const;

    MemberIterator memberBegin()
    {
        assert(type_ == TYPE_OBJECT);
        return o_->begin();
    }

    ConstMemberIterator memberBegin() const
    {
        return const_cast<Value&>(*this).memberBegin();
    }

    MemberIterator memberEnd()
    {
        assert(type_ == TYPE_OBJECT);
        return o_->end();
    }

    ConstMemberIterator memberEnd() const
    {
        return const_cast<Value&>(*this).memberEnd();
    }

    MemberIterator findMember(std::string_view key);

    ConstMemberIterator findMember(std::string_view key) const;

    template <typename K, typename V>
    Value& addMember(K&& key, V&& value)
    {
        return addMember(Value(std::forward<K>(key)),
                         Value(std::forward<V>(value)));
    };
    Value& addMember(Value&& key, Value&& value);

    template <typename T>
    Value& addValue(T&& value)
    {
        assert(type_ == TYPE_ARRAY);
        a_->emplace_back(std::forward<T>(value));
        return a_->back();
    }

    const Value& operator[] (size_t i) const
    {
        assert(type_ == TYPE_ARRAY);
        return (*a_)[i];
    }

    Value& operator[] (size_t i)
    {
        assert(type_ == TYPE_ARRAY);
        return (*a_)[i];
    }

    template <typename Handler>
    bool writeTo(Handler& handler) const;

private:
    ValueType type_;

    union {
        int32_t  i32_;
        int64_t  i64_;
        double   d_;
        String*  s_;
        Array*   a_;
        Object*  o_;
    };
};

struct Member
{
    Member(Value&& key_, Value&& value_):
            key(std::move(key_)),
            value(std::move(value_))
    {}

    Member(std::string_view key_, Value&& value_):
            key(key_),
            value(std::move(value_))
    {}

    Value key;
    Value value;
};


#define CALL(expr) do { if (!(expr)) return false; } while(false)

template <typename Handler>
inline bool Value::writeTo(Handler& handler) const
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
            CALL(handler.String(getString()));
            break;
        case TYPE_ARRAY:
            CALL(handler.StartArray());
            for (auto& val: getArray()) {
                CALL(val.writeTo(handler));
            }
            CALL(handler.EndArray());
            break;
        case TYPE_OBJECT:
            CALL(handler.StartObject());
            for (auto& member: getObject()) {
                handler.Key(member.key.getString());
                CALL(member.value.writeTo(handler));
            }
            CALL(handler.EndObject());
            break;
        default:
            assert(false && "bad type");
    }
    return true;
}

#undef CALL

}

#endif //TJSON_VALUE_H
