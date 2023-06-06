#include "doctest.h"

#include "myanalysis/AnalysisResult.h"
#include "myanalysis/AnalysisFactory.h"

#include <set>

using mar = my_analysis::AnalysisResult;

TEST_SUITE_BEGIN("testDivisionByZero");

TEST_CASE("testDivisionByZeroTest") {
    my_analysis::AnalysisFactory factory("resources/DivisionByZero");
    std::unique_ptr<my_analysis::Analysis> analysis = factory.createDivisionByZeroAnalysis();
    analysis->analyze();
    const my_analysis::AnalysisResult& result = analysis->getResult();
    CHECK_EQ(result.getCode(), 0);
    CHECK_EQ(result.getAnalysisType(), "Division By Zero Analysis");

    CHECK_EQ(result.getFileAnalyseResults().size(), 1);
    std::vector<mar::ResultUnit> fileResult = result.getFileAnalyseResults().at(
        "resources/DivisionByZero/main.cpp");

    CHECK_EQ(fileResult.size(), 4);

    std::set<int> resultSet;
    for (const auto& unit : fileResult) {
        resultSet.insert(unit.getStartLine());
    }

    CHECK_EQ(resultSet, std::set<int>{8, 9, 11, 13});

}

TEST_SUITE_END();