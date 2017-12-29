set(GOOGLETEST_ROOT third_party/gtest/googletest CACHE STRING "Google Test source root")
include_directories(SYSTEM
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/include
        )

set(GOOGLETEST_SOURCES
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/src/gtest-all.cc
        ${PROJECT_SOURCE_DIR}/${GOOGLETEST_ROOT}/src/gtest_main.cc
        )

foreach(_source ${GOOGLETEST_SOURCES})
    set_source_files_properties(${_source} PROPERTIES GENERATED 1)
endforeach()

add_library(gtest ${GOOGLETEST_SOURCES})
target_link_libraries(gtest pthread)

add_executable(test_error test_error.cc)
target_link_libraries(test_error jackson gtest)

add_executable(test_value test_value.cc)
target_link_libraries(test_value jackson gtest)

add_executable(test_roundtrip test_roundtrip.cc)
target_link_libraries(test_roundtrip jackson gtest)

set(TEST_DIR ${EXECUTABLE_OUTPUT_PATH})
add_test(test_error ${TEST_DIR}/test_error)
add_test(test_value ${TEST_DIR}/test_value)
add_test(test_roundtrip ${TEST_DIR}/test_roundtrip)