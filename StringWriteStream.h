//
// Created by frank on 17-12-26.
//

#ifndef TJSON_STRINGWRITESTREAM_H
#define TJSON_STRINGWRITESTREAM_H

#include <string>

#include "noncopyable.h"

namespace tjson
{

class StringWriteStream: noncopyable
{
public:
    void put(char c)
    { buffer_.push_back(c); }
    void put(std::string_view str)
    { buffer_.append(str); }
    const std::string& get() const
    { return buffer_; }

private:
    std::string buffer_;
};

}

#endif //TJSON_STRINGWRITESTREAM_H
