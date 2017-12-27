//
// Created by frank on 17-12-26.
//

#include <jackson/Document.h>
#include <jackson/PrettyWriter.h>
#include <jackson/FileWriteStream.h>

#include "json.h"

using namespace json;

int main()
{
    Document document;
    ParseError err = document.parse(json[1]);
    if (err != PARSE_OK) {
        puts(parseErrorStr(err));
        return 1;
    }

    FileWriteStream os(stdout);
    PrettyWriter writer(os);
    writeTo(document, writer);
}