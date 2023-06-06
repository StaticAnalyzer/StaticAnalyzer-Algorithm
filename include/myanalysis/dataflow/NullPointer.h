#ifndef STATIC_ANALYZER_MEMORYLEAK_H
#define STATIC_ANALYZER_MEMORYLEAK_H

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
     * @class MemoryLeak
     * @brief memory leak analysis
     */
    class NullPointer: public libdataflow::AnalysisDriver<fact::SetFact<ir::Var>> {
    public:

        /**
         * @brief constructor for use before define analysis
         * @param analysisConfig the analysis configuration
         */
        explicit NullPointer(std::unique_ptr<config::AnalysisConfig>& analysisConfig);

    protected:

        [[nodiscard]] std::unique_ptr<libdataflow::DataflowAnalysis<fact::SetFact<ir::Var>>>
            makeAnalysis(const std::shared_ptr<graph::CFG>& cfg) const override;

    };


} // dataflow


#endif //STATIC_ANALYZER_MEMORYLEAK_H
