//
// Created by frank on 17-12-26.
//

#include "Value.h"

using namespace tjson;

Value::Value(ValueType type): type_(type), s_(nullptr)
{
    switch (type_) {
        case TYPE_NULL:   break;
        case TYPE_TRUE:   break;
        case TYPE_FALSE:  break;
        case TYPE_INT32:  break;
        case TYPE_INT64:  break;
        case TYPE_DOUBLE: break;
        case TYPE_STRING: s_ = new String(); break;
        case TYPE_ARRAY:  a_ = new Array();  break;
        case TYPE_OBJECT: o_ = new Object(); break;
        default: assert(false && "bad value type in ctor");
    }
}

Value::~Value() {
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

    o_->emplace_back(key, Value(TYPE_NULL));
    return o_->back().value;
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
    assert(type_ == TYPE_OBJECT);
    return std::find_if(o_->begin(), o_->end(), [key](const Member& m)->bool {
        return m.key.getString() == key;
    });
}