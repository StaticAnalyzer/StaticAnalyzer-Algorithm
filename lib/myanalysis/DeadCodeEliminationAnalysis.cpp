#include "myanalysis/Analysis.h"

#include "World.h"
#include "analysis/dataflow/LiveVariable.h"

namespace al = analyzer;
namespace air = al::ir;
namespace cf = al::config;
namespace df = al::analysis::dataflow;
namespace dfact = df::fact;

namespace my_analysis
{

    DeadCodeEliminationAnalysis::DeadCodeEliminationAnalysis() = default;

    void DeadCodeEliminationAnalysis::analyze()
    {
        initializeSuccessfulResult("Dead Code Elimination Analysis");

        const al::World &world = al::World::get();

        std::unique_ptr<cf::AnalysisConfig> analysisConfig = std::make_unique<cf::DefaultAnalysisConfig>("dead code elimination analysis");
        std::unique_ptr<df::LiveVariable> lvd = std::make_unique<df::LiveVariable>(analysisConfig);

        for (const auto &[_, method] : world.getAllMethods()) {
            std::shared_ptr<air::IR> ir = method->getIR();
            auto result = lvd->analyze(ir);

            for (const auto &stmt : ir->getStmts()) {
                std::shared_ptr<dfact::SetFact<air::Var>> outFact = result->getOutFact(stmt);

                std::unordered_set<std::shared_ptr<air::Var>> deadVars;
                for (const auto &use : stmt->getDefs()) {
                    if (!outFact->contains(use)) {
                        deadVars.insert(use);
                    }
                }

                if (!deadVars.empty()) {
                    std::string msg = "Dead Var: ";
                    bool first = true;
                    for (const auto &var : deadVars) {
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
