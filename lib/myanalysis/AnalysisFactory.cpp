#include "World.h"

#include "myanalysis/AnalysisFactory.h"

namespace al = analyzer;

namespace my_analysis {

    AnalysisFactory::AnalysisFactory(
        const std::string& sourceDir, const std::string& includeDir, const std::string& std, const std::vector<std::string>& optArgs)
    {
        al::World::initialize(sourceDir, includeDir, std, optArgs);
    }

    std::unique_ptr<Analysis> AnalysisFactory::createAlwaysFailedAnalysis() const
    {
        return std::make_unique<AlwaysFailedAnalysis>();
    }

    std::unique_ptr<Analysis> AnalysisFactory::createUseBeforeDefAnalysis() const
    {
        return std::make_unique<UseBeforeDefAnalysis>();
    }

    std::unique_ptr<Analysis> AnalysisFactory::createArithmeticIntensityAnalysis() const {
        return std::make_unique<ArithmeticIntensityAnalysis>();
    }

    std::unique_ptr<Analysis> AnalysisFactory::createDeadCodeEliminationAnalysis() const {
        return std::make_unique<DeadCodeEliminationAnalysis>();
    }

    std::unique_ptr<Analysis> AnalysisFactory::createMemoryLeakAnalysis() const {
        return std::make_unique<MemoryLeakAnalysis>();
    }

    std::unique_ptr<Analysis> AnalysisFactory::createNullPointerDereferenceAnalysis() const {
        return std::make_unique<NullPointerDereferenceAnalysis>();
    }

    std::unique_ptr<Analysis> AnalysisFactory::createDivisionByZeroAnalysis() const {
        return std::make_unique<DivisionByZeroAnalysis>();
    }

    std::unique_ptr<Analysis> AnalysisFactory::createArrayIndexOutOfBoundsAnalysis() const {
        return std::make_unique<ArrayIndexOutOfBoundsAnalysis>();
    }

}
