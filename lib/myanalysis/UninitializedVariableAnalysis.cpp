#include "myanalysis/Analysis.h"

#include "World.h"
#include "analysis/dataflow/ReachingDefinition.h"

#include <iostream>

namespace al = analyzer;
namespace air = al::ir;
namespace cf = al::config;
namespace df = al::analysis::dataflow;
namespace dfact = al::analysis::dataflow::fact;

namespace my_analysis {

    UninitializedVariableAnalysis::UninitializedVariableAnalysis() = default;

    void UninitializedVariableAnalysis::analyze()
    {
        initializeSuccessfulResult("Uninitialized Variable Analysis");

        const al::World& world = al::World::get();

        std::unique_ptr<cf::AnalysisConfig> analysisConfig
            = std::make_unique<cf::DefaultAnalysisConfig>("reaching definition analysis");
        std::unique_ptr<df::ReachingDefinition> rd =
            std::make_unique<df::ReachingDefinition>(analysisConfig);

        for(const auto &iter : world.getAllMethods())
        {
            std::shared_ptr<lang::CPPMethod> method = iter.second;
            std::shared_ptr<air::IR> ir = method->getIR();
            std::vector<std::shared_ptr<air::Var>> paramsVec = ir->getParams();
            std::unordered_set<std::shared_ptr<air::Var>> params(paramsVec.begin(), paramsVec.end());

            std::shared_ptr<dfact::DataflowResult<dfact::SetFact<air::Stmt>>> result = rd->analyze(ir);
            std::vector<std::shared_ptr<air::Stmt>> stmts = ir->getStmts();
            for(const auto &stmt : stmts)
            {
                // continue if return stmt
                const clang::Stmt *clangStmt = stmt->getClangStmt();
                if(!clangStmt || clangStmt->getStmtClass() == clang::Stmt::ReturnStmtClass)
                    continue;

                std::shared_ptr<dfact::SetFact<air::Stmt>> in = result->getInFact(stmt);

                std::unordered_set<std::shared_ptr<air::Var>> inFactDefs;
                in->forEach([&inFactDefs](const std::shared_ptr<air::Stmt> &stmt)
                {
                    std::unordered_set<std::shared_ptr<air::Var>> defVars = stmt->getDefs();
                    inFactDefs.insert(defVars.begin(), defVars.end());
                });
            
                std::unordered_set<std::shared_ptr<air::Var>> stmtUses = stmt->getUses();
                std::unordered_set<std::shared_ptr<air::Var>> unitVars;
                for(const auto &use : stmtUses)
                {
                    if(params.find(use) == params.end() &&
                            inFactDefs.find(use) == inFactDefs.end())
                    {
                        unitVars.insert(use);
                    }
                }

                if(!unitVars.empty()) {
                    std::string msg = "Uninitialized variables: ";
                    bool first = true;
                    for(const auto &var : unitVars)
                    {
                        if(!first)
                            msg += ", ";
                        msg += var->getName();
                        first = false;
                    }
                    addFileResultEntry(method->getContainingFilePath(),
                        stmt->getStartLine(), stmt->getStartColumn(), stmt->getEndLine(), stmt->getEndColumn(),
                        AnalysisResult::Severity::Warning, msg);
                }
            }
        }

    }

}
