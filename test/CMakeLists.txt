
set(SRC_LIST Timestamp_test.cc)
add_executable(Timestamp_test ${SRC_LIST})
target_link_libraries(Timestamp_test fst_muduo)