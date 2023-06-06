#include <iostream>
#include <sstream>
#include <bitset>
#include "myanalysis/Analysis.h"

#include "World.h"
#include "clang/AST/StmtVisitor.h"

namespace my_analysis
{
    class ArithmeticVisitor : public clang::StmtVisitor<ArithmeticVisitor, void> {
    public:
        void VisitBinaryOperator(clang::BinaryOperator *binaryOp) {
            const auto& astContext = analyzer::World::get().getAstList().front()->getASTContext();

            clang::Expr::EvalResult constResult;
            bool isExprConstant = binaryOp->EvaluateAsRValue(constResult, astContext);
            if (isExprConstant) {
                // expr evaluate to constant
                std::string resultDumpString;
                llvm::raw_string_ostream os(resultDumpString);
                constResult.Val.printPretty(os, astContext, binaryOp->getType());
                os.flush();
                std::string msg = "could be folded to ";
                msg += resultDumpString;
                addOptimizeResult(binaryOp->getBeginLoc(), binaryOp->getEndLoc(), msg);
                return;
            }

            clang::Expr* lhs = binaryOp->getLHS();
            clang::Expr* rhs = binaryOp->getRHS();

            Visit(lhs);
            Visit(rhs);

            if (!lhs->getType()->isIntegerType() || !rhs->getType()->isIntegerType())
                return;   // process only integer types

            auto lhsConstant = lhs->getIntegerConstantExpr(astContext);
            auto rhsConstant = rhs->getIntegerConstantExpr(astContext);
            if (rhsConstant.has_value()) {
                auto rhsValue = rhsConstant.value().tryZExtValue().value_or(-1);
                int powOf2 = calcPowOf2(rhsValue);
                if (powOf2 != -1) {
                    if (binaryOp->getOpcode() == clang::BinaryOperatorKind::BO_Rem) {
                        // %
                        std::stringstream ss;
                        ss << "%" << rhsValue << " can be replaced by ";
                        ss << "&" << (1 << powOf2) - 1;
                        addOptimizeResult(binaryOp->getBeginLoc(), binaryOp->getEndLoc(), ss.str());
                    } else if (binaryOp->getOpcode() == clang::BinaryOperatorKind::BO_Div) {
                        // /
                        std::stringstream ss;
                        ss << "/" << rhsValue << " can be replaced by ";
                        ss << ">>" << powOf2;
                        addOptimizeResult(binaryOp->getBeginLoc(), binaryOp->getEndLoc(), ss.str());
                    } else if (binaryOp->getOpcode() == clang::BinaryOperatorKind::BO_Mul) {
                        // *
                        std::stringstream ss;
                        ss << "*" << rhsValue << " can be replaced by ";
                        ss << "<<" << powOf2;
                        addOptimizeResult(binaryOp->getBeginLoc(), binaryOp->getEndLoc(), ss.str());
                    }
                }
            } else if (lhsConstant.has_value()) {
                auto lhsValue = lhsConstant.value().getExtValue();
                int powOf2 = calcPowOf2(lhsValue);
                if (powOf2 != -1) {
                    if (binaryOp->getOpcode() == clang::BinaryOperatorKind::BO_Mul) {
                        std::stringstream ss;
                        ss << "*" << lhsValue << " can be replaced by ";
                        ss << "<<" << powOf2;
                        addOptimizeResult(binaryOp->getBeginLoc(), binaryOp->getEndLoc(), ss.str());
                    }
                }
            }
        }

        void VisitStmt(clang::Stmt *stmt) {
            for (clang::Stmt *child: stmt->children())
                if (child) Visit(child);
        }

        void performArithmeticAnalysis(clang::Stmt *stmt) {
            return Visit(stmt);
        }

        [[nodiscard]] auto getOptimizeResults() const {
            return optimizeResults;
        }

    private:
        std::vector<std::tuple<clang::SourceLocation, clang::SourceLocation, std::string>> optimizeResults;

        void addOptimizeResult(clang::SourceLocation startLoc, clang::SourceLocation endLoc, const std::string& message) {
            optimizeResults.emplace_back(startLoc, endLoc, message);
        }

        static int calcPowOf2(uint64_t val) {
            bool isPowOf2 = std::bitset<64>(val).count() == 1;
            if (!isPowOf2)
                return -1;
            for (int i = 0; i < 64; ++i)
                if (val & (1 << i))
                    return i;
            return -1;
        }
    };

    ArithmeticIntensityAnalysis::ArithmeticIntensityAnalysis() = default;

    void ArithmeticIntensityAnalysis::analyze()
    {
        initializeSuccessfulResult("Arithmetic Intensity Reduce Analysis");

        const analyzer::World &world = analyzer::World::get();

        for (const auto &[_, method] : world.getAllMethods()) {
            std::shared_ptr<analyzer::ir::IR> ir = method->getIR();

            ArithmeticVisitor arithmeticVisitor;
            for (const auto &stmt : ir->getStmts()) {
                const clang::Stmt *clangStmt = stmt->getClangStmt();
                if (clangStmt)
                    arithmeticVisitor.performArithmeticAnalysis(const_cast<clang::Stmt *>(clangStmt));
            }

            for (const auto& [startLoc, endLoc, msg] : arithmeticVisitor.getOptimizeResults()) {
                const auto& sourceManager = method->getASTUnit()->getASTContext().getSourceManager();
                auto startLine = static_cast<int>(sourceManager.getPresumedLineNumber(
                        sourceManager.getExpansionLoc(startLoc)));
                auto endLine = static_cast<int>(sourceManager.getPresumedLineNumber(
                        sourceManager.getExpansionLoc(startLoc)));
                auto startColumn = static_cast<int>(sourceManager.getPresumedColumnNumber(
                        sourceManager.getExpansionLoc(endLoc)));
                auto endColumn = static_cast<int>(sourceManager.getPresumedColumnNumber(
                        sourceManager.getExpansionLoc(endLoc)));
                addFileResultEntry(method->getContainingFilePath(), startLine, startColumn, endLine, endColumn,
                                   AnalysisResult::Severity::Hint, msg);
            }
        }
    }

}
