#ifndef STATIC_ANALYZER_USEBEFOREDEF_H
#define STATIC_ANALYZER_USEBEFOREDEF_H

#include <memory>

#include "analysis/dataflow/AnalysisDriver.h"
#include "analysis/dataflow/fact/SetFact.h"

namespace my_analysis::dataflow {

    namespace fact = analyzer::analysis::dataflow::fact;
    namespace ir = analyzer::ir;
    namespace libdataflow = analyzer::analysis::dataflow;
    namespace config = analyzer::config;
    namespace graph = analyzer::analysis::graph;

    /**
     * @class UseBeforeDef
     * @brief use before define analysis
     */
    class UseBeforeDef: public libdataflow::AnalysisDriver<fact::SetFact<ir::Var>> {
    public:

        /**
         * @brief constructor for use before define analysis
         * @param analysisConfig the analysis configuration
         */
        explicit UseBeforeDef(std::unique_ptr<config::AnalysisConfig>& analysisConfig);

    protected:

        [[nodiscard]] std::unique_ptr<libdataflow::DataflowAnalysis<fact::SetFact<ir::Var>>>
            makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const override;

    };


} // dataflow


#endif //STATIC_ANALYZER_USEBEFOREDEF_H
