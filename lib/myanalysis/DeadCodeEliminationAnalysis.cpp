#include "myanalysis/Analysis.h"

#include "World.h"
#include "analysis/dataflow/LiveVariable.h"
#include <map>

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
        class Util {
        public:
            static bool isStructOrPointerOrArray(const clang::VarDecl* varDecl) {
                const clang::QualType type = varDecl->getType();
                const clang::Type* canonicalType = type.getTypePtr()->getCanonicalTypeInternal().getTypePtrOrNull();

                if (canonicalType) {
                    if (canonicalType->isStructureType()) {
                        return true;
                    } else if (canonicalType->isPointerType()) {
                        return true;
                    } else if (canonicalType->isArrayType()) {
                        return true;
                    }
                }

                return false;
            }
        };
        initializeSuccessfulResult("Dead Code Elimination Analysis");

        const al::World &world = al::World::get();

        std::unique_ptr<cf::AnalysisConfig> analysisConfig = std::make_unique<cf::DefaultAnalysisConfig>("dead code elimination analysis");
        std::unique_ptr<df::LiveVariable> lvd = std::make_unique<df::LiveVariable>(analysisConfig);

        for (const auto &[_, method] : world.getAllMethods()) {
            std::shared_ptr<air::IR> ir = method->getIR();
            auto result = lvd->analyze(ir);

            std::map<int, std::vector<std::tuple<std::shared_ptr<air::Stmt>, std::vector<std::shared_ptr<air::Var>>>>> deadVars;    // line -> [(stmt, [var])]

            for (const auto &stmt : ir->getStmts()) {
                std::shared_ptr<dfact::SetFact<air::Var>> outFact = result->getOutFact(stmt);

                std::vector<std::shared_ptr<air::Var>> deadVarArr;
                for (const auto &def : stmt->getDefs()) {
                    if (!outFact->contains(def)) {
                        deadVarArr.push_back(def);
                    }
                }
                if (!deadVarArr.empty()) {
                    deadVars[stmt->getStartLine()].emplace_back(stmt, deadVarArr);
                }
            }
            for (const auto& [line, deadVarStmtResult] : deadVars) {
                // 本行的每个Stmt都记录的deadVar才会输出
                std::unordered_set<std::shared_ptr<air::Var>> deadVarSet;
                int startColumn = std::numeric_limits<int>::max();
                int endColumn = std::numeric_limits<int>::min();
                for (const auto& [stmt, deadVarArr] : deadVarStmtResult) {
                    startColumn = std::min(startColumn, stmt->getStartColumn());
                    endColumn = std::max(endColumn, stmt->getEndColumn());
                    for (const auto& var : deadVarArr) {
                        deadVarSet.insert(var);
                    }
                }

                std::unordered_set<std::shared_ptr<air::Var>> deadVarResult;
                for (const auto& var : deadVarSet) {
                    if (!var->getClangVarDecl()->isLocalVarDecl()) continue;    // 全局变量不考虑
                    if (Util::isStructOrPointerOrArray(var->getClangVarDecl())) continue;    // 结构体和指针不考虑
                    for (const auto& [stmt, deadVarArr] : deadVarStmtResult) {
                        if (std::find(deadVarArr.begin(), deadVarArr.end(), var) != deadVarArr.end()) {
                            deadVarResult.insert(var);
                        }
                    }
                }

                if (!deadVarResult.empty()) {
                    std::string msg = "Dead Var: ";
                    bool first = true;
                    for (const auto& var : deadVarResult) {
                        if (!first)
                            msg += ", ";
                        msg += var->getName();
                        first = false;
                    }
                    addFileResultEntry(method->getContainingFilePath(),
                                       line, startColumn, line, endColumn,
                                       AnalysisResult::Severity::Warning, msg);
                }
            }

        }
    }

}
