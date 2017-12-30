//
// Created by frank on 17-12-26.
//

#include <jackson/Value.h>

using namespace json;

Value::Value(ValueType type): type_(type), s_(nullptr)
{
    switch (type_) {
        case TYPE_NULL:
        case TYPE_TRUE:
        case TYPE_FALSE:
        case TYPE_INT32:
        case TYPE_INT64:
        case TYPE_DOUBLE: break;
        case TYPE_STRING: s_ = new String(); break;
        case TYPE_ARRAY:  a_ = new Array();  break;
        case TYPE_OBJECT: o_ = new Object(); break;
        default: assert(false && "bad value type in ctor");
    }
}

Value::~Value()
{
    switch (type_) {
        case TYPE_NULL:
        case TYPE_TRUE:
        case TYPE_FALSE:
        case TYPE_INT32:
        case TYPE_INT64:
        case TYPE_DOUBLE: break;
        case TYPE_STRING: delete s_; break;
        case TYPE_ARRAY:  delete a_; break;
        case TYPE_OBJECT: delete o_; break;
        default: assert(false && "bad value type in dtor");
    }
}

Value& Value::operator[] (std::string_view key)
{
    assert(type_ == TYPE_OBJECT);

    auto it = findMember(key);
    if (it != o_->end())
        return it->value;

    assert(false); // unlike std::map
    static Value fake(TYPE_NULL);
    return fake;
}

const Value&  Value::operator[] (std::string_view key) const
{
    return const_cast<Value&>(*this)[key];
}

Value::MemberIterator Value::findMember(std::string_view key)
{
    assert(type_ == TYPE_OBJECT);
    return std::find_if(o_->begin(), o_->end(), [key](const Member& m)->bool {
        return m.key.getString() == key;
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
    assert(findMember(key.getString()) == memberEnd());
    o_->emplace_back(std::move(key), std::move(value));
    return o_->back().value;
}