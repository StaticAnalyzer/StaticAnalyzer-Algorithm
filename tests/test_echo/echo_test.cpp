#include <gtest/gtest.h>
#include <iostream>

#include "analysis/AnalysisFactory.h"

TEST(EchoTest, Test1) {
    analysis::AnalysisFactory analysisFactory(
        "/home/ubuntu/StaticAnalyzer-Algorithm/tests/test_echo/astlist.txt",
        "/home/ubuntu/StaticAnalyzer-Algorithm/tests/test_echo/config.txt");
    std::unique_ptr<analysis::Analysis> echo = analysisFactory.createEchoAnalysis();
    echo->analyze();
    const analysis::AnalysisResult& result = echo->getResult();
    std::cout << result.getMsg() << std::endl;
    // EXPECT_STREQ(result.dump().c_str(), R"({"analyseType":"EchoAnalysis","code":1,"fileAnalyseResults":null,"msg":"----------------------------------------------------------\nCFG of all functions:\n----------------------------------------------------------\nCFG of function fib:\n---------------------\n\n [B6 (ENTRY)]\n   Succs (1): B5\n\n [B1]\n   1: return i;\n   Preds (1): B4\n   Succs (1): B0\n\n [B2]\n   Preds (1): B3\n   Succs (1): B4\n\n [B3]\n   1: int tmp = i;\n   2: i = j\n   3: j = i + tmp\n   4: n--\n   Preds (1): B4\n   Succs (1): B2\n\n [B4]\n   1: n > 0\n   T: while [B4.1]\n   Preds (2): B2 B5\n   Succs (2): B3 B1\n\n [B5]\n   1: int i = 0;\n   2: int j = 1;\n   Preds (1): B6\n   Succs (1): B4\n\n [B0 (EXIT)]\n   Preds (1): B1\n\nCFG of function main:\n---------------------\n\n [B6 (ENTRY)]\n   Succs (1): B5\n\n [B1]\n   1: return 0;\n   Preds (1): B4\n   Succs (1): B0\n\n [B2]\n   1: i++\n   Preds (1): B3\n   Succs (1): B4\n\n [B3]\n   1: fib(i)\n   2: x = [B3.1]\n   Preds (1): B4\n   Succs (1): B2\n\n [B4]\n   1: i < 10\n   T: for (...; [B4.1]; ...)\n   Preds (2): B2 B5\n   Succs (2): B3 B1\n\n [B5]\n   1: int x;\n   2: int i = 0;\n   Preds (1): B6\n   Succs (1): B4\n\n [B0 (EXIT)]\n   Preds (1): B1\n\n"})");
    // std::cout << std::setw(4) << result << std::endl;
}
