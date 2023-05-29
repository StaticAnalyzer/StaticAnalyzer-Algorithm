#include "myanalysis/Analysis.h"

#include "World.h"
#include "myanalysis/dataflow/MemoryLeak.h"

namespace al = analyzer;
namespace air = al::ir;
namespace cf = al::config;
namespace df = al::analysis::dataflow;
namespace dfact = df::fact;

namespace my_analysis {
    MemoryLeakAnalysis::MemoryLeakAnalysis() = default;

    void MemoryLeakAnalysis::analyze() {
        initializeSuccessfulResult("Memory Leak Analysis");

        const al::World &world = al::World::get();

        std::unique_ptr<cf::AnalysisConfig> analysisConfig = std::make_unique<cf::DefaultAnalysisConfig>(
                "memory leak analysis");
        auto memoryLeakDriver = std::make_unique<df::MemoryLeak>(analysisConfig);

        for (const auto &[_, method]: world.getAllMethods()) {
            std::shared_ptr<air::IR> ir = method->getIR();
            auto memoryLeakResult = memoryLeakDriver->analyze(ir);

            for (const auto &stmt: ir->getStmts()) {
                auto clangStmt = stmt->getClangStmt();

                std::unordered_set<std::shared_ptr<air::Var>> leakVars;
                for (const auto& def : stmt->getDefs()) {
                    // 一个变量保留着地址，又进行了一次定义，之前保存地址可能泄露
                    if (memoryLeakResult->getInFact(stmt)->contains(def)) {
                        leakVars.insert(def);
                    }
                }

                if (!leakVars.empty()) {
                    std::string msg = "Potential Memory Leak: ";
                    bool first = true;
                    for (const auto &var : leakVars) {
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

            auto exitStmt = ir->getCFG()->getExit();
            auto exitFact = memoryLeakResult->getInFact(exitStmt);
            std::unordered_set<std::shared_ptr<air::Var>> leakVars;
            exitFact->forEach([&](const auto& var) {
                leakVars.insert(var);
            });

            if (!leakVars.empty()) {
                std::string msg = "Potential Memory Leak: ";
                bool first = true;
                for (const auto &var : leakVars) {
                    if (!first)
                        msg += ", ";
                    msg += var->getName();
                    first = false;
                }
                const auto& sourceManager = method->getASTUnit()->getASTContext().getSourceManager();
                auto beginLoc = method->getFunctionDecl()->getBeginLoc();
                auto endLoc = method->getFunctionDecl()->getEndLoc();
                auto startLine = static_cast<int>(sourceManager.getPresumedLineNumber(
                        sourceManager.getExpansionLoc(beginLoc)));
                auto endLine = startLine;
                auto startColumn = static_cast<int>(sourceManager.getPresumedColumnNumber(
                        sourceManager.getExpansionLoc(endLoc)));
                auto endColumn = startColumn + 5;
                addFileResultEntry(method->getContainingFilePath(),
                                   startLine, startColumn, endLine, endColumn,
                                   AnalysisResult::Severity::Warning, msg);
            }
        }
    }

}
