add_library(jackson STATIC
        Exception.h
        FileReadStream.h FileWriteStream.h
        StringReadStream.h StringWriteStream.h
        Reader.cc Reader.h
        Writer.h
        Value.cc Value.h
        Document.h
        noncopyable.h
        PrettyWriter.h)