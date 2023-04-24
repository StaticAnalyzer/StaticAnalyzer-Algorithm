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
     */
    class AnalysisFactory final {
    public:

        /**
         * @brief constructor for analysis factory
         * @param sourceDir the directory path of all source files
         * @param includeDir the directory path of all (default: "")
         * @param std language standard, e.g. c++98(default), c++11, c99
         */
        AnalysisFactory(const std::string& sourceDir, const std::string& includeDir="",
                        const std::string& std=std::string("c++98"));

        [[nodiscard]] std::unique_ptr<Analysis> createUninitializedVariableAnalysis() const;

    };

} // my analysis

#endif
