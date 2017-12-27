//
// Created by frank on 17-12-24.
//

#ifndef TJSON_READER_H
#define TJSON_READER_H

#include <cassert>
#include <string>
#include <vector>
#include <variant>
#include <memory>

#include "Exception.h"
#include "Value.h"

namespace json
{

class Reader: noncopyable
{
public:
    template <typename ReadStream, typename Handler>
    static ParseError parse(ReadStream& is, Handler& handler)
    {
        try {
            parseWhitespace(is);
            parseValue(is, handler);
            parseWhitespace(is);
            if (is.hasNext())
                throw Exception(PARSE_ROOT_NOT_SINGULAR);
            return PARSE_OK;
        } catch (Exception& e) {
            return e.err();
        }
    }

private:

#define CALL(expr) \
    if (!(expr)) throw Exception(PARSE_USER_STOPPED)

    template <typename ReadStream>
    static unsigned parseHex4(ReadStream& is)
    {
        // unicode stuff from Milo's tutorial
        unsigned u = 0;
        for (int i = 0; i < 4; i++) {
            u <<= 4;
            switch (char ch = is.next()) {
                case '0'...'9': u |= ch - '0'; break;
                case 'a'...'f': u |= ch - 'a' + 10; break;
                case 'A'...'F': u |= ch - 'A' + 10; break;
                default: throw Exception(PARSE_BAD_UNICODE_HEX);
            }
        }
        return u;
    }

    template <typename ReadStream>
    static void parseWhitespace(ReadStream& is)
    {
        while (is.hasNext()) {
            char ch = is.peek();
            if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
                is.next();
            else break;
        }
    }

    template <typename ReadStream, typename Handler>
    static void parseLiteral(ReadStream& is, Handler& handler, const char* literal, ValueType type)
    {
        is.assertNext(*literal++);
        while (*literal != '\0' && *literal == is.peek()) {
            literal++;
            is.next();
        }
        if (*literal == '\0') {
            switch (type) {
                case TYPE_NULL:
                    CALL(handler.Null());
                    return;
                case TYPE_TRUE:
                    CALL(handler.Bool(true));
                    return;
                case TYPE_FALSE:
                    CALL(handler.Bool(false));
                    return;
                default:
                    assert(false && "bad type");
            }
        }
        throw Exception(PARSE_BAD_VALUE);
    }

    template <typename ReadStream, typename Handler>
    static void parseNumber(ReadStream& is, Handler& handler)
    {
        bool useDouble = false;
        auto start = is.getIter();

        if (is.peek() == '-')
            is.next();

        if (is.peek() == '0') {
            is.next();
            if (isdigit(is.peek()))
                throw Exception(PARSE_BAD_VALUE);
        } else if (isDigit19(is.peek())) {
            is.next();
            while (isDigit(is.peek())) is.next();
        } else throw Exception(PARSE_BAD_VALUE);
        if (is.peek() == '.') {
            useDouble = true;
            is.next();
            if (!isDigit(is.peek()))
                throw Exception(PARSE_BAD_VALUE);
            is.next();
            while (isDigit(is.peek()))
                is.next();
        }
        if (is.peek() == 'e' || is.peek() == 'E') {
            useDouble = true;
            is.next();
            if (is.peek() == '+' || is.peek() == '-')
                is.next();
            if (!isDigit(is.peek()))
                throw Exception(PARSE_BAD_VALUE);
            is.next();
            while (isDigit(is.peek())) is.next();
        }

        auto end = is.getIter();
        if (start == end)
            throw Exception(PARSE_BAD_VALUE);

        try {
            std::size_t idx = 0;
            std::string buffer(start, end);
            if (useDouble) {
                double d = std::stod(buffer, &idx);
                assert(idx == buffer.length());
                CALL(handler.Double(d));
            } else {
                int64_t i64 = std::stol(buffer, &idx);
                assert(idx == buffer.length());
                if (i64 <= std::numeric_limits<int32_t>::max() &&
                    i64 >= std::numeric_limits<int32_t>::min()) {
                    CALL(handler.Int32(static_cast<int32_t>(i64)));
                }
                else
                    CALL(handler.Int64(i64));
            }
        } catch (std::out_of_range &e) {
            throw Exception(PARSE_NUMBER_TOO_BIG);
        }
    }

    template <typename ReadStream, typename Handler>
    static void parseString(ReadStream& is, Handler& handler, bool isKey)
    {
        is.assertNext('"');
        std::string buffer;
        while (is.hasNext()) {
            switch (char ch = is.next()) {
                case '"':
                    if (isKey) {
                        CALL(handler.Key(std::move(buffer)));
                    }
                    else {
                        CALL(handler.String(std::move(buffer)));
                    }
                    return;
                case '\x01'...'\x1f':
                    throw Exception(PARSE_BAD_STRING_CHAR);
                case '\\':
                    switch (is.next()) {
                        case '"':  buffer.push_back('"');  break;
                        case '\\': buffer.push_back('\\'); break;
                        case '/':  buffer.push_back('/');  break;
                        case 'b':  buffer.push_back('\b'); break;
                        case 'f':  buffer.push_back('\f'); break;
                        case 'n':  buffer.push_back('\n'); break;
                        case 'r':  buffer.push_back('\r'); break;
                        case 't':  buffer.push_back('\t'); break;
                        case 'u': {
                            // unicode stuff from Milo's tutorial
                            unsigned u = parseHex4(is);
                            if (u >= 0xD800 && u <= 0xDBFF) {
                                if (is.next() != '\\')
                                    throw Exception(PARSE_BAD_UNICODE_SURROGATE);
                                if (is.next() != 'u')
                                    throw Exception(PARSE_BAD_UNICODE_SURROGATE);
                                unsigned u2 = parseHex4(is);
                                if (u2 >= 0xDC00 && u2 <= 0xDFFF)
                                    u = 0x10000 + (u - 0xD800) * 0x400 + (u2 - 0xDC00);
                                else
                                    throw Exception(PARSE_BAD_UNICODE_SURROGATE);
                            }
                            encodeUtf8(buffer, u);
                            break;
                        }
                        default: throw Exception(PARSE_BAD_STRING_ESCAPE);
                    }
                    break;
                default: buffer.push_back(ch);
            }
        }
        throw Exception(PARSE_MISS_QUOTATION_MARK);
    }

    template <typename ReadStream, typename Handler>
    static void parseArray(ReadStream& is, Handler& handler)
    {
        CALL(handler.StartArray());

        is.assertNext('[');
        parseWhitespace(is);
        if (is.peek() == ']') {
            is.next();
            CALL(handler.EndArray());
            return;
        }

        while (true) {
            parseValue(is, handler);
            parseWhitespace(is);
            switch (is.next()) {
                case ',':
                    parseWhitespace(is);
                    break;
                case ']':
                    CALL(handler.EndArray());
                    return;
                default:
                    throw Exception(PARSE_MISS_COMMA_OR_SQUARE_BRACKET);
            }
        }
    }

    template <typename ReadStream, typename Handler>
    static void parseObject(ReadStream& is, Handler& handler)
    {
        CALL(handler.StartObject());

        is.assertNext('{');
        parseWhitespace(is);
        if (is.peek() == '}') {
            is.next();
            CALL(handler.EndObject());
            return;
        }

        while (true) {

            if (is.peek() != '"')
                throw Exception(PARSE_MISS_KEY);

            parseString(is, handler, true);

            // parse ':'
            parseWhitespace(is);
            if (is.next() != ':')
                throw Exception(PARSE_MISS_COLON);
            parseWhitespace(is);

            // go on
            parseValue(is, handler);
            parseWhitespace(is);
            switch (is.next()) {
                case ',':
                    parseWhitespace(is);
                    break;
                case '}':
                    CALL(handler.EndObject());
                    return;
                default:
                    throw Exception(PARSE_MISS_COMMA_OR_CURLY_BRACKET);
            }
        }
    }

#undef CALL

    template <typename ReadStream, typename Handler>
    static void parseValue(ReadStream& is, Handler& handler)
    {
        if (!is.hasNext())
            throw Exception(PARSE_EXPECT_VALUE);

        switch (is.peek()) {
            case 'n': return parseLiteral(is, handler, "null", TYPE_NULL);
            case 't': return parseLiteral(is, handler, "true", TYPE_TRUE);
            case 'f': return parseLiteral(is, handler, "false", TYPE_FALSE);
            case '"': return parseString(is, handler, false);
            case '[': return parseArray(is, handler);
            case '{': return parseObject(is, handler);
            default:  return parseNumber(is, handler);
        }
    }

private:
    static bool isDigit(char ch)
    { return ch >= '0' && ch <= '9'; }
    static bool isDigit19(char ch)
    { return ch >= '1' && ch <= '9'; }
    static void encodeUtf8(std::string& buffer, unsigned u);
};

}

#endif //TJSON_READER_H
