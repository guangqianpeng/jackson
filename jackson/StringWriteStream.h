//
// Created by frank on 17-12-26.
//

#ifndef TJSON_STRINGWRITESTREAM_H
#define TJSON_STRINGWRITESTREAM_H

#include <string_view>
#include <vector>

#include <jackson/noncopyable.h>

namespace json
{

class StringWriteStream: noncopyable
{
public:
    void put(char c)
    {
        buffer_.push_back(c);
    }
    void put(std::string_view str)
    {
        buffer_.insert(buffer_.end(), str.begin(), str.end());
    }
    std::string_view get() const
    {
        return std::string_view(&*buffer_.begin(), buffer_.size());
    }

private:
    std::vector<char> buffer_;
};

}

#endif //TJSON_STRINGWRITESTREAM_H
