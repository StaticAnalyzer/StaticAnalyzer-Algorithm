#include "myanalysis/Analysis.h"

#include "World.h"
#include "myanalysis/dataflow/UseBeforeDef.h"

namespace al = analyzer;
namespace air = al::ir;
namespace cf = al::config;
namespace df = al::analysis::dataflow;
namespace dfact = df::fact;

namespace my_analysis
{

    UseBeforeDefAnalysis::UseBeforeDefAnalysis() = default;

    void UseBeforeDefAnalysis::analyze()
    {
        initializeSuccessfulResult("Use Before Define Analysis");

        const al::World &world = al::World::get();

        std::unique_ptr<cf::AnalysisConfig> analysisConfig = std::make_unique<cf::DefaultAnalysisConfig>("use bofore define analysis");
        std::unique_ptr<df::UseBeforeDef> ubd = std::make_unique<df::UseBeforeDef>(analysisConfig);

        for (const auto &[_, method] : world.getAllMethods()) {
            std::shared_ptr<air::IR> ir = method->getIR();
            std::shared_ptr<dfact::DataflowResult<dfact::SetFact<air::Var>>>
                result = ubd->analyze(ir);

            for (const auto &stmt : ir->getStmts()) {
                // continue if return stmt
                const clang::Stmt *clangStmt = stmt->getClangStmt();
                if (!clangStmt || clangStmt->getStmtClass() == clang::Stmt::ReturnStmtClass)
                    continue;

                std::shared_ptr<dfact::SetFact<air::Var>> inFact = result->getInFact(stmt);

                std::unordered_set<std::shared_ptr<air::Var>> unitVars;
                for (const auto &use : stmt->getUses()) {
                    if (inFact->contains(use)) {
                        unitVars.insert(use);
                    }
                }

                if (!unitVars.empty()) {
                    std::string msg = "Uninitialized variables: ";
                    bool first = true;
                    for (const auto &var : unitVars) {
                        if (!first)
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
