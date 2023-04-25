#include "myanalysis/Analysis.h"

namespace my_analysis {

    AlwaysFailedAnalysis::AlwaysFailedAnalysis() = default;

    void AlwaysFailedAnalysis::analyze()
    {
        initializeFailedResult("Always Failed Analysis",
                               "always failed analysis used for test");
    }

}
