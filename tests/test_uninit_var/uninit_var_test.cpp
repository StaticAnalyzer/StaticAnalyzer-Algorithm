#include <gtest/gtest.h>
#include <iostream>

#include "analysis/AnalysisFactory.h"

TEST(UnInitVarTest, Test1) {
  analysis::AnalysisFactory analysisFactory(
      "/home/ubuntu/StaticAnalyzer-Algorithm/tests/test_uninit_var/"
      "astlist.txt",
      "/home/ubuntu/StaticAnalyzer-Algorithm/tests/test_uninit_var/"
      "config.txt");
  std::unique_ptr<analysis::Analysis> uninit_var =
      analysisFactory.createUninitializedVariableAnalysis();
  uninit_var->analyze();
  json result = uninit_var->getResult();
  EXPECT_STREQ(result.dump().c_str(), R"({"analyseType":"UninitializedVariableAnalysis","code":0,"fileAnalyseResults":{"/home/ubuntu/StaticAnalyzer-Algorithm/tests/test_uninit_var/example.ast":[{"endColumn":9,"endLine":7,"message":"Uninitialized variable","severity":"Warning","startColumn":5,"startLine":7},{"endColumn":13,"endLine":13,"message":"Uninitialized variable","severity":"Warning","startColumn":9,"startLine":13}]},"msg":"success"})");
  std::cout << std::setw(4) << result << std::endl;
}
