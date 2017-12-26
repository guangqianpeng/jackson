//
// Created by frank on 17-12-26.
//

#include "Value.h"

using namespace tjson;

Value::Value(ValueType type): type_(type), var_(nullptr)
{
    switch (type_) {
        case TYPE_NULL:   break;
        case TYPE_TRUE:   break;
        case TYPE_FALSE:  break;
        case TYPE_INT32:  break;
        case TYPE_INT64:  break;
        case TYPE_DOUBLE: break;
        case TYPE_STRING: var_ = new Variant(String{}); break;
        case TYPE_ARRAY:  var_ = new Variant(Array{});  break;
        case TYPE_OBJECT: var_ = new Variant(Object{}); break;
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
        case TYPE_STRING:
        case TYPE_ARRAY:
        case TYPE_OBJECT: delete var_; break;
        default: assert(false && "bad value type in dtor");
    }
}