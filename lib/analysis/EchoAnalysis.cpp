#include <sstream>

#include <llvm/Support/raw_ostream.h>

#include "analysis/Analysis.h"

analysis::EchoAnalysis::EchoAnalysis(ASTResource& resource, ASTManager& manager, CallGraph& callGraph, Config& configure)
        :Analysis(resource, manager, callGraph, configure)
{

}

void analysis::EchoAnalysis::analyze() 
{   
    std::string str;
    llvm::raw_string_ostream result(str);

    result << "----------------------------------------------------------\n";
    result << "CFG of all functions:\n";
    result << "----------------------------------------------------------\n";

    for (ASTFunction* fun : resource.getFunctions()) {
        const FunctionDecl* funDecl = manager.getFunctionDecl(fun);
        result << "CFG of function " << funDecl->getQualifiedNameAsString() 
            << ":\n---------------------\n";
        std::unique_ptr<clang::CFG>& cfg = manager.getCFG(fun);
        LangOptions LO;
        LO.CPlusPlus = true;
        cfg->print(result, LO, false);
    }
    result << "----------------------------------------------------------\n";
    result << "Call Graph:\n";
    result << "----------------------------------------------------------\n";
    callGraph.printCallGraph(result);
    result << "----------------------------------------------------------\n";
    
    initializeFailedResult("EchoAnalysis", str);
    //return str;
}

analysis::EchoAnalysis::~EchoAnalysis() {
    
}

