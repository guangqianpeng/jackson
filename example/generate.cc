//
// Created by frank on 17-12-26.
//

#include <cstdio>

#include <jackson/Writer.h>
#include <jackson/FileWriteStream.h>

using namespace json;

int main()
{
    FileWriteStream os(stdout);
    Writer writer(os);

    writer.StartArray();
    writer.Bool(true);
    writer.Bool(true);
    writer.String("asdf");
    writer.EndArray();
}