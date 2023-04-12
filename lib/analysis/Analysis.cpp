#include <list>

#include "analysis/Analysis.h"

namespace analysis {

    Analysis::Analysis(ASTResource& resource, ASTManager& manager, CallGraph& callGraph, Config& configure)
        :resource(resource), manager(manager), callGraph(callGraph), configure(configure)
    {

    }

    const json& Analysis::getResult()
    {
        return result;
    }
    
    void Analysis::initializeFailedResult(const std::string& analyseType, const std::string& msg)
    {
        result = {
            {"analyseType", analyseType},
            {"code", 1},
            {"msg", msg},
            {"fileAnalyseResults", nullptr}
        };
    }

    void Analysis::initializeSuccessfulResult(const std::string& analyseType)
    {
        result = {
            {"analyseType", analyseType},
            {"code", 0},
            {"msg", "success"},
            {"fileAnalyseResults", json()}
        };
    }

    void Analysis::addFileResultEntry(const std::string& file, 
        int startLine, int startColumn, int endLine, int endColumn, 
        const std::string& severity, const std::string& message)
    {
        if (!result["fileAnalyseResults"].contains(file)) {
            result["fileAnalyseResults"][file] = std::list<json>();
        }
        
        result["fileAnalyseResults"][file].emplace_back(json{
            {"startLine", startLine},
            {"startColumn", startColumn},
            {"endLine", endLine},
            {"endColumn", endColumn},
            {"severity", severity},
            {"message", message}
        });
    }

} 

