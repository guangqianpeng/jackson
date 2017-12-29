//
// Created by frank on 17-12-26.
//

#ifndef TJSON_NONCOPYABLE_H
#define TJSON_NONCOPYABLE_H

namespace json
{

class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

}

#endif //TJSON_NONCOPYABLE_H
