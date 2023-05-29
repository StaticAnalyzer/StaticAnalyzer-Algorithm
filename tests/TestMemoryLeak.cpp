#include "doctest.h"

#include "myanalysis/AnalysisResult.h"
#include "myanalysis/AnalysisFactory.h"

#include <set>

using mar = my_analysis::AnalysisResult;

TEST_SUITE_BEGIN("testMemoryLeak");

TEST_CASE("testMemoryLeak") {
    my_analysis::AnalysisFactory factory("resources/MemoryLeak");
    std::unique_ptr<my_analysis::Analysis> analysis = factory.createMemoryLeakAnalysis();
    analysis->analyze();
    const my_analysis::AnalysisResult& result = analysis->getResult();
    CHECK_EQ(result.getCode(), 0);
    CHECK_EQ(result.getAnalysisType(), "Memory Leak Analysis");

    CHECK_EQ(result.getFileAnalyseResults().size(), 1);
    std::vector<mar::ResultUnit> fileResult = result.getFileAnalyseResults().at(
            "resources/MemoryLeak/main.cpp");

    CHECK_EQ(fileResult.size(), 3);
}

TEST_SUITE_END();

