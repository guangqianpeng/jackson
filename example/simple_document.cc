//
// Created by frank on 17-12-26.
//

#include <iostream>

#include "Document.h"
#include "Writer.h"
#include "FileWriteStream.h"

using namespace tjson;

int main()
{
    Document document;
    ParseError err = document.parse("[\n"
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
    if (err != PARSE_OK) {
        puts(parseErrorStr(err));
        return 1;
    }

    FileWriteStream os(stdout);
    Writer writer(os);
    document.writeTo(writer);
}