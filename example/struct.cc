//
// Created by frank on 18-1-29.
//

#include <string>

#include <jackson/Value.h>
#include <jackson/Document.h>
#include <jackson/StringWriteStream.h>
#include <jackson/Writer.h>
#include <jackson/FileWriteStream.h>

using namespace json;

struct Person
{
    explicit Person(const Value& value)
    {
        switch (value.getType())
        {
            case TYPE_OBJECT:
                name = value["name"].getString();
                age = value["age"].getInt32();
                break;
            case TYPE_ARRAY:
                name = value[0].getString();
                age = value[1].getInt32();
                break;
            default:
                assert(false && "bad type");
                break;
        }
    }

    Value toJsonObject() const
    {
        Value value(json::TYPE_OBJECT);
        value.addMember("name", name);
        value.addMember("age", age);
        return value;
    }

    Value toJsonArray() const
    {
        Value value(json::TYPE_ARRAY);
        value.addValue(name);
        value.addValue(age);
        return value;
    }

    std::string name;
    int32_t age;
};

int main()
{
    Value value(TYPE_OBJECT);
    value.addMember("name", "123");
    value.addMember("age", 123);

    Person p(value);

    value = p.toJsonArray();
    FileWriteStream os(stdout);
    Writer writer(os);
    value.writeTo(writer);
}