#include "World.h"

#include "myanalysis/AnalysisFactory.h"

namespace al = analyzer;

namespace my_analysis {

    AnalysisFactory::AnalysisFactory(
        const std::string& sourceDir, const std::string& includeDir, const std::string& std)
    {
        al::World::initialize(sourceDir, includeDir, std);
    }

    std::unique_ptr<Analysis> AnalysisFactory::createAlwaysFailedAnalysis() const
    {
        return std::make_unique<AlwaysFailedAnalysis>();
    }

    std::unique_ptr<Analysis> AnalysisFactory::createUninitializedVariableAnalysis() const
    {
        return std::make_unique<UninitializedVariableAnalysis>();
    }

}
