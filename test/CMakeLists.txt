
set(SRC_TIMESTAMP_TEST Timestamp_test.cc )
set(SRC_INETADDRESS_TEST InetAddress_test.cc)
set(SRC_THREAD_TEST Thread_test.cc)

add_executable(Timestamp_test ${SRC_TIMESTAMP_TEST})
add_executable(InetAddress_test ${SRC_INETADDRESS_TEST})
add_executable(Thread_test ${SRC_THREAD_TEST})

target_link_libraries(Timestamp_test fst_muduo)
target_link_libraries(InetAddress_test fst_muduo)
target_link_libraries(Thread_test fst_muduo)
