#include <sstream>

#include "analysis/Analysis.h"
#include <llvm/Support/raw_ostream.h>
#include <unordered_set>

using namespace clang;

analysis::UninitializedVariableAnalysis::UninitializedVariableAnalysis(
    ASTResource &resource, ASTManager &manager, CallGraph &callGraph,
    Config &configure)
    : Analysis(resource, manager, callGraph, configure) {}

analysis::UninitializedVariableAnalysis::~UninitializedVariableAnalysis() {}

// 按照AST的顺序遍历每个Stmt
static void visitStmts(const Stmt *S, std::unordered_set<const VarDecl *> &uninit_vars) {
  // 如果是声明语句
  if (const DeclStmt *DS = dyn_cast<DeclStmt>(S)) {
    for (const Decl *D : DS->decls()) {
      // 如果是变量声明
      if (const VarDecl *VD = dyn_cast<VarDecl>(D)) {
        // 如果是局部变量且没有初始化
        if(VD->hasLocalStorage() && !VD->hasInit())
          uninit_vars.insert(VD);
      }
    }
  } else if (const BinaryOperator *BO =
                  dyn_cast<BinaryOperator>(S)) {
    // 如果是赋值语句
    if (BO->getOpcode() == BO_Assign) {
      const Expr *lhs = BO->getLHS();
      if (const DeclRefExpr *DRE = dyn_cast<DeclRefExpr>(lhs)) {
        const VarDecl *VD = dyn_cast<VarDecl>(DRE->getDecl());
        uninit_vars.erase(VD);
      }
    }
  } else if (const IfStmt *IS = dyn_cast<IfStmt>(S)) {
    // 如果是if语句
    visitStmts(IS->getThen(), uninit_vars);
    if(IS->getElse())
      visitStmts(IS->getElse(), uninit_vars);
  } else if (const WhileStmt *WS = dyn_cast<WhileStmt>(S)) {
    // 如果是while语句
    visitStmts(WS->getBody(), uninit_vars);
  } else if (const ForStmt *FS = dyn_cast<ForStmt>(S)) {
    // 如果是for语句
    visitStmts(FS->getBody(), uninit_vars);
  } else if (const DoStmt *DS = dyn_cast<DoStmt>(S)) {
    // 如果是do-while语句
    visitStmts(DS->getBody(), uninit_vars);
  } else if (const SwitchStmt *SS = dyn_cast<SwitchStmt>(S)) {
    // 如果是switch语句
    visitStmts(SS->getBody(), uninit_vars);
  } else if (const CompoundStmt *CS = dyn_cast<CompoundStmt>(S)) {
    // 如果是复合语句
    for (const Stmt *S : CS->body()) {
      visitStmts(S, uninit_vars);
    }
  }
}

// 递归遍历CFG
static void recursiveVisitBlock(const CFGBlock *block,
                                std::unordered_set<const CFGBlock *> &visited,
                                std::unordered_set<const VarDecl *> &uninit_vars) {
  if(visited.find(block) != visited.end())
    return;
  visited.insert(block);

  // 遍历block中的语句
  for (const CFGElement &element : *block) {
    if (Optional<CFGStmt> CS = element.getAs<CFGStmt>()) {
      const Stmt *S = CS->getStmt();
      visitStmts(S, uninit_vars);
    }
  }

  // 遍历block的后继
  for (const CFGBlock *succ : block->succs()) {
    recursiveVisitBlock(succ, visited, uninit_vars);
  }
}

void analysis::UninitializedVariableAnalysis::analyze() {
  initializeSuccessfulResult(std::string("UninitializedVariableAnalysis"));

  for (ASTFunction *fun : resource.getFunctions()) {
    FunctionDecl *funDecl = manager.getFunctionDecl(fun);
    std::unique_ptr<clang::CFG> &cfg = manager.getCFG(fun);

    std::unordered_set<const CFGBlock *> visited;
    std::unordered_set<const VarDecl *> uninit_vars;
    recursiveVisitBlock(&cfg->getEntry(), visited, uninit_vars);

    // 输出未初始化变量
    if (uninit_vars.empty()) {
      continue;
    }

    for (const VarDecl *VD : uninit_vars) {
      SourceLocation startLoc = VD->getBeginLoc();
      SourceLocation endLoc = VD->getEndLoc();
      SourceManager &SM = manager.getASTContext(fun)->getSourceManager();
      
      addFileResultEntry(fun->getASTFile()->getAST(),
        SM.getPresumedLineNumber(startLoc), SM.getPresumedColumnNumber(startLoc),
        SM.getPresumedLineNumber(endLoc), SM.getPresumedColumnNumber(endLoc),
        AnalysisResult::Severity::Warning, std::string("Uninitialized variable")
        );
    }
  }

}
