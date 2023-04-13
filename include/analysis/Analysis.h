#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <string>
#include <unordered_map>

#include "nlohmann/json.h"

#include "framework/ASTManager.h"
#include "framework/CallGraph.h"
#include "framework/Common.h"

using json = nlohmann::json;

namespace analysis {

/// @brief interface class for all analysis
class Analysis 
{
protected:
    enum class Severity {
        Hint, Info, Warning, Error
    };

private:
    json result;
    static const std::unordered_map<Severity, std::string> enumToString;

protected:
    ASTResource& resource;
    ASTManager& manager;
    CallGraph& callGraph;
    Config& configure;

    /// @brief initialize the result as a failed result
    /// @param analyseType analysis algorithm type
    /// @param msg error message
    void initializeFailedResult(const std::string& analyseType, const std::string& msg);

    /// @brief initialze the result as a successful result
    /// @param analyseType analysis algorithm type
    void initializeSuccessfulResult(const std::string& analyseType);

    /// @brief add a file result entry
    /// @param file path of the file being analyzed
    /// @param startLine error location start line (indexed from 1)
    /// @param startColumn error location start column (indexed from 1)
    /// @param endLine error location end line (included)
    /// @param endColumn error location end column (excluded)
    /// @param severity severity level: Severity::Hint, Severity::Info, Severity::Warning, Severity::Error
    /// @param message error message
    void addFileResultEntry(const std::string& file, 
        int startLine, int startColumn, int endLine, int endColumn, 
        Severity severity, const std::string& message);

public:
    Analysis(ASTResource& resource, ASTManager& manager, CallGraph& callGraph, Config& configure);

    ~Analysis() = default;

    /// @brief perform the analysis
    virtual void analyze()=0;
    
    /// @brief get the result of analysis
    /// @return a json object
    const json& getResult();
};

/// @brief echo analysis simply returns the cfg and cg of input program
class EchoAnalysis: public Analysis
{

public:
    EchoAnalysis(ASTResource& resource, ASTManager& manager, CallGraph& callGraph, Config& configure);
    ~EchoAnalysis();

    virtual void analyze();
};

/// @brief uninitialized variable analysis detects uninitialized variables
class UninitializedVariableAnalysis: public Analysis
{

public:
    UninitializedVariableAnalysis(ASTResource& resource, ASTManager& manager, CallGraph& callGraph, Config& configure);
    ~UninitializedVariableAnalysis();

    virtual void analyze();
};

}


#endif
