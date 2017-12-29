add_executable(bench_parse bench_parse.cc)

target_link_libraries(bench_parse jackson benchmark pthread)