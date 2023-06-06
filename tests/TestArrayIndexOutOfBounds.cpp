#include "doctest.h"

#include "myanalysis/AnalysisResult.h"
#include "myanalysis/AnalysisFactory.h"

#include <set>

using mar = my_analysis::AnalysisResult;

TEST_SUITE_BEGIN("testArrayIndexOutOfBounds");

TEST_CASE("testArrayIndexOutOfBoundsTest") {
    my_analysis::AnalysisFactory factory("resources/ArrayIndexOutOfBounds");
    std::unique_ptr<my_analysis::Analysis> analysis = factory.createArrayIndexOutOfBoundsAnalysis();
    analysis->analyze();
    const my_analysis::AnalysisResult& result = analysis->getResult();
    CHECK_EQ(result.getCode(), 0);
    CHECK_EQ(result.getAnalysisType(), "Array Index Out Of Bounds Analysis");

    CHECK_EQ(result.getFileAnalyseResults().size(), 1);
    std::vector<mar::ResultUnit> fileResult = result.getFileAnalyseResults().at(
            "resources/ArrayIndexOutOfBounds/main.cpp");

    CHECK_EQ(fileResult.size(), 4);

    std::set<std::pair<int,int>> resultSet;
    for (const auto& unit : fileResult) {
        resultSet.insert(std::make_pair(unit.getStartLine(), unit.getStartColumn()));
    }

    CHECK_EQ(resultSet, std::set<std::pair<int,int>>{{4, 7}, {4, 10}, {5, 14}, {5, 20}});

}

TEST_SUITE_END();
