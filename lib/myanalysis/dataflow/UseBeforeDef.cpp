#include "myanalysis/dataflow/UseBeforeDef.h"

namespace my_analysis::dataflow {

    UseBeforeDef::UseBeforeDef(std::unique_ptr<config::AnalysisConfig>& analysisConfig)
        :AnalysisDriver<fact::SetFact<ir::Var>>(analysisConfig)
    {

    }

    std::unique_ptr<libdataflow::DataflowAnalysis<fact::SetFact<ir::Var>>>
        UseBeforeDef::makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const
    {

        class Analysis: public libdataflow::AbstractDataflowAnalysis<fact::SetFact<ir::Var>>{
        public:

            [[nodiscard]] bool isForward() const override
            {
                return true;
            }

            [[nodiscard]] std::shared_ptr<fact::SetFact<ir::Var>> newBoundaryFact() const override
            {
                auto boundaryFact = std::make_shared<fact::SetFact<ir::Var>>();
                for (const auto &var : cfg->getIR()->getVars()) {
                    const clang::VarDecl *decl = var->getClangVarDecl();
                    if (decl != nullptr && decl->isLocalVarDecl()) {
                        boundaryFact->add(var);
                    }
                }
                return boundaryFact;
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
                for (const std::shared_ptr<ir::Var>& def : stmt->getDefs()) {
                    out->remove(def);
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
                result = std::make_shared<fact::DataflowResult<fact::SetFact<ir::Var>>>();
            }

        private:

            std::shared_ptr<fact::DataflowResult<fact::SetFact<ir::Var>>> result;

        };

        return std::make_unique<Analysis>(cfg);
    }

}
