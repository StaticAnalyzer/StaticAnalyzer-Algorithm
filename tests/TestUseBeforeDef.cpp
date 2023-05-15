#include "doctest.h"

#include "myanalysis/AnalysisResult.h"
#include "myanalysis/AnalysisFactory.h"

using mar = my_analysis::AnalysisResult;

TEST_SUITE_BEGIN("testUseBeforeDef");

TEST_CASE("testUseBeforeDefTest") {
    my_analysis::AnalysisFactory factory("resources/UseBeforeDef");
    std::unique_ptr<my_analysis::Analysis> analysis = factory.createUseBeforeDefAnalysis();
    analysis->analyze();
    const my_analysis::AnalysisResult& result = analysis->getResult();
    CHECK_EQ(result.getCode(), 0);
    CHECK_EQ(result.getAnalysisType(), "Use Before Define Analysis");

    CHECK_EQ(result.getFileAnalyseResults().size(), 1);
    std::vector<mar::ResultUnit> fileResult = result.getFileAnalyseResults().at(
        "resources/UseBeforeDef/UseBeforeDefTest.cpp");
    CHECK_EQ(fileResult.size(), 3);
    CHECK_EQ(fileResult[0].getStartLine(), 29);
    CHECK_EQ(fileResult[0].getMessage(), "Uninitialized variables: c");
    CHECK_EQ(fileResult[1].getStartLine(), 17);
    CHECK_EQ(fileResult[1].getMessage(), "Uninitialized variables: y");
    CHECK_EQ(fileResult[2].getStartLine(), 7);
    CHECK_EQ(fileResult[2].getMessage(), "Uninitialized variables: b, a");
}

TEST_SUITE_END();

