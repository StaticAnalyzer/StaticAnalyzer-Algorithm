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
  analysis::AnalysisResult result = uninit_var->getResult();
  EXPECT_EQ(result.getAnalysisType(), "UninitializedVariableAnalysis");
  EXPECT_EQ(result.getCode(), 0);
  EXPECT_EQ(result.getMsg().size(), 0);

  const std::unordered_map<std::string, 
    std::vector<analysis::AnalysisResult::ResultUnit>>&
    resultUnitsMap = result.getFileAnalyseResults();
  
  EXPECT_EQ(resultUnitsMap.size(), 1);

  const std::vector<analysis::AnalysisResult::ResultUnit>& resultUnits
    = resultUnitsMap.at("/home/ubuntu/StaticAnalyzer-Algorithm/tests/test_uninit_var/example.ast");

  EXPECT_EQ(resultUnits[0].getStartLine(), 7);
  EXPECT_EQ(resultUnits[0].getStartColumn(), 5);
  EXPECT_EQ(resultUnits[0].getEndLine(), 7);
  EXPECT_EQ(resultUnits[0].getEndColumn(), 9);
  EXPECT_EQ(resultUnits[0].getMessage(), "Uninitialized variable");
  EXPECT_EQ(resultUnits[0].getSeverity(), analysis::AnalysisResult::Severity::Warning);

  EXPECT_EQ(resultUnits[1].getStartLine(), 13);
  EXPECT_EQ(resultUnits[1].getStartColumn(), 9);
  EXPECT_EQ(resultUnits[1].getEndLine(), 13);
  EXPECT_EQ(resultUnits[1].getEndColumn(), 13);
  EXPECT_EQ(resultUnits[1].getMessage(), "Uninitialized variable");
  EXPECT_EQ(resultUnits[1].getSeverity(), analysis::AnalysisResult::Severity::Warning);

}
