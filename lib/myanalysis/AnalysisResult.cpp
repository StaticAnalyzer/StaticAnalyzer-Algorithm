#include "myanalysis/AnalysisResult.h"

namespace my_analysis {

    AnalysisResult::ResultUnit::ResultUnit(int startLine, int startColumn, 
        int endLine, int endColumn, Severity severity, const std::string& message)
        :startLine(startLine), startColumn(startColumn), 
            endLine(endLine), endColumn(endColumn), 
            severity(severity), message(message)
    {
        
    }

    AnalysisResult::ResultUnit::ResultUnit(const ResultUnit& resultUnit)
        :startLine(resultUnit.startLine), startColumn(resultUnit.startColumn), 
            endLine(resultUnit.endLine), endColumn(resultUnit.endColumn), 
            severity(resultUnit.severity), message(resultUnit.message)
    {

    }

    AnalysisResult::ResultUnit& AnalysisResult::ResultUnit::operator=(
        const ResultUnit& resultUnit)
    {
        startLine = resultUnit.startLine;
        startColumn = resultUnit.startColumn;
        endLine = resultUnit.endLine;
        endColumn = resultUnit.endColumn; 
        severity = resultUnit.severity;
        message = resultUnit.message;
        return *this;
    }
            
    int AnalysisResult::ResultUnit::getStartLine() const
    {
        return startLine;
    }

    int AnalysisResult::ResultUnit::getStartColumn() const
    {
        return startColumn;
    }

    int AnalysisResult::ResultUnit::getEndLine() const
    {
        return endLine;
    }

    int AnalysisResult::ResultUnit::getEndColumn() const
    {
        return endColumn;
    }

    AnalysisResult::Severity AnalysisResult::ResultUnit::getSeverity() const
    {
        return severity;
    }

    const std::string& AnalysisResult::ResultUnit::getMessage() const
    {
        return message;
    }

    const std::string& AnalysisResult::getAnalysisType() const
    {
        return analyseType;
    }

    int AnalysisResult::getCode() const
    {
        return code;
    }

    const std::string& AnalysisResult::getMsg() const
    {
        return msg;
    }
    
    const std::unordered_map<std::string, std::vector<AnalysisResult::ResultUnit>>&
        AnalysisResult::getFileAnalyseResults() const
    {
        return fileAnalyseResults;
    }

    void AnalysisResult::setAnalyseType(const std::string& analyseType)
    {
        this->analyseType = analyseType;
    }

    void AnalysisResult::setCode(int code)
    {
        this->code = code;
    }

    void AnalysisResult::setMessage(const std::string& msg)
    {
        this->msg = msg;
    }

    void AnalysisResult::addFileResultUnit
        (const std::string& file, AnalysisResult::ResultUnit resultUnit)
    {
        if (fileAnalyseResults.find(file) == fileAnalyseResults.end()) {
            fileAnalyseResults.emplace(file, 
                std::vector<AnalysisResult::ResultUnit>{});
        }
        fileAnalyseResults.at(file).emplace_back(resultUnit);
    }

} // namespace analysis
