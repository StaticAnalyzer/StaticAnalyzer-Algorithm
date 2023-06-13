#include "myanalysis/dataflow/MemoryLeak.h"

#include "clang/AST/StmtVisitor.h"

namespace my_analysis::dataflow {

    MemoryLeak::MemoryLeak(std::unique_ptr<config::AnalysisConfig>& analysisConfig)
            :AnalysisDriver<fact::SetFact<ir::Var>>(analysisConfig)
    {

    }

    std::unique_ptr<libdataflow::DataflowAnalysis<fact::SetFact<ir::Var>>>
        MemoryLeak::makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const
    {
        class NewDeleteVisitor: public clang::StmtVisitor<NewDeleteVisitor, void> {
        public:
            void VisitBinAssign(clang::BinaryOperator* binAssign)
            {
                clang::Expr* rhsExpr = binAssign->getRHS()->IgnoreParenCasts();
                if (clang::dyn_cast<clang::CXXNewExpr>(rhsExpr)) {
                    if (auto* varDeclExpr = clang::dyn_cast<clang::DeclRefExpr>(binAssign->getLHS()->IgnoreParenCasts())) {
                        if (auto* varDecl = clang::dyn_cast<clang::VarDecl>(varDeclExpr->getDecl()))
                            newVarDecl = varDecl;
                    }
                }
                else if(auto* callExpr = clang::dyn_cast<clang::CallExpr>(rhsExpr)) {
                    if (auto* varDeclExpr = clang::dyn_cast<clang::DeclRefExpr>(binAssign->getLHS()->IgnoreParenCasts())) {
                        if (auto* varDecl = clang::dyn_cast<clang::VarDecl>(varDeclExpr->getDecl()))
                            newVarDecl = varDecl;
                    }
                }
            }

            void VisitDeclStmt(clang::DeclStmt* declStmt)
            {
                for (clang::Decl* decl: declStmt->decls()) {
                    if (auto* varDecl = clang::dyn_cast<clang::VarDecl>(decl)) {
                        if (clang::Expr* initExpr = varDecl->getInit()) {
                            if (clang::dyn_cast<clang::CXXNewExpr>(initExpr->IgnoreParenCasts())) {
                                newVarDecl = varDecl;
                            }
                            else if (auto* callExpr =
                                    clang::dyn_cast<clang::CallExpr>(initExpr->IgnoreParenCasts())) {
                                if (auto callee = callExpr->getDirectCallee(); callee && callee->getNameInfo().getAsString() == "malloc") {
                                    newVarDecl = varDecl;
                                }
                            }
                        }
                    }
                }
            }

            void VisitCallExpr(clang::CallExpr* callExpr)
            {
                if (auto callee = callExpr->getDirectCallee(); callee && callee->getNameInfo().getAsString() == "free") {
                    clang::Expr* arg = callExpr->getArg(0);
                    if (auto* declRefExpr =
                            clang::dyn_cast<clang::DeclRefExpr>(arg->IgnoreParenCasts())) {
                        deleteVarDecl = clang::dyn_cast<clang::VarDecl>(declRefExpr->getDecl());
                    }
                }
            }

            void VisitCXXDeleteExpr(clang::CXXDeleteExpr* deleteExpr)
            {
                if (auto* declRefExpr =
                        clang::dyn_cast<clang::DeclRefExpr>(
                                deleteExpr->getArgument()->IgnoreParenCasts())) {
                    deleteVarDecl = clang::dyn_cast<clang::VarDecl>(declRefExpr->getDecl());
                }
            }

            void performNewDeleteAnalysis(clang::Stmt *stmt)
            {
                if (stmt) {
                    Visit(stmt);
                }
            }

            clang::VarDecl* getNewVarDecl()
            {
                return newVarDecl;
            }

            clang::VarDecl* getDeleteVarDecl()
            {
                return deleteVarDecl;
            }

        private:

            clang::VarDecl* newVarDecl = nullptr;

            clang::VarDecl* deleteVarDecl = nullptr;

        };


        class Analysis: public libdataflow::AbstractDataflowAnalysis<fact::SetFact<ir::Var>> {
        public:

            [[nodiscard]] bool isForward() const override
            {
                return true;
            }

            [[nodiscard]] std::shared_ptr<fact::SetFact<ir::Var>> newBoundaryFact() const override
            {
                return newInitialFact();
            }

            [[nodiscard]] std::shared_ptr<fact::SetFact<ir::Var>> newInitialFact() const override
            {
                return std::make_shared<fact::SetFact<ir::Var>>();
            }

            void meetInto(std::shared_ptr<fact::SetFact<ir::Var>> fact,
                                        std::shared_ptr<fact::SetFact<ir::Var>> target) const override
            {
                target->unionN(fact);
            }

            [[nodiscard]] bool transferNode(
                    std::shared_ptr<ir::Stmt> stmt,
                    std::shared_ptr<fact::SetFact<ir::Var>> in,
                    std::shared_ptr<fact::SetFact<ir::Var>> out) const override
            {
                std::shared_ptr<fact::SetFact<ir::Var>> oldOut = out->copy();
                out->setSetFact(in);
                NewDeleteVisitor newDeleteVisitor;
                newDeleteVisitor.performNewDeleteAnalysis(
                        const_cast<clang::Stmt*>(stmt->getClangStmt()));
                if (clang::VarDecl* newVarDecl = newDeleteVisitor.getNewVarDecl()) {
                    out->add(clangToLocalVarMap.at(newVarDecl));
                } else if (clang::VarDecl* deleteVarDecl = newDeleteVisitor.getDeleteVarDecl()) {
                    out->remove(clangToLocalVarMap.at(deleteVarDecl));
                }
                return !out->equalsTo(oldOut);
            }

            [[nodiscard]] std::shared_ptr<fact::DataflowResult<fact::SetFact<ir::Var>>>
                getResult() const override
            {
                return result;
            }

            explicit Analysis(const std::shared_ptr<graph::CFG>& myCFG)
                : AbstractDataflowAnalysis<fact::SetFact<ir::Var>>(myCFG)
            {
                for(std::shared_ptr<ir::Var>& var : myCFG->getIR()->getVars()) {
                    clangToLocalVarMap.insert_or_assign(var->getClangVarDecl(), var);
                }

                result = std::make_shared<fact::DataflowResult<fact::SetFact<ir::Var>>>();

            }

        private:

            std::unordered_map<const clang::VarDecl*, std::shared_ptr<ir::Var>> clangToLocalVarMap;

            std::shared_ptr<fact::DataflowResult<fact::SetFact<ir::Var>>> result;

        };

        return std::make_unique<Analysis>(cfg);
    }

}
