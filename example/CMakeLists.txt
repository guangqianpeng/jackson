add_executable(simple_parse parse.cc)
target_link_libraries(simple_parse tjson)

add_executable(simple_generate generate.cc)
target_link_libraries(simple_generate tjson)

add_executable(simple_document print_document.cc)
target_link_libraries(simple_document tjson)

add_executable(add_one add_one.cc json.h)
target_link_libraries(add_one tjson)

add_executable(pretty pretty.cc)
target_link_libraries(pretty tjson)
