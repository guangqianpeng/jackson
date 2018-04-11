add_executable(parse parse.cc)
target_link_libraries(parse jackson)

add_executable(generate generate.cc)
target_link_libraries(generate jackson)

add_executable(print_document print_document.cc)
target_link_libraries(print_document jackson)

add_executable(add_one add_one.cc sample.h)
target_link_libraries(add_one jackson)

add_executable(pretty pretty.cc)
target_link_libraries(pretty jackson)

add_executable(write_document write_document.cc)
target_link_libraries(write_document jackson)

add_executable(struct struct.cc)
target_link_libraries(struct jackson)
