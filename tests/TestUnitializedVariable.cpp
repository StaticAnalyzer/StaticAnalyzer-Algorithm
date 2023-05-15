#include "doctest.h"

#include "myanalysis/AnalysisResult.h"
#include "myanalysis/AnalysisFactory.h"

using mar = my_analysis::AnalysisResult;

TEST_SUITE_BEGIN("testUnitializedVariable");

TEST_CASE("testUnitializedVariableTest") {
    my_analysis::AnalysisFactory factory("resources/UnitVar");
    std::unique_ptr<my_analysis::Analysis> analysis = factory.createUninitializedVariableAnalysis();
    analysis->analyze();
    const my_analysis::AnalysisResult& result = analysis->getResult();
    CHECK_EQ(result.getCode(), 0);
    CHECK_EQ(result.getAnalysisType(), "Uninitialized Variable Analysis");

    CHECK_EQ(result.getFileAnalyseResults().size(), 1);
    std::vector<mar::ResultUnit> fileResult = result.getFileAnalyseResults().at(
        "resources/UnitVar/UnitVarTest.cpp");
    CHECK_EQ(fileResult.size(), 2);
    CHECK_EQ(fileResult[0].getStartLine(), 17);
    CHECK_EQ(fileResult[0].getMessage(), "Uninitialized variables: y");
    CHECK_EQ(fileResult[1].getStartLine(), 7);
    CHECK_EQ(fileResult[1].getMessage(), "Uninitialized variables: b, a");
}

TEST_SUITE_END();

