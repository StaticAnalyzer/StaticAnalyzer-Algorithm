#include <gtest/gtest.h>
#include <iostream>

#include "analysis/AnalysisFactory.h"

TEST(EchoTest, Test1) {
    analysis::AnalysisFactory analysisFactory("/home/ubuntu/StaticAnalyzer-Algorithm/tests/test_echo/astlist.txt",
        "/home/ubuntu/StaticAnalyzer-Algorithm/tests/test_echo/config.txt");
    std::unique_ptr<analysis::Analysis> echo = analysisFactory.createEchoAnalysis();
    std::cout << echo->analyze() << std::endl;
    // EXPECT_STREQ("test", echo->analyze().c_str());
}
