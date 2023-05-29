#include "myanalysis/dataflow/MemoryLeak.h"

#include "clang/AST/StmtVisitor.h"

namespace analyzer::analysis::dataflow {

    MemoryLeak::MemoryLeak(std::unique_ptr<config::AnalysisConfig>& analysisConfig)
            :AnalysisDriver<fact::SetFact<ir::Var>>(analysisConfig)
    {

    }

    std::unique_ptr<DataflowAnalysis<fact::SetFact<ir::Var>>>
    MemoryLeak::makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const
    {
        class NewDeleteVisitor: public clang::StmtVisitor<NewDeleteVisitor, void> {
        public:
            void VisitBinAssign(clang::BinaryOperator* binAssign) {
                auto rhsExpr = binAssign->getRHS()->IgnoreParenCasts();
                if (auto newExpr = clang::dyn_cast<clang::CXXNewExpr>(rhsExpr)) {
                    auto varDeclExpr = clang::dyn_cast<clang::DeclRefExpr>(binAssign->getLHS()->IgnoreParenCasts());
                    auto varDecl = clang::dyn_cast<clang::VarDecl>(varDeclExpr->getDecl());
                    newVarDecl = varDecl;
                }
                else if(auto callExpr = clang::dyn_cast<clang::CallExpr>(rhsExpr)) {
                    auto funcName = callExpr->getDirectCallee()->getNameInfo().getAsString();
                    if (funcName == "malloc") {
                        auto varDeclExpr = clang::dyn_cast<clang::DeclRefExpr>(binAssign->getLHS()->IgnoreParenCasts());
                        auto varDecl = clang::dyn_cast<clang::VarDecl>(varDeclExpr->getDecl());
                        newVarDecl = varDecl;
                    }
                }
            }

            void VisitDeclStmt(clang::DeclStmt* declStmt) {
                for (auto decl: declStmt->decls()) {
                    if (auto varDecl = clang::dyn_cast<clang::VarDecl>(decl)) {
                        if (auto initExpr = varDecl->getInit()) {
                            if (auto newExpr = clang::dyn_cast<clang::CXXNewExpr>(initExpr->IgnoreParenCasts())) {
                                newVarDecl = varDecl;
                            }
                            else if (auto callExpr = clang::dyn_cast<clang::CallExpr>(initExpr->IgnoreParenCasts())) {
                                auto funcName = callExpr->getDirectCallee()->getNameInfo().getAsString();
                                if (funcName == "malloc") {
                                    newVarDecl = varDecl;
                                }
                            }
                        }
                    }
                }
            }

            void VisitCallExpr(clang::CallExpr* callExpr) {
                auto funcName = callExpr->getDirectCallee()->getNameInfo().getAsString();
                if (funcName == "free") {
                    auto arg = callExpr->getArg(0);
                    if (auto declRefExpr = clang::dyn_cast<clang::DeclRefExpr>(arg->IgnoreParenCasts())) {
                        deleteVarDecl = clang::dyn_cast<clang::VarDecl>(declRefExpr->getDecl());
                    }
                }
            }

            void VisitCXXDeleteExpr(clang::CXXDeleteExpr* deleteExpr) {
                auto arg = deleteExpr->getArgument();
                if (auto declRefExpr = clang::dyn_cast<clang::DeclRefExpr>(arg->IgnoreParenCasts())) {
                    deleteVarDecl = clang::dyn_cast<clang::VarDecl>(declRefExpr->getDecl());
                }
            }

            void performNewDeleteAnalysis(clang::Stmt *stmt) {
                if (stmt)
                    Visit(stmt);
            }

            clang::VarDecl* getNewVarDecl() {
                return newVarDecl;
            }

            clang::VarDecl* getDeleteVarDecl() {
                return deleteVarDecl;
            }

        private:
            clang::VarDecl* newVarDecl = nullptr;
            clang::VarDecl* deleteVarDecl = nullptr;
        };


        class Analysis: public AbstractDataflowAnalysis<fact::SetFact<ir::Var>>{
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
                auto clangStmt = stmt->getClangStmt();
                NewDeleteVisitor newDeleteVisitor;
                newDeleteVisitor.performNewDeleteAnalysis(const_cast<clang::Stmt*>(clangStmt));
                if (auto newVarDecl = newDeleteVisitor.getNewVarDecl()) {
                    auto var = clangToLocalVarMap.at(newVarDecl);
                    out->add(var);
                }
                else if (auto deleteVarDecl = newDeleteVisitor.getDeleteVarDecl()) {
                    auto var = clangToLocalVarMap.at(deleteVarDecl);
                    out->remove(var);
                }
                return !out->equalsTo(oldOut);
            }

            explicit Analysis(const std::shared_ptr<graph::CFG>& myCFG)
                : AbstractDataflowAnalysis<fact::SetFact<ir::Var>>(myCFG)
            {
                for(auto& var : myCFG->getIR()->getVars())
                    clangToLocalVarMap[var->getClangVarDecl()] = var;
            }

        private:
            std::unordered_map<const clang::VarDecl*, std::shared_ptr<ir::Var>> clangToLocalVarMap;

        };

        return std::make_unique<Analysis>(cfg);
    }

}
