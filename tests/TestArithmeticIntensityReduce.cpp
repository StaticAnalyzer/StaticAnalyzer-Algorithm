#include "doctest.h"

#include "myanalysis/AnalysisResult.h"
#include "myanalysis/AnalysisFactory.h"

#include <set>

using mar = my_analysis::AnalysisResult;

TEST_SUITE_BEGIN("testArithmeticIntensityReduce");

TEST_CASE("testArithmeticIntensityReduce") {
    my_analysis::AnalysisFactory factory("resources/ArithmeticIntensityReduce");
    std::unique_ptr<my_analysis::Analysis> analysis = factory.createArithmeticIntensityAnalysis();
    analysis->analyze();
    const my_analysis::AnalysisResult& result = analysis->getResult();
    CHECK_EQ(result.getCode(), 0);
    CHECK_EQ(result.getAnalysisType(), "Arithmetic Intensity Reduce Analysis");

    CHECK_EQ(result.getFileAnalyseResults().size(), 1);
    std::vector<mar::ResultUnit> fileResult = result.getFileAnalyseResults().at(
        "resources/ArithmeticIntensityReduce/main.cpp");
    
    CHECK_EQ(fileResult.size(), 5);
}

TEST_SUITE_END();

