#include <cstdio>

#include "json.h"
#include "Reader.h"
#include "Writer.h"
#include "StringReadStream.h"
#include "FileWriteStream.h"

using namespace tjson;

int main()
{
    StringReadStream is(json[0]);
    FileWriteStream os(stdout);
    Writer writer(os);

    ParseError err = Reader::parse(is, writer);
    if (err != PARSE_OK) {
        printf(parseErrorStr(err));
    }
}