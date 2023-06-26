#include "myanalysis/Analysis.h"

#include "World.h"
#include "myanalysis/dataflow/NullPointer.h"
#include "clang/AST/StmtVisitor.h"
#include "analysis/graph/CFG.h"

namespace al = analyzer;
namespace air = al::ir;
namespace cf = al::config;
namespace df = al::analysis::dataflow;
namespace dfact = df::fact;

namespace my_analysis
{

    NullPointerDereferenceAnalysis::NullPointerDereferenceAnalysis() = default;

    void NullPointerDereferenceAnalysis::analyze()
    {
        class PointerAssignVisitor: public clang::StmtVisitor<PointerAssignVisitor, void> {
        public:
            void VisitUnaryDeref(clang::UnaryOperator* stmt) {
                clang::Expr* expr = stmt->getSubExpr()->IgnoreParenCasts();
                if (auto declRefExpr = clang::dyn_cast<clang::DeclRefExpr>(expr)) {
                    const clang::VarDecl* varDecl = clang::dyn_cast<clang::VarDecl>(declRefExpr->getDecl());
                    if (varDecl) {
                        if (nullPtrFacts.find(varDecl) != nullPtrFacts.end()) {
                            nullDerefVarDecls.insert(varDecl);
                        }
                    }
                }
            }

            void VisitArraySubscriptExpr(clang::ArraySubscriptExpr* stmt) {
                clang::Expr* expr = stmt->getBase()->IgnoreParenCasts();
                if (auto declRefExpr = clang::dyn_cast<clang::DeclRefExpr>(expr)) {
                    const clang::VarDecl* varDecl = clang::dyn_cast<clang::VarDecl>(declRefExpr->getDecl());
                    if (varDecl) {
                        if (nullPtrFacts.find(varDecl) != nullPtrFacts.end()) {
                            nullDerefVarDecls.insert(varDecl);
                        }
                    }
                }
            }

            void VisitMemberExpr(clang::MemberExpr *E) {
                if (E->isArrow()) {
                    clang::Expr* expr = E->getBase()->IgnoreParenCasts();
                    if (auto declRefExpr = clang::dyn_cast<clang::DeclRefExpr>(expr)) {
                        const clang::VarDecl* varDecl = clang::dyn_cast<clang::VarDecl>(declRefExpr->getDecl());
                        if (varDecl) {
                            if (nullPtrFacts.find(varDecl) != nullPtrFacts.end()) {
                                nullDerefVarDecls.insert(varDecl);
                            }
                        }
                    }
                }
            }

            void VisitBinaryOperator(clang::BinaryOperator* binOp) {
                Visit(binOp->getLHS());
                Visit(binOp->getRHS());
            }

            void visitPointerUse(clang::Stmt* stmt, const std::shared_ptr<dfact::SetFact<analyzer::ir::Var>>& pInFact) {
                pInFact->forEach([&](auto var) {
                    nullPtrFacts.insert(var->getClangVarDecl());
                });
                if (stmt)
                    Visit(stmt);
            }

            std::unordered_set<const clang::VarDecl*> getNullDerefVarDecls() {
                return nullDerefVarDecls;
            }


        private:
            std::unordered_set<const clang::VarDecl*> nullDerefVarDecls;
            std::unordered_set<const clang::VarDecl*> nullPtrFacts;

        };
        initializeSuccessfulResult("Null Pointer Dereference Analysis");

        const al::World &world = al::World::get();

        std::unique_ptr<cf::AnalysisConfig> analysisConfig = std::make_unique<cf::DefaultAnalysisConfig>("null pointer dereference analysis");
        std::unique_ptr<dataflow::NullPointer> nullPointerDriver =
                std::make_unique<dataflow::NullPointer>(analysisConfig);

        for (const auto &[_, method] : world.getAllMethods()) {
            std::shared_ptr<air::IR> ir = method->getIR();
            std::shared_ptr<dfact::DataflowResult<dfact::SetFact<air::Var>>>
                result = nullPointerDriver->analyze(ir);

            for (const auto &stmt : ir->getStmts()) {
                auto clangStmt = stmt->getClangStmt();
                PointerAssignVisitor pointerAssignVisitor;
                auto inFact = result->getInFact(stmt);
                pointerAssignVisitor.visitPointerUse(const_cast<clang::Stmt*>(clangStmt), inFact);
                auto nullDerefVarDecls = pointerAssignVisitor.getNullDerefVarDecls();

                if (!nullDerefVarDecls.empty()) {
                    std::string msg = "Null pointer dereference: ";
                    bool first = true;
                    for (const auto &varDecl : nullDerefVarDecls) {
                        if (!first)
                            msg += ", ";
                        msg += varDecl->getName();
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
