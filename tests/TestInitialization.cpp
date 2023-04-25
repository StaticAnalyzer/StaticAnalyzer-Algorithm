#include "doctest.h"

#include "myanalysis/AnalysisFactory.h"

TEST_SUITE_BEGIN("testInitialization");

TEST_CASE("testAlwaysFailedTest") {
    my_analysis::AnalysisFactory factory("resources/example01/src",
                                         "resources/example01/include");
    std::unique_ptr<my_analysis::Analysis> demo = factory.createAlwaysFailedAnalysis();
    demo->analyze();
    const my_analysis::AnalysisResult& result = demo->getResult();
    CHECK_EQ(result.getCode(), 1);
    CHECK_EQ(result.getAnalysisType(), "Always Failed Analysis");
    CHECK_EQ(result.getMsg(), "always failed analysis used for test");
}

TEST_SUITE_END();

