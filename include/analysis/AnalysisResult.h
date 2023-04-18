#ifndef ANALYSIS_RESULT_H
#define ANALYSIS_RESULT_H

#include <unordered_map>
#include <string>

namespace analysis {

/// @brief a result class
class AnalysisResult 
{
public:

    /// @brief severity level
    enum class Severity {
        Hint, Info, Warning, Error
    };

    // /// @brief a map from severity to string
    // static const std::unordered_map<Severity, std::string> enumToString;

    /// @brief a class holding the unit result
    class ResultUnit {
        public:

            /// @brief get the start line number
            /// @return start line number, indexed from 1
            int getStartLine() const;

            /// @brief get the start column number
            /// @return start column number, indexed from 1
            int getStartColumn() const;

            /// @brief get the end line number
            /// @return end line number, included
            int getEndLine() const;

            /// @brief get the end column number
            /// @return the end column number, excluded
            int getEndColumn() const;

            /// @brief the severity of this result
            /// @return Hint, Info, Warning, Error
            Severity getSeverity() const;

            ResultUnit(int startLine, int startColumn, 
                int endLine, int endColumn,
                Severity severity, const std::string& message);

            ResultUnit(const ResultUnit& resultUnit);

            ResultUnit& operator=(const ResultUnit& resultUnit);

        private:
            int startLine;
            int startColumn;
            int endLine;
            int endColumn;
            Severity severity;
            std::string message;
    };

    /// @brief get the analysis type
    /// @return the name of analysis
    const std::string& getAnalysisType() const;

    /// @brief get success code
    /// @return 0 for success and 1 for failed
    int getCode() const;

    /// @brief get error message
    /// @return the error message, empty if not failed
    const std::string& getMsg() const;
    
    /// @brief get file analysis result
    /// @return a map from filename to a vector of unit results
    const std::unordered_map<std::string, std::vector<ResultUnit>>&
        getFileAnalyseResults() const;

    void setAnalyseType(const std::string& analyseType);

    void setCode(int code);

    void setMessage(const std::string& msg);

    void addFileResultUnit(const std::string& file, ResultUnit resultUnit);

private:

    std::string analyseType;
    
    int code;
    
    std::string msg;
    
    std::unordered_map<std::string, std::vector<ResultUnit>> fileAnalyseResults;

};

}


#endif
