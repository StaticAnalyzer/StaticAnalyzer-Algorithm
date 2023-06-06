#include "myanalysis/Analysis.h"

#include "World.h"
#include "analysis/dataflow/ConstantPropagation.h"

namespace al = analyzer;
namespace air = al::ir;
namespace cf = al::config;
namespace df = al::analysis::dataflow;
namespace dfact = df::fact;

namespace my_analysis
{

    static void visitClangExpr(
            const clang::Expr *expr,
            const std::shared_ptr<df::CPResult> &res,
            std::vector<const clang::Stmt*> &targets)
    {
        if (auto* castExpr = llvm::dyn_cast<clang::CastExpr>(expr)){
            auto subExpr = castExpr->getSubExpr();
            visitClangExpr(subExpr, res, targets);
        } else if (auto* parenExpr = llvm::dyn_cast<clang::ParenExpr>(expr)) {
            auto subExpr = parenExpr->getSubExpr();
            visitClangExpr(subExpr, res, targets);
        } else if (auto* unaryOp = llvm::dyn_cast<clang::UnaryOperator>(expr)) {
            auto subExpr = unaryOp->getSubExpr();
            visitClangExpr(subExpr, res, targets);
        } else if (auto* binaryOperator = llvm::dyn_cast<clang::BinaryOperator>(expr)) {
            auto lhs = binaryOperator->getLHS();
            auto rhs = binaryOperator->getRHS();
            visitClangExpr(lhs, res, targets);
            visitClangExpr(rhs, res, targets);
        } else if (auto* arraySubscriptExpr = llvm::dyn_cast<clang::ArraySubscriptExpr>(expr)) {
            auto base = arraySubscriptExpr->getBase();
            auto index = arraySubscriptExpr->getIdx();
            visitClangExpr(base, res, targets);
            visitClangExpr(index, res, targets);
            auto index_value = res->getExprValue(index);
            const clang::ConstantArrayType *arrayType = nullptr;
            if (auto* implicitCastExpr = llvm::dyn_cast<clang::ImplicitCastExpr>(base))
                if (implicitCastExpr->getCastKind() == clang::CastKind::CK_ArrayToPointerDecay) {
                    auto* subExpr = implicitCastExpr->getSubExpr();
                    if (auto* arraySubscriptExpr = llvm::dyn_cast<clang::ArraySubscriptExpr>(subExpr)) {
                        arrayType = clang::dyn_cast<clang::ConstantArrayType>(arraySubscriptExpr->getType());
                    } else if (auto* declRefExpr = llvm::dyn_cast<clang::DeclRefExpr>(subExpr)) {
                        arrayType = clang::dyn_cast<clang::ConstantArrayType>(declRefExpr->getType());
                    }
                }
            if (index_value != nullptr && index_value->isConstant() && arrayType != nullptr) {
                auto index_const = index_value->getConstantValue();
                llvm::APSInt size = llvm::APSInt(arrayType->getSize(), index_const.isUnsigned());
                if (index_const.isNegative() || index_const >= size) {
                    targets.push_back(index);
                }
            }
        } else if (auto* conditionalOperator = llvm::dyn_cast<clang::ConditionalOperator>(expr)) {
            auto cond = conditionalOperator->getCond();
            auto trueExpr = conditionalOperator->getTrueExpr();
            auto falseExpr = conditionalOperator->getFalseExpr();
            visitClangExpr(cond, res, targets);
            visitClangExpr(trueExpr, res, targets);
            visitClangExpr(falseExpr, res, targets);
        } else if(auto* callExpr = llvm::dyn_cast<clang::CallExpr>(expr)) {
            auto callee = callExpr->getCallee();
            visitClangExpr(callee, res, targets);
            for(auto arg : callExpr->arguments()) {
                visitClangExpr(arg, res, targets);
            }
        }
    }

    ArrayIndexOutOfBoundsAnalysis::ArrayIndexOutOfBoundsAnalysis() = default;

    void ArrayIndexOutOfBoundsAnalysis::analyze()
    {
        initializeSuccessfulResult("Array Index Out Of Bounds Analysis");

        const al::World &world = al::World::get();

        std::unique_ptr<cf::AnalysisConfig> analysisConfig;
        analysisConfig = std::make_unique<cf::DefaultAnalysisConfig>("array index out of bounds analysis");
        std::unique_ptr<df::ConstantPropagation> cp = std::make_unique<df::ConstantPropagation>(analysisConfig);

        for (const auto& [_, method] : world.getAllMethods()) {
            std::vector<const clang::Stmt*> targets;
            std::shared_ptr<air::IR> ir = method->getIR();
            clang::ASTContext &astContext = method->getASTUnit()->getASTContext();
            auto result = std::dynamic_pointer_cast<df::CPResult>(cp->analyze(ir));

            for (const auto& stmt : ir->getStmts()) {
                auto* clangStmt = stmt->getClangStmt();
                if (clangStmt == nullptr) {
                    continue;
                } else if (auto* declStmt = llvm::dyn_cast<clang::DeclStmt>(clangStmt)) {
                    for (auto decl : declStmt->decls()) {
                        if (auto* varDecl = llvm::dyn_cast<clang::VarDecl>(decl)) {
                            auto init = varDecl->getInit();
                            if (init != nullptr) {
                                visitClangExpr(init, result, targets);
                            }
                        }
                    }
                } else if (auto* expr = llvm::dyn_cast<clang::Expr>(clangStmt)) {
                    visitClangExpr(expr, result, targets);
                }
            }

            for(auto target : targets) {
                auto startLoc = target->getBeginLoc();
                auto endLoc = target->getEndLoc();
                int startLine = astContext.getSourceManager().getSpellingLineNumber(startLoc);
                int startColumn = astContext.getSourceManager().getSpellingColumnNumber(startLoc);
                int endLine = astContext.getSourceManager().getSpellingLineNumber(endLoc);
                int endColumn = astContext.getSourceManager().getSpellingColumnNumber(endLoc);
                addFileResultEntry(
                        method->getContainingFilePath(),
                        startLine, startColumn, endLine, endColumn,
                        AnalysisResult::Severity::Warning,
                        "Array index out of bounds"
                );
            }

        }

    }

}
