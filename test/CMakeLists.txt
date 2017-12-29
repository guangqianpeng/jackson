find_package(GTest REQUIRED)

add_executable(test_error test_error.cc)
target_link_libraries(test_error jackson ${GTEST_BOTH_LIBRARIES} pthread)

add_executable(test_value test_value.cc)
target_link_libraries(test_value jackson ${GTEST_BOTH_LIBRARIES} pthread)

add_executable(test_roundtrip test_roundtrip.cc)
target_link_libraries(test_roundtrip jackson ${GTEST_BOTH_LIBRARIES} pthread)

add_test(all test_error test_value test_roundtrip)