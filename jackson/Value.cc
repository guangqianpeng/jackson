//
// Created by frank on 17-12-26.
//

#include <jackson/Value.h>

using namespace json;

Value::Value(const json::Value& rhs)
        : type_(rhs.type_)
        , a_(rhs.a_)
{
    switch (type_) {
        case TYPE_NULL:
        case TYPE_BOOL:
        case TYPE_INT32:
        case TYPE_INT64:
        case TYPE_DOUBLE: break;
        case TYPE_STRING:
            s_->incrAndGet(); break;
        case TYPE_ARRAY:
            a_->incrAndGet(); break;
        case TYPE_OBJECT:
            o_->incrAndGet(); break;
        default: assert(false && "bad value type");
    }
}

Value::Value(Value&& rhs)
        : type_(rhs.type_)
        , a_(rhs.a_)
{
    rhs.type_ = TYPE_NULL;
    rhs.a_ = nullptr;
}

Value& Value::operator=(const json::Value& rhs)
{
    assert(this != &rhs);
    this->~Value();
    type_ = rhs.type_;
    a_ = rhs.a_;
    switch (type_)
    {
        case TYPE_NULL:
        case TYPE_BOOL:
        case TYPE_INT32:
        case TYPE_INT64:
        case TYPE_DOUBLE: break;
        case TYPE_STRING:
            s_->incrAndGet(); break;
        case TYPE_ARRAY:
            a_->incrAndGet(); break;
        case TYPE_OBJECT:
            o_->incrAndGet(); break;
        default: assert(false && "bad value type");
    }
    return *this;
}

Value& Value::operator=(Value&& rhs)
{
    assert(this != &rhs);
    this->~Value();
    type_ = rhs.type_;
    a_ = rhs.a_;
    rhs.type_ = TYPE_NULL;
    rhs.a_ = nullptr;
    return *this;
}

Value::Value(ValueType type)
        : type_(type)
        , s_(nullptr)
{
    switch (type_) {
        case TYPE_NULL:
        case TYPE_BOOL:
        case TYPE_INT32:
        case TYPE_INT64:
        case TYPE_DOUBLE:                                break;
        case TYPE_STRING: s_ = new StringWithRefCount(); break;
        case TYPE_ARRAY:  a_ = new ArrayWithRefCount();  break;
        case TYPE_OBJECT: o_ = new ObjectWithRefCount(); break;
        default: assert(false && "bad value type");
    }
}

Value::~Value()
{
    switch (type_) {
        case TYPE_NULL:
        case TYPE_BOOL:
        case TYPE_INT32:
        case TYPE_INT64:
        case TYPE_DOUBLE: break;
        case TYPE_STRING:
            if (s_->decrAndGet() == 0)
                delete s_;
            break;
        case TYPE_ARRAY:
            if (a_->decrAndGet() == 0)
                delete a_;
            break;
        case TYPE_OBJECT:
            if (o_->decrAndGet() == 0)
                delete o_;
            break;
        default: assert(false && "bad value type");
    }
}

Value& Value::operator[] (std::string_view key)
{
    assert(type_ == TYPE_OBJECT);

    auto it = findMember(key);
    if (it != o_->data.end())
        return it->value;

    assert(false); // unlike std::map
    static Value fake(TYPE_NULL);
    return fake;
}

size_t Value::getSize() const
{
    if (type_ == TYPE_ARRAY)
        return a_->data.size();
    else if (type_ == TYPE_OBJECT)
        return o_->data.size();
    return 1;
}

const Value&  Value::operator[] (std::string_view key) const
{
    return const_cast<Value&>(*this)[key];
}

Value::MemberIterator Value::findMember(std::string_view key)
{
    assert(type_ == TYPE_OBJECT);
    return std::find_if(o_->data.begin(), o_->data.end(), [key](const Member& m)->bool {
        return m.key.getStringView() == key;
    });
}

Value::ConstMemberIterator Value::findMember(std::string_view key) const
{
    return const_cast<Value&>(*this).findMember(key);
}


Value& Value::addMember(Value&& key, Value&& value)
{
    assert(type_ == TYPE_OBJECT);
    assert(key.type_ == TYPE_STRING);
    assert(findMember(key.getStringView()) == memberEnd());
    o_->data.emplace_back(std::move(key), std::move(value));
    return o_->data.back().value;
}