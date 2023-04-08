#include <llvm-c/Target.h>

#include "analysis/AnalysisFactory.h"

analysis::AnalysisFactory::AnalysisFactory(std::string astListFilePath, std::string configFilePath)
    :configure(configFilePath)
{
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmParser();

    ASTs = initialize(astListFilePath);
    resource = new ASTResource();
    manager = new ASTManager(ASTs, *resource, configure);
    callGraph = new CallGraph(*manager, *resource);
}

Config &analysis::AnalysisFactory::getConfigure()
{
    return configure;
}

std::unique_ptr<analysis::Analysis> analysis::AnalysisFactory::createEchoAnalysis()
{
    return std::make_unique<EchoAnalysis>(*resource, *manager, *callGraph, configure);
}

analysis::AnalysisFactory::~AnalysisFactory()
{
    delete resource;
    delete manager;
    delete callGraph;
}
