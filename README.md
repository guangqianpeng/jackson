# jackson: A simple and fast JSON parser/generator

[![Build Status](https://travis-ci.org/guangqianpeng/jackson.svg?branch=master)](https://travis-ci.org/guangqianpeng/jackson)

## 简介

jackson是一个简单快速的JSON解析器/生成器, 支持DOM和SAX两种风格的API. jackson定义了3个concept: `ReadStream`, `WriteStream`, `Handler`

- `ReadStream`用于读取字符流, 目前实现了`FileReadStream`和`StringReadStream`, 分别从文件和内存中读取.

- `WriteStream`用于输出字节流, 目前实现了`FileWriteStream`和`StringWriteString`, 分别输出至文件和内存.

- `Handler`是解析时的事件处理器, 是实现SAX风格API的关键. 目前实现了`Write`, `PrettyWriter`和`Document`. 前两者接收事件后输出字符串至`WriteStream`, 而`Document`接收事件后则构建树形存储结构.

用户可以自定义并自行组合这3个concept, 例如将多个`Handler`串联起来完成复杂的任务(`PrettyWriter`内部就串联`Writer`). 这种灵活性依赖于C++的模板机制而非虚函数.

## 读写JSON

DOM (**D**ocument **O**bject **M**odel) 风格的API使用方法如下:

```C++
int main()
{
    Document document;
    ParseError err = document.parse("{"
    "    \"precision\": \"zip\","
    "    \"Latitude\": 37.766800000000003,"
    "    \"Longitude\": -122.3959,"
    "    \"Address\": \"\","
    "    \"City\": \"SAN FRANCISCO\","
    "    \"State\": \"CA\","
    "    \"Zip\": \"94107\","
    "    \"Country\": \"US\""
    "    }");

    if (err != PARSE_OK) {
        puts(parseErrorStr(err));
        exit(1);
    }

    // get 'State' field
    Value& state = document["State"];
    std::cout << "State: " << state.getString() << '\n';

    // get 'Zip' field
    Value& zip = document["Zip"];
    std::cout << "Zip: " << zip.getString() << "\n";

    // set 'Zip' field
    zip.setInt32(9527);
    std::cout << "Zip: " << zip.getInt32() << "\n";
}
```

`Document`也是一个`Value`, 可以调用`getXXX()`和`setXXX()`访问. `Value`内部使用`union`来存储不同类型的值, 若`getXXX()`类型不匹配会在Debug时崩溃, 而`setXXX()`则没有这种限制. `Document`将所有数据都保存在内存里方便读写, 对于小型json文档很合适. SAX (**S**imple **A**PI for **X**ML) 风格的API使用如下:

```c++
template <typename Handler>
class AddOne: noncopyable
{
public:
    explicit AddOne(Handler& handler): handler_(handler) {}

    bool Null()                { return handler_.Null(); }
    bool Bool(bool b)          { return handler_.Bool(b); }
    bool Int32(int32_t i32)    { return handler_.Int32(i32 + 1); } // add one
    bool Int64(int64_t i64)    { return handler_.Int64(i64 + 1); } // add one
    bool Double(double d)      { return handler_.Double(d + 1); }  // add one
    bool String(string_view s) { return handler_.String(s); }
    bool StartObject()         { return handler_.StartObject(); }
    bool Key(string_view s)    { return handler_.Key(s); }
    bool EndObject()           { return handler_.EndObject(); }
    bool StartArray()          { return handler_.StartArray(); }
    bool EndArray()            { return handler_.EndArray(); }

private:
    Handler& handler_;
};

int main()
{
    FileReadStream is(stdin);
    FileWriteStream os(stdout);
    Writer writer(os);
    AddOne addOne(writer);

    ParseError err = Reader::parse(is, addOne);
    if (err != PARSE_OK) {
        printf(parseErrorStr(err));
    }
}
```

代码定义了一个`Handler AddOne`, 将json文档中的所有数字加1. `main`函数中把`AddOne`和`Writer`(另一个`Handler`)组合起来输出至`stdout`. 事件处理的过程中并没有创建`Value`或者`Document`, 即使对于大体积的json文档也不必担心内存占用.

## 生成JSON

生成json文档只需要手动调用`Handler`即可:

```C++
int main()
{
    FileWriteStream os(stdout);
    Writer writer(os);

    writer.StartArray();
    writer.Bool(true);
    writer.String("\0蛤蛤蛤\0"sv); // null character is OK
    writer.Bool(true);
    writer.EndArray();
}
```

输出

```json
[true,"\u0000蛤蛤蛤\u0000",true]
```

## 测试&&性能

jackson使用[Google Test](https://github.com/google/googletest)和[Google Benchmark](https://github.com/google/benchmark), 分别见`test`和`benchmark`目录.

## 安装

```shell
$ git clone git@github.com:guangqianpeng/jackson.git
$ cd jackson
$ git submodule update --init --recursive
$ ./build.sh 
$ ./build.sh install
$ ./build.sh test
```

## 参考

- [RapidJSON](https://github.com/Tencent/rapidjson): A fast JSON parser/generator for C++ with both SAX/DOM style API.
- [JSON tutorial](https://github.com/miloyip/json-tutorial): 从零开始的 JSON 库教程.