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
#include <atomic>

#include <jackson/noncopyable.h>

namespace json
{

using namespace std::string_view_literals;

enum ValueType {
    TYPE_NULL,
    TYPE_BOOL,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_OBJECT,
};

struct Member;
class Document;

class Value
{
    friend class Document;
public:
    typedef std::vector<Member>::iterator       MemberIterator;
    typedef std::vector<Member>::const_iterator ConstMemberIterator;

public:
    explicit Value(ValueType type = TYPE_NULL);

    explicit Value(bool b):
            type_(TYPE_BOOL),
            b_(b)
    {}

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
            s_(new StringWithRefCount(s.begin(), s.end()))
    {}

    explicit Value(const char* s):
            type_(TYPE_STRING),
            s_(new StringWithRefCount(s, s + strlen(s)))
    {}

    Value(const char* s, size_t len):
            Value(std::string_view(s, len))
    {}

    Value(const Value& rhs);
    Value(Value&& rhs);

    Value& operator=(const Value& rhs);
    Value& operator=(Value&& rhs);

    ~Value();

    ValueType getType() const { return type_; }
    size_t getSize() const;

    bool isNull()   const { return type_ == TYPE_NULL; }
    bool isBool()   const { return type_ == TYPE_BOOL; }
    bool isInt32()  const { return type_ == TYPE_INT32; }
    bool isInt64()  const { return type_ == TYPE_INT64 || type_ == TYPE_INT32; }
    bool isDouble() const { return type_ == TYPE_DOUBLE; }
    bool isString() const { return type_ == TYPE_STRING; }
    bool isArray()  const { return type_ == TYPE_ARRAY; }
    bool isObject() const { return type_ == TYPE_OBJECT; }

    // getter && setter
    bool getBool() const
    {
        assert(type_ == TYPE_BOOL);
        return b_;
    }

    int32_t getInt32() const
    {
        assert(type_ == TYPE_INT32);
        return i32_;
    }

    int64_t getInt64() const
    {
        assert(type_ == TYPE_INT64 || type_ == TYPE_INT32);
        return type_ == TYPE_INT64 ? i64_ : i32_;
    }

    double getDouble() const
    {
        assert(type_ == TYPE_DOUBLE);
        return d_;
    }

    std::string_view getStringView() const
    {
        assert(type_ == TYPE_STRING);
        return std::string_view(&*s_->data.begin(), s_->data.size());
    }

    std::string getString() const
    {
        return std::string(getStringView());
    }

    const auto& getArray() const
    {
        assert(type_ == TYPE_ARRAY);
        return a_->data;
    }

    const auto& getObject() const
    {
        assert(type_ == TYPE_OBJECT);
        return o_->data;
    }

    Value& setNull()
    {
        this->~Value();
        return *new (this) Value(TYPE_NULL);
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
        return o_->data.begin();
    }

    ConstMemberIterator memberBegin() const
    {
        return const_cast<Value&>(*this).memberBegin();
    }

    MemberIterator memberEnd()
    {
        assert(type_ == TYPE_OBJECT);
        return o_->data.end();
    }

    ConstMemberIterator memberEnd() const
    {
        return const_cast<Value&>(*this).memberEnd();
    }

    MemberIterator findMember(std::string_view key);

    ConstMemberIterator findMember(std::string_view key) const;

    template <typename V>
    Value& addMember(const char* key, V&& value)
    {
        return addMember(Value(key),
                         Value(std::forward<V>(value)));
    };


    Value& addMember(Value&& key, Value&& value);

    template <typename T>
    Value& addValue(T&& value)
    {
        assert(type_ == TYPE_ARRAY);
        a_->data.emplace_back(std::forward<T>(value));
        return a_->data.back();
    }

    const Value& operator[] (size_t i) const
    {
        assert(type_ == TYPE_ARRAY);
        return a_->data[i];
    }

    Value& operator[] (size_t i)
    {
        assert(type_ == TYPE_ARRAY);
        return a_->data[i];
    }

    template <typename Handler>
    bool writeTo(Handler& handler) const;

private:
    ValueType type_;

    template <typename T>
    struct AddRefCount
    {
        template <typename... Args>
        AddRefCount(Args&&... args)
                : refCount(1)
                , data(std::forward<Args>(args)...)
        {}
        ~AddRefCount()
        { assert(refCount == 0); }

        int incrAndGet()
        {
            assert(refCount > 0);
            return ++refCount;
        }

        int decrAndGet()
        {
            assert(refCount > 0);
            return --refCount;
        }

        std::atomic_int refCount;
        T data;
    };

    typedef AddRefCount<std::vector<char>>   StringWithRefCount;
    typedef AddRefCount<std::vector<Value>>  ArrayWithRefCount;
    typedef AddRefCount<std::vector<Member>> ObjectWithRefCount;

    union {
        bool     b_;
        int32_t  i32_;
        int64_t  i64_;
        double   d_;
        StringWithRefCount*  s_;
        ArrayWithRefCount*   a_;
        ObjectWithRefCount*  o_;
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
        case TYPE_BOOL:
            CALL(handler.Bool(b_));
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
            CALL(handler.String(getStringView()));
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
                handler.Key(member.key.getStringView());
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
