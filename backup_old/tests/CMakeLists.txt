enable_testing()

add_executable(unit_tests
  test_message.cpp
  test_vectorclock.cpp
  ../src/Message.cpp ../src/Protocol.cpp ../src/VectorClock.cpp
)
target_link_libraries(unit_tests PRIVATE Qt6::Core Qt6::Test)
add_test(NAME unit_tests COMMAND unit_tests)
