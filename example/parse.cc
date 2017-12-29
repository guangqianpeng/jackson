#include <cstdio>
#include <jackson/Reader.h>
#include <jackson/Writer.h>
#include <jackson/FileReadStream.h>
#include <jackson/FileWriteStream.h>

using namespace json;

int main()
{
    FileReadStream is(stdin);
    FileWriteStream os(stdout);
    Writer writer(os);

    ParseError err = Reader::parse(is, writer);
    if (err != PARSE_OK) {
        printf("%s\n", parseErrorStr(err));
    }
}