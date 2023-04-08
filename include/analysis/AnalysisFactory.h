#ifndef ANALYSIS_FACTORY_H
#define ANALYSIS_FACTORY_H

#include <vector>
#include <string>
#include <memory>

#include "analysis/Analysis.h"

namespace analysis {

/// @brief The factory class to generate diffenent analysis
/// based on the same program.
class AnalysisFactory {
private:
    /// @brief ast file path list
    std::vector<std::string> ASTs;

    /// @brief configuration object
    Config configure;

    ASTResource* resource;
    ASTManager* manager;
    CallGraph* callGraph;

public:
    /// @brief constructor for AnalysisFactory
    /// @param astListFilePath the file path of ast file path list
    /// @param configFilePath the file path of configuration file
    AnalysisFactory(std::string astListFilePath, std::string configFilePath);

    ~AnalysisFactory();

    /// @brief get the configuration object corresponding to the 
    /// given configuration file
    /// @return the reference of Config object
    Config& getConfigure();

    /// @brief echo analysis simply returns the cfg and cg of input program
    /// @return an echo analysis object 
    std::unique_ptr<Analysis> createEchoAnalysis();
};

}

#endif
