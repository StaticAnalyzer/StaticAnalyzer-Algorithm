#include "myanalysis/dataflow/UseBeforeDef.h"

namespace analyzer::analysis::dataflow {

    UseBeforeDef::UseBeforeDef(std::unique_ptr<config::AnalysisConfig>& analysisConfig)
        :AnalysisDriver<fact::SetFact<ir::Var>>(analysisConfig)
    {

    }

    std::unique_ptr<DataflowAnalysis<fact::SetFact<ir::Var>>>
        UseBeforeDef::makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const
    {

        class Analysis: public AbstractDataflowAnalysis<fact::SetFact<ir::Var>>{
        public:

            [[nodiscard]] bool isForward() const override
            {
                return true;
            }

            [[nodiscard]] std::shared_ptr<fact::SetFact<ir::Var>> newBoundaryFact() const override
            {
                auto boundaryFact = std::make_shared<fact::SetFact<ir::Var>>();
                for (const auto &var : cfg->getIR()->getVars()) {
                    boundaryFact->add(var);
                }
                for (const auto &param : cfg->getIR()->getParams()) {
                    boundaryFact->remove(param);
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

            explicit Analysis(const std::shared_ptr<graph::CFG>& myCFG)
                : AbstractDataflowAnalysis<fact::SetFact<ir::Var>>(myCFG)
            {

            }

        };

        return std::make_unique<Analysis>(cfg);
    }

}
