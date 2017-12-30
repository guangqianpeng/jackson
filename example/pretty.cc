//
// Created by frank on 17-12-26.
//

#include <jackson/Document.h>
#include <jackson/PrettyWriter.h>
#include <jackson/FileWriteStream.h>

#include "sample.h"

using namespace json;

int main()
{
    Document document;
    ParseError err = document.parse(sample[1]);
    if (err != PARSE_OK) {
        puts(parseErrorStr(err));
        return 1;
    }

    FileWriteStream os(stdout);
    PrettyWriter writer(os);
    document.writeTo(writer);
}