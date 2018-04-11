//
// Created by frank on 17-12-28.
//

#include <iostream>

#include <jackson/Document.h>
#include <jackson/Writer.h>
#include <jackson/FileWriteStream.h>

using namespace json;

int main()
{
    Document document;
    ParseError err = document.parse(R"(
{
    "precision": "zip",
    "Latitude": 37.766800000000003,
    "Longitude": -122.3959,
    "Address": "sadfasdf",
    "City": "SAN FRANCISCO",
    "State": "CA",
    "Zip": "94107",
    "Country": "US",
    "Number": 0i32,
    "Number2": 0i64
}
)");

    if (err != PARSE_OK) {
        puts(parseErrorStr(err));
        exit(1);
    }


    Value& state = document["State"];
    std::cout << "State: " << state.getStringView() << '\n';

    Value& zip = document["Zip"];
    std::cout << "Zip: " << zip.getStringView() << "\n";

    zip.setInt32(9527);
    std::cout << "Zip: " << zip.getInt32() << "\n";

    document.addMember("123", "456");
}