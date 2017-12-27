//
// Created by frank on 17-12-26.
//

#include "json.h"
#include "Document.h"
#include "Writer.h"
#include "FileWriteStream.h"

using namespace tjson;

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