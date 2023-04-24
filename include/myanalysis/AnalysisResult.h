#ifndef ANALYSIS_RESULT_H
#define ANALYSIS_RESULT_H

#include <unordered_map>
#include <string>
#include <vector>

namespace my_analysis {

    /**
     * @class AnalysisResult
     * @brief a result class
     */
    class AnalysisResult {
    public:

        /**
         * @brief severity level
         */
        enum class Severity {
            Hint, Info, Warning, Error
        };

        /**
         * @class ResultUnit
         * @brief a class holding the unit result
         */
        class ResultUnit {
        public:

            /**
             * @brief get the start line number
             * @return start line number, indexed from 1
             */
            [[nodiscard]] int getStartLine() const;

            /**
             * @brief get the start column number
             * @return start column number, indexed from 1
             */
            [[nodiscard]] int getStartColumn() const;

            /**
             * @brief get the end line number
             * @return end line number, included
             */
            [[nodiscard]] int getEndLine() const;

            /**
             * @brief get the end column number
             * @return the end column number, included
             */
            [[nodiscard]] int getEndColumn() const;

            /**
             * @brief the severity of this result
             * @return Hint, Info, Warning, Error
             */
            [[nodiscard]] Severity getSeverity() const;

            /**
             * @brief get the error message
             * @return message of this error
             */
            [[nodiscard]] const std::string& getMessage() const;

            /**
             * @brief constructor for file result unit
             * @param startLine start line number, indexed from 1
             * @param startColumn start column number, indexed from 1
             * @param endLine end line number, included
             * @param endColumn end column number, included
             * @param severity the severity of this result
             * @param message error message
             */
            ResultUnit(int startLine, int startColumn,
                int endLine, int endColumn,
                Severity severity, const std::string& message);

            ResultUnit(const ResultUnit& resultUnit);

            ResultUnit& operator=(const ResultUnit& resultUnit);

        private:

            int startLine; ///< start line number, indexed from 1

            int startColumn; ///< start column number, indexed from 1

            int endLine; ///< end line number, included

            int endColumn; ///< end column number, included

            Severity severity; ///< the severity of this result

            std::string message; ///< error message

        };

        /**
         * @brief get the analysis type
         * @return the name of analysis
         */
        [[nodiscard]] const std::string& getAnalysisType() const;

        /**
         * @brief get success code
         * @return 0 for success and 1 for failure
         */
        [[nodiscard]] int getCode() const;

        /**
         * @brief get error message
         * @return the error message, empty if not failed
         */
        [[nodiscard]] const std::string& getMsg() const;

        /**
         * @brief get file analysis result
         * @return a map from filename to a vector of unit results
         */
        [[nodiscard]] const std::unordered_map<std::string, std::vector<ResultUnit>>&
            getFileAnalyseResults() const;

        /**
         * @brief set the analysis type
         * @param analyseType a string representing the analysis type
         */
        void setAnalyseType(const std::string& analyseType);

        /**
         * @brief set the status code
         * @param code 0 for success and 1 for failure
         */
        void setCode(int code);

        /**
         * @brief set the error message
         * @param msg a string describing why the error occurs
         */
        void setMessage(const std::string& msg);

        /**
         * @brief add a new file result unit
         * @param file the file name
         * @param resultUnit the result unit
         */
        void addFileResultUnit(const std::string& file, ResultUnit resultUnit);

    private:

        std::string analyseType; ///< a string representing the analysis type

        int code; ///< 0 for success and 1 for failure

        std::string msg; ///< a string describing why the error occurs

        std::unordered_map<std::string, std::vector<ResultUnit>>
            fileAnalyseResults; ///< analysis result units of each file

    };

} // my analysis


#endif
