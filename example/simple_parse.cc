#include <cstdio>

#include "Reader.h"
#include "Writer.h"
#include "StringReadStream.h"
#include "FileWriteStream.h"

using namespace tjson;

int main()
{
    StringReadStream is("[\n"
                                "  {\n"
                                "    \"precision\": \"zip\",\n"
                                "    \"Latitude\": 37.766800000000003,\n"
                                "    \"Longitude\": -122.3959,\n"
                                "    \"Address\": \"\",\n"
                                "    \"City\": \"SAN FRANCISCO\",\n"
                                "    \"State\": \"CA\",\n"
                                "    \"Zip\": \"94107\",\n"
                                "    \"Country\": \"US\"\n"
                                "  },\n"
                                "  {\n"
                                "    \"precision\": \"zip\",\n"
                                "    \"Latitude\": 37.371991000000001,\n"
                                "    \"Longitude\": -122.02602,\n"
                                "    \"Address\": \"\",\n"
                                "    \"City\": \"SUNNYVALE\",\n"
                                "    \"State\": \"CA\",\n"
                                "    \"Zip\": \"94085\",\n"
                                "    \"Country\": \"US\"\n"
                                "  }\n"
                                "]");
    FileWriteStream os(stdout);
    Writer writer(os);

    ParseError err = Reader::parse(is, writer);
    if (err != PARSE_OK) {
        printf(parseErrorStr(err));
    }
}