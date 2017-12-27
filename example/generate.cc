//
// Created by frank on 17-12-26.
//

#include <cstdio>

#include "Writer.h"
#include "FileWriteStream.h"

using namespace tjson;

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