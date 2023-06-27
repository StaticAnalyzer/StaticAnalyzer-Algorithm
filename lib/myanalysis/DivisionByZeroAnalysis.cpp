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

    static std::shared_ptr<df::CPValue> visitClangExpr(
        const clang::Expr *expr,
        const std::shared_ptr<df::CPResult> &res,
        std::unordered_set<const clang::Stmt*> &targets)
    {
        if (auto* castExpr = llvm::dyn_cast<clang::CastExpr>(expr)){
            auto subExpr = castExpr->getSubExpr();
            visitClangExpr(subExpr, res, targets);
        } else if (auto* parenExpr = llvm::dyn_cast<clang::ParenExpr>(expr)) {
            auto subExpr = parenExpr->getSubExpr();
            return visitClangExpr(subExpr, res, targets);
        } else if (auto* unaryOp = llvm::dyn_cast<clang::UnaryOperator>(expr)) {
            auto subExpr = unaryOp->getSubExpr();
            visitClangExpr(subExpr, res, targets);
        } else if (auto* binaryOperator = llvm::dyn_cast<clang::BinaryOperator>(expr)) {
            auto lhs = binaryOperator->getLHS();
            auto rhs = binaryOperator->getRHS();
            visitClangExpr(lhs, res, targets);
            auto rhsValue = visitClangExpr(rhs, res, targets);
            if (binaryOperator->getOpcode() == clang::BinaryOperatorKind::BO_Xor) {
                clang::VarDecl* varDecl = nullptr;
                if (auto* lhsCastExpr = llvm::dyn_cast<clang::CastExpr>(lhs)) {
                    if (auto* lhsDeclDef = llvm::dyn_cast<clang::DeclRefExpr>(lhsCastExpr->getSubExpr())) {
                        auto lhsDecl = lhsDeclDef->getDecl();
                        if (auto* lhsVarDecl = llvm::dyn_cast<clang::VarDecl>(lhsDecl)) {
                            varDecl = lhsVarDecl;
                        }
                    }
                }
                if (auto* rhsCastExpr = llvm::dyn_cast<clang::CastExpr>(rhs)) {
                    if (auto* rhsDeclDef = llvm::dyn_cast<clang::DeclRefExpr>(rhsCastExpr->getSubExpr())) {
                        auto rhsDecl = rhsDeclDef->getDecl();
                        if (auto* rhsVarDecl = llvm::dyn_cast<clang::VarDecl>(rhsDecl)) {
                            if (varDecl == rhsVarDecl) {
                                return df::CPValue::makeConstant(llvm::APSInt(llvm::APInt()));
                            }
                        }
                    }
                }
            }
            if (rhsValue->isConstant()) {
                switch (binaryOperator->getOpcode()) {
                    case clang::BinaryOperatorKind::BO_Div:
                    case clang::BinaryOperatorKind::BO_DivAssign:
                    case clang::BinaryOperatorKind::BO_Rem:
                    case clang::BinaryOperatorKind::BO_RemAssign:
                        if (rhsValue->getConstantValue().isZero()) {
                            targets.insert(binaryOperator);
                        }
                        break;
                    case clang::BinaryOperatorKind::BO_And:
                        if (rhsValue->getConstantValue().isZero()) {
                            return df::CPValue::makeConstant(llvm::APSInt(llvm::APInt()));
                        }
                        break;
                    default:
                        break;
                }
            }
        } else if (auto* arraySubscriptExpr = llvm::dyn_cast<clang::ArraySubscriptExpr>(expr)) {
            auto base = arraySubscriptExpr->getBase();
            auto index = arraySubscriptExpr->getIdx();
            visitClangExpr(base, res, targets);
            visitClangExpr(index, res, targets);
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

        auto value = res->getExprValue(expr);
        return value ? value : df::CPValue::getNAC();
    }

    DivisionByZeroAnalysis::DivisionByZeroAnalysis() = default;

    void DivisionByZeroAnalysis::analyze()
    {
        initializeSuccessfulResult("Division By Zero Analysis");

        const al::World &world = al::World::get();

        std::unique_ptr<cf::AnalysisConfig> analysisConfig;
        analysisConfig = std::make_unique<cf::DefaultAnalysisConfig>("division by zero analysis");
        std::unique_ptr<df::ConstantPropagation> cp = std::make_unique<df::ConstantPropagation>(analysisConfig);

        for (const auto& [_, method] : world.getAllMethods()) {
            std::unordered_set<const clang::Stmt*> targets;
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
                    "division by zero"
                );
            }

        }

    }

}
