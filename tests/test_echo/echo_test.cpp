#include <gtest/gtest.h>
#include <iostream>

#include "analysis/AnalysisFactory.h"

TEST(EchoTest, Test1) {
    analysis::AnalysisFactory analysisFactory(
        "/home/ubuntu/StaticAnalyzer-Algorithm/tests/test_echo/astlist.txt",
        "/home/ubuntu/StaticAnalyzer-Algorithm/tests/test_echo/config.txt");
    std::unique_ptr<analysis::Analysis> echo = analysisFactory.createEchoAnalysis();
    std::cout << echo->analyze() << std::endl;
    EXPECT_STREQ(R"(----------------------------------------------------------
CFG of all functions:
----------------------------------------------------------
CFG of function fib:
---------------------

 [B6 (ENTRY)]
   Succs (1): B5

 [B1]
   1: return i;
   Preds (1): B4
   Succs (1): B0

 [B2]
   Preds (1): B3
   Succs (1): B4

 [B3]
   1: int tmp = i;
   2: i = j
   3: j = i + tmp
   4: n--
   Preds (1): B4
   Succs (1): B2

 [B4]
   1: n > 0
   T: while [B4.1]
   Preds (2): B2 B5
   Succs (2): B3 B1

 [B5]
   1: int i = 0;
   2: int j = 1;
   Preds (1): B6
   Succs (1): B4

 [B0 (EXIT)]
   Preds (1): B1

CFG of function main:
---------------------

 [B6 (ENTRY)]
   Succs (1): B5

 [B1]
   1: return 0;
   Preds (1): B4
   Succs (1): B0

 [B2]
   1: i++
   Preds (1): B3
   Succs (1): B4

 [B3]
   1: fib(i)
   2: x = [B3.1]
   Preds (1): B4
   Succs (1): B2

 [B4]
   1: i < 10
   T: for (...; [B4.1]; ...)
   Preds (2): B2 B5
   Succs (2): B3 B1

 [B5]
   1: int x;
   2: int i = 0;
   Preds (1): B6
   Succs (1): B4

 [B0 (EXIT)]
   Preds (1): B1

----------------------------------------------------------
Call Graph:
----------------------------------------------------------
Callee of main  . 
 fib  int  
Callee of fib  int  . 
----------------------------------------------------------
)", echo->analyze().c_str());

}
