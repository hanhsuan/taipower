file(GLOB_RECURSE srcs *.c)

SET(TEST_NAME "unittest")

ADD_EXECUTABLE(${TEST_NAME} ${srcs})

TARGET_LINK_LIBRARIES(${TEST_NAME} PUBLIC ${PROJECT_NAME})

TARGET_INCLUDE_DIRECTORIES(${TEST_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/inc)

ADD_TEST(NAME ut COMMAND ${TEST_NAME})