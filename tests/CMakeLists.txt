enable_testing()

add_executable(unit_message
  test_message.cpp
  ../src/Message.cpp ../src/Protocol.cpp
)
target_link_libraries(unit_message PRIVATE Qt6::Core Qt6::Test)
add_test(NAME unit_message COMMAND unit_message)

add_executable(unit_vc
  test_vectorclock.cpp
)
target_link_libraries(unit_vc PRIVATE Qt6::Core Qt6::Test)
add_test(NAME unit_vc COMMAND unit_vc)
