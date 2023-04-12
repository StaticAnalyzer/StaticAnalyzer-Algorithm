add_executable(
  uninit_var_test
  uninit_var_test.cpp
)

target_link_libraries(uninit_var_test
  GTest::gtest_main
  analysis
  framework
  ${CLANG_LIBS}
  ${LLVM_LIBS}
  ${OTHER_LIBS}
)

include(GoogleTest)
gtest_discover_tests(uninit_var_test)
