#include <gtest/gtest.h>
#include <iostream>

#include "analysis/AnalysisFactory.h"

TEST(UnInitVarTest, Test1) {
  analysis::AnalysisFactory analysisFactory(
      "/root/StaticAnalyzer-WebUI/src/Algorithm/tests/test_uninit_var/"
      "astlist.txt",
      "/root/StaticAnalyzer-WebUI/src/Algorithm/tests/test_uninit_var/"
      "config.txt");
  std::unique_ptr<analysis::Analysis> uninit_var =
      analysisFactory.createUninitializedVariableAnalysis();

  std::string analyze_result_json = uninit_var->analyze();
  std::cout << analyze_result_json << std::endl;
// R"({"code": 0, "msg": "", "analyseResults": [)" +  + R"(]})";

  EXPECT_STREQ(
      R"({file: "/root/StaticAnalyzer-WebUI/src/Algorithm/tests/test_uninit_var/example.ast", analyseResults: [{startLine: 13, startColumn: 9, endLine: 13, endColumn: 13, severity: 'warning', message: 'Uninitialized variable'},{startLine: 7, startColumn: 5, endLine: 7, endColumn: 9, severity: 'warning', message: 'Uninitialized variable'},]},)",
      analyze_result_json.c_str());
}
