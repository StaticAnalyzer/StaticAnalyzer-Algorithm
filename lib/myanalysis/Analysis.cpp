#include <list>

#include "myanalysis/Analysis.h"

namespace my_analysis {

    const AnalysisResult& Analysis::getResult() const
    {
        return result;
    }
    
    void Analysis::initializeFailedResult(const std::string& analyseType, const std::string& msg)
    {
        result.setAnalyseType(analyseType);
        result.setCode(1);
        result.setMessage(msg);
    }

    void Analysis::initializeSuccessfulResult(const std::string& analyseType)
    {
        result.setAnalyseType(analyseType);
        result.setCode(0);
    }

    void Analysis::addFileResultEntry(const std::string& file, 
        int startLine, int startColumn, int endLine, int endColumn, 
        AnalysisResult::Severity severity, const std::string& message)
    {
        result.addFileResultUnit(file, 
            AnalysisResult::ResultUnit(startLine, startColumn, endLine, endColumn,
                severity, message));
    }

} 

