//
// Created by frank on 17-12-25.
//

#ifndef TJSON_STRINGREADSTREAM_H
#define TJSON_STRINGREADSTREAM_H

#include <string>
#include <cassert>

#include <jackson/noncopyable.h>

namespace json
{


class StringReadStream: noncopyable
{
public:
    using Iterator = std::string_view::iterator;

public:
    explicit
    StringReadStream(std::string_view json)
            : json_(json),
              iter_(json_.begin())
    {}

    bool hasNext() const
    { return iter_ != json_.end(); }

    char peek()
    {
        return hasNext() ? *iter_ : '\0';
    }

    Iterator getIter() const
    {
        return iter_;
    }

    char next()
    {
        if (hasNext()) {
            char ch = *iter_;
            iter_++;
            return ch;
        }
        return '\0';
    };

    void assertNext(char ch)
    {
        assert(peek() == ch);
        next();
    }

private:
    std::string_view  json_;
    Iterator          iter_;
};


}
#endif //TJSON_STRINGREADSTREAM_H
