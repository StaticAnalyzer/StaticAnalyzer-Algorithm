#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <string>

#include "framework/ASTManager.h"
#include "framework/CallGraph.h"
#include "framework/Common.h"

namespace analysis {

/// @brief interface class for all analysis
class Analysis 
{
protected:
    ASTResource& resource;
    ASTManager& manager;
    CallGraph& callGraph;
    Config& configure;

public:
    Analysis(ASTResource& resource, ASTManager& manager, CallGraph& callGraph, Config& configure)
        :resource(resource), manager(manager), callGraph(callGraph), configure(configure)
    {

    }

    ~Analysis() { }

    /// @brief perform the analysis
    /// @return the string representing the result of analysis
    /// (usually in the form of json)
    virtual std::string analyze()=0;
};

/// @brief echo analysis simply returns the cfg and cg of input program
class EchoAnalysis: public Analysis
{

public:
    EchoAnalysis(ASTResource& resource, ASTManager& manager, CallGraph& callGraph, Config& configure);
    ~EchoAnalysis();

    virtual std::string analyze();
};

/// @brief uninitialized variable analysis detects uninitialized variables
class UninitializedVariableAnalysis: public Analysis
{

public:
    UninitializedVariableAnalysis(ASTResource& resource, ASTManager& manager, CallGraph& callGraph, Config& configure);
    ~UninitializedVariableAnalysis();

    virtual std::string analyze();
};

}


#endif
