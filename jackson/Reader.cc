//
// Created by frank on 17-12-26.
//

#include <jackson/Reader.h>

using namespace json;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
void Reader::encodeUtf8(std::string& buffer, unsigned u)
{
    // unicode stuff from Milo's tutorial
    switch (u) {
        case 0x00 ... 0x7F:
            buffer.push_back(u & 0xFF); break;
        case 0x080 ... 0x7FF:
            buffer.push_back(0xC0 | ((u >> 6) & 0xFF));
            buffer.push_back(0x80 | (u & 0x3F)); break;
        case 0x0800 ... 0xFFFF:
            buffer.push_back(0xE0 | ((u >> 12) & 0xFF));
            buffer.push_back(0x80 | ((u >> 6) & 0x3F));
            buffer.push_back(0x80 | (u & 0x3F)); break;
        case 0x010000 ... 0x10FFFF:
            buffer.push_back(0xF0 | ((u >> 18) & 0xFF));
            buffer.push_back(0x80 | ((u >> 12) & 0x3F));
            buffer.push_back(0x80 | ((u >> 6) & 0x3F));
            buffer.push_back(0x80 | (u & 0x3F)); break;
        default: assert(false && "out of range");
    }
}
#pragma GCC diagnostic pop