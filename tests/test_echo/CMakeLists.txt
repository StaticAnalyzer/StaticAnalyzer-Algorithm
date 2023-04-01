add_executable(
  echo_test
  echo_test.cpp
)

target_link_libraries(echo_test
  GTest::gtest_main
  analysis
  framework
  ${CLANG_LIBS}
  ${LLVM_LIBS}
  ${OTHER_LIBS}
)

include(GoogleTest)
gtest_discover_tests(echo_test)
