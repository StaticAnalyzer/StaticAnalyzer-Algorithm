#include "myanalysis/dataflow/NullPointer.h"

#include <utility>

#include "clang/AST/StmtVisitor.h"

namespace analyzer::analysis::dataflow {

    NullPointer::NullPointer(std::unique_ptr<config::AnalysisConfig>& analysisConfig)
            :AnalysisDriver<fact::SetFact<ir::Var>>(analysisConfig)
    {

    }

    std::unique_ptr<DataflowAnalysis<fact::SetFact<ir::Var>>>
    NullPointer::makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const
    {
        class PointerAssignVisitor: public clang::StmtVisitor<PointerAssignVisitor, void> {
        public:
            void VisitBinAssign(clang::BinaryOperator* binAssign) {
                auto lhsExpr = binAssign->getLHS()->IgnoreParenCasts();
                auto rhsExpr = binAssign->getRHS()->IgnoreParenCasts();
                if (auto declRef = clang::dyn_cast<clang::DeclRefExpr>(lhsExpr)) {
                    auto varDecl = clang::dyn_cast<clang::VarDecl>(declRef->getDecl());

                    if (varDecl && varDecl->getType()->isPointerType()) {
                        if (rhsExpr->isNullPointerConstant(cfg->getIR()->getMethod().getASTUnit()->getASTContext(),
                                                           clang::Expr::NPC_ValueDependentIsNotNull)) {
                            nullPtrVarDecls.push_back(varDecl);
                        }
                        else {
                            defPtrVarDecls.push_back(varDecl);
                        }
                    }
                }

                Visit(binAssign->getLHS());
                Visit(binAssign->getRHS());
            }

            void VisitDeclStmt(clang::DeclStmt* declStmt) {
                for (auto decl: declStmt->decls()) {
                    if (auto varDecl = clang::dyn_cast<clang::VarDecl>(decl)) {
                        if (auto initExpr = varDecl->getInit()) {
                            if (initExpr->isNullPointerConstant(cfg->getIR()->getMethod().getASTUnit()->getASTContext(),
                                                               clang::Expr::NPC_ValueDependentIsNotNull)) {
                                nullPtrVarDecls.push_back(varDecl);
                            }
                            else {
                                defPtrVarDecls.push_back(varDecl);
                            }
                        }
                    }
                }
            }

            void visitPointerAssign(clang::Stmt* stmt, std::shared_ptr<graph::CFG> pCfg) {
                cfg = std::move(pCfg);
                if (stmt)
                    Visit(stmt);
            }

            std::vector<const clang::VarDecl*> getNullPtrVarDecls() {
                return nullPtrVarDecls;
            }

            std::vector<const clang::VarDecl*> getDefPtrVarDecls() {
                return defPtrVarDecls;
            }


        private:
            std::shared_ptr<graph::CFG> cfg;
            std::vector<const clang::VarDecl*> nullPtrVarDecls;
            std::vector<const clang::VarDecl*> defPtrVarDecls;
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
                PointerAssignVisitor visitor;
                visitor.visitPointerAssign(const_cast<clang::Stmt*>(clangStmt), cfg);

                for (auto varDecl: visitor.getNullPtrVarDecls()) {
                    auto var = clangToLocalVarMap.at(varDecl);
                    out->add(var);
                }

                for (auto varDecl: visitor.getDefPtrVarDecls()) {
                    auto var = clangToLocalVarMap.at(varDecl);
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
