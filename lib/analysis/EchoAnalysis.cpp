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
    llvm::raw_string_ostream buffer(str);

    buffer << "----------------------------------------------------------\n";
    buffer << "CFG of all functions:\n";
    buffer << "----------------------------------------------------------\n";

    for (ASTFunction* fun : resource.getFunctions()) {
        const FunctionDecl* funDecl = manager.getFunctionDecl(fun);
        buffer << "CFG of function " << funDecl->getQualifiedNameAsString() 
            << ":\n---------------------\n";
        std::unique_ptr<clang::CFG>& cfg = manager.getCFG(fun);
        LangOptions LO;
        LO.CPlusPlus = true;
        cfg->print(buffer, LO, false);
    }
    buffer << "----------------------------------------------------------\n";
    buffer << "Call Graph:\n";
    buffer << "----------------------------------------------------------\n";
    callGraph.printCallGraph(buffer);
    buffer << "----------------------------------------------------------\n";
    buffer.flush();
    initializeFailedResult("EchoAnalysis", str);

}

analysis::EchoAnalysis::~EchoAnalysis() {
    
}

