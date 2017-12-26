add_executable(simple_parse simple_parse.cc)
target_link_libraries(simple_parse tjson)

add_executable(simple_generate simple_generate.cc)
target_link_libraries(simple_generate tjson)

add_executable(simple_document simple_document.cc)
target_link_libraries(simple_document tjson)

add_executable(add_one add_one.cc)
target_link_libraries(add_one tjson)

