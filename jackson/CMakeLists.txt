add_library(jackson STATIC
        Exception.h
        FileReadStream.h FileWriteStream.h
        StringReadStream.h StringWriteStream.h
        Reader.cc Reader.h
        Writer.cc Writer.h
        Value.cc Value.h
        Document.h
        noncopyable.h
        PrettyWriter.h)
install(TARGETS jackson DESTINATION lib)

set(HEADERS
        Document.h
        Exception.h
        FileReadStream.h
        FileWriteStream.h
        noncopyable.h
        PrettyWriter.h
        Reader.h
        StringReadStream.h
        StringWriteStream.h
        Value.h
        Writer.h
        )
install(FILES ${HEADERS} DESTINATION include)