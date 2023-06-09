#ifndef ANALYSIS_FACTORY_H
#define ANALYSIS_FACTORY_H

#include <vector>
#include <string>
#include <memory>

#include "myanalysis/Analysis.h"

namespace my_analysis {

    /**
     * @brief The factory class to generate different analysis
     * based on the same program.
     * !!! there should be only one factory in the memory at the same time.
     */
    class AnalysisFactory final {
    public:

        /**
         * @brief constructor for analysis factory
         * @param sourceDir the directory path of all source files
         * @param includeDir the directory path of all (default: "")
         * @param std language standard, e.g. c++98(default), c++11, c99
         */
        explicit AnalysisFactory(const std::string& sourceDir, const std::string& includeDir="",
                        const std::string& std=std::string("c++98"), const std::vector<std::string>& optArgs={});

        /**
         * @brief for temporary
         * @return a always failed analysis used for test
         */
        [[nodiscard]] std::unique_ptr<Analysis> createAlwaysFailedAnalysis() const;

        /**
         * @brief create use before define analysis
         * @return a use before define analysis
         */
        [[nodiscard]] std::unique_ptr<Analysis> createUseBeforeDefAnalysis() const;

        /**
         * @brief create arithmetic intensity reduce analysis
         * @return a arithmetic intensity reduce analysis
         */
        [[nodiscard]] std::unique_ptr<Analysis> createArithmeticIntensityAnalysis() const;

        /**
         * @brief create dead code elimination analysis
         * @return a dead code elimination analysis
         */
        [[nodiscard]] std::unique_ptr<Analysis> createDeadCodeEliminationAnalysis() const;

        /**
         * @brief create memory leak analysis
         * @return a memory leak analysis
         */
        [[nodiscard]] std::unique_ptr<Analysis> createMemoryLeakAnalysis() const;

        /**
         * @brief create null pointer dereference analysis
         * @return a null pointer dereference analysis
         */
        [[nodiscard]] std::unique_ptr<Analysis> createNullPointerDereferenceAnalysis() const;

        /**
         * @brief create array index out of bound analysis
         * @return a array index out of bound analysis
         */
        [[nodiscard]] std::unique_ptr<Analysis> createArrayIndexOutOfBoundsAnalysis() const;

        /**
         * @brief create divide by zero analysis
         * @return a divide by zero analysis
         */
        [[nodiscard]] std::unique_ptr<Analysis> createDivisionByZeroAnalysis() const;
    };

} // my analysis

#endif
