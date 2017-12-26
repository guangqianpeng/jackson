//
// Created by frank on 17-12-26.
//

#ifndef TJSON_FILEWRITESTREAM_H
#define TJSON_FILEWRITESTREAM_H

#include <string>
#include <cstdio>

#include "noncopyable.h"

namespace tjson
{

class FileWriteStream: noncopyable
{
public:
    explicit FileWriteStream(FILE* output):
            output_(output) {}

    void put(char c)
    { putc(c, output_); }
    void put(const char* str)
    { fputs(str, output_); }
    void put(const std::string& str)
    { fputs(str.c_str(), output_); }

private:
    FILE* output_;
};

}

#endif //TJSON_FILEWRITESTREAM_H
