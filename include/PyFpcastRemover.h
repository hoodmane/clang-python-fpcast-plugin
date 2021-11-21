#ifndef MyPlugin_H
#define MyPlugin_H

#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Rewrite/Core/Rewriter.h"

//-----------------------------------------------------------------------------
// ASTMatcher callback
//-----------------------------------------------------------------------------
class LACommenterMatcher
    : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  LACommenterMatcher(clang::Rewriter &LACRewriter, std::unique_ptr<llvm::raw_ostream>&& os) : LACRewriter(LACRewriter),  os_(std::move(os))  {}

  void run(const clang::ast_matchers::MatchFinder::MatchResult &) override;
  void onEndOfTranslationUnit() override;

private:

  clang::Rewriter LACRewriter;
  std::unique_ptr<llvm::raw_ostream>&& os_;
  llvm::SmallSet<clang::FullSourceLoc, 8> EditedLocations;
};

//-----------------------------------------------------------------------------
// ASTConsumer
//-----------------------------------------------------------------------------
class LACommenterASTConsumer : public clang::ASTConsumer {
public:
  LACommenterASTConsumer(clang::Rewriter &R, std::unique_ptr<llvm::raw_ostream>&& os);
  void HandleTranslationUnit(clang::ASTContext &Ctx) override {
    Finder.matchAST(Ctx);
  }

private:
  clang::ast_matchers::MatchFinder Finder;
  LACommenterMatcher LACHandler;
};

#endif
