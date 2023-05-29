#include "doctest.h"

#include "myanalysis/AnalysisResult.h"
#include "myanalysis/AnalysisFactory.h"

#include <set>

using mar = my_analysis::AnalysisResult;

TEST_SUITE_BEGIN("testDeadCodeElimination");

TEST_CASE("testDeadCodeElimination") {
    my_analysis::AnalysisFactory factory("resources/DeadCodeElimination");
    std::unique_ptr<my_analysis::Analysis> analysis = factory.createDeadCodeEliminationAnalysis();
    analysis->analyze();
    const my_analysis::AnalysisResult& result = analysis->getResult();
    CHECK_EQ(result.getCode(), 0);
    CHECK_EQ(result.getAnalysisType(), "Dead Code Elimination Analysis");

    CHECK_EQ(result.getFileAnalyseResults().size(), 1);
    std::vector<mar::ResultUnit> fileResult = result.getFileAnalyseResults().at(
            "resources/DeadCodeElimination/main.cpp");

    CHECK_EQ(fileResult.size(), 2);
}

TEST_SUITE_END();

