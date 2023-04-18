#include <list>

#include "analysis/Analysis.h"

namespace analysis {

    Analysis::Analysis(ASTResource& resource, ASTManager& manager, CallGraph& callGraph, Config& configure)
        :resource(resource), manager(manager), callGraph(callGraph), configure(configure)
    {

    }

    const AnalysisResult& Analysis::getResult()
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

