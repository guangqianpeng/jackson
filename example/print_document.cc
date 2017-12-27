//
// Created by frank on 17-12-26.
//

#include <jackson/Document.h>
#include <jackson/Writer.h>
#include <jackson/FileWriteStream.h>

#include "json.h"

using namespace json;

int main()
{
    Document document;
    ParseError err = document.parse(json[0]);
    if (err != PARSE_OK) {
        puts(parseErrorStr(err));
        return 1;
    }

    FileWriteStream os(stdout);
    Writer writer(os);
    writeTo(document, writer);
}