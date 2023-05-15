#include "doctest.h"

#include "myanalysis/AnalysisResult.h"
#include "myanalysis/AnalysisFactory.h"

#include <set>

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
    
    CHECK_EQ(fileResult.size(), 5);
    std::set<std::pair<int,std::string>> resultSet;
    for (const auto& unit : fileResult) {
        resultSet.insert(std::make_pair(unit.getStartLine(), unit.getMessage()));
    }

    CHECK_EQ(resultSet, std::set<std::pair<int,std::string>>{
        {7,  "Uninitialized variables: b, a"},
        {17, "Uninitialized variables: y"},
        {29, "Uninitialized variables: c"},
        {60, "Uninitialized variables: a"},
        {61, "Uninitialized variables: a"}});
    
}

TEST_SUITE_END();

