#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <string>
#include <unordered_map>

#include "myanalysis/AnalysisResult.h"

namespace my_analysis {

    /**
     * @class Analysis
     * @brief the interface class for all analysis that used by the grpc server
     */
    class Analysis {
    public:

        /**
         * @brief perform the analysis
         */
        virtual void analyze() = 0;

        /**
         * @return the result of this analysis
         */
        [[nodiscard]] const AnalysisResult& getResult() const;

        Analysis() = default;

        virtual ~Analysis() = default;

    protected:

        /**
         * @brief initialize the result as a failed result
         * @param analyseType analysis algorithm type
         * @param msg error message
         */
        void initializeFailedResult(const std::string& analyseType, const std::string& msg);

        /**
         * @brief initialize the result as a successful result
         * @param analyseType analysis algorithm type
         */
        void initializeSuccessfulResult(const std::string& analyseType);

        /**
         * @brief add a file result entry
         * @param file path of the file being analyzed
         * @param startLine error location start line (indexed from 1)
         * @param startColumn error location start column (indexed from 1)
         * @param endLine error location end line (included)
         * @param endColumn error location end column (included)
         * @param severity severity level: Severity::Hint, Severity::Info, Severity::Warning, Severity::Error
         * @param message error message
         */
        void addFileResultEntry(const std::string& file,
            int startLine, int startColumn, int endLine, int endColumn,
            AnalysisResult::Severity severity, const std::string& message);

    private:

        AnalysisResult result; ///< the analysis result used by the grpc server

    };

    /**
     * @class AlwaysFailedAnalysis
     * @brief always failed analysis used for test
     */
    class AlwaysFailedAnalysis: public Analysis {
    public:

        AlwaysFailedAnalysis();

        void analyze() override;

    };

    /**
     * @class UseBeforeDefAnalysis
     * @brief detect use before define variable
     */
    class UseBeforeDefAnalysis: public Analysis {
    public:

        UseBeforeDefAnalysis();

        void analyze() override;

    };

    /**
     * @class ArithmeticIntensityAnalysis
     * @brief reduce arithmetic intensity
     */
    class ArithmeticIntensityAnalysis: public Analysis {
    public:

        ArithmeticIntensityAnalysis();

        void analyze() override;

    };

    /**
     * @class DeadCodeEliminationAnalysis
     * @brief eliminate dead code
     */
    class DeadCodeEliminationAnalysis: public Analysis {
    public:

        DeadCodeEliminationAnalysis();

        void analyze() override;

    };

    /**
     * @class MemoryLeakAnalysis
     * @brief detect memory leak
     */
    class MemoryLeakAnalysis: public Analysis {
    public:

        MemoryLeakAnalysis();

        void analyze() override;

    };

} // my analysis


#endif
