#include "doctest.h"

#include "myanalysis/AnalysisResult.h"
#include "myanalysis/AnalysisFactory.h"

#include <set>

using mar = my_analysis::AnalysisResult;

TEST_SUITE_BEGIN("testNullPointer");

TEST_CASE("testNullPointer") {
    my_analysis::AnalysisFactory factory("resources/NullPointer", "", "c++11");
    std::unique_ptr<my_analysis::Analysis> analysis = factory.createNullPointerDereferenceAnalysis();
    analysis->analyze();
    const my_analysis::AnalysisResult& result = analysis->getResult();
    CHECK_EQ(result.getCode(), 0);
    CHECK_EQ(result.getAnalysisType(), "Null Pointer Dereference Analysis");

    CHECK_EQ(result.getFileAnalyseResults().size(), 1);
    std::vector<mar::ResultUnit> fileResult = result.getFileAnalyseResults().at(
            "resources/NullPointer/main.cpp");

    CHECK_EQ(fileResult.size(), 3);
}

TEST_SUITE_END();

