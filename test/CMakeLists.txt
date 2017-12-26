find_package(GTest REQUIRED)
enable_testing()

add_executable(test_tjson test.cc)
target_link_libraries(test_tjson tjson ${GTEST_BOTH_LIBRARIES} pthread)

add_test(all test_json)