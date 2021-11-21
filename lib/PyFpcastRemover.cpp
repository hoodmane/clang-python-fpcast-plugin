#include "PyFpcastRemover.h"

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

#define METH_VARARGS  0x0001
#define METH_KEYWORDS 0x0002
/* METH_NOARGS and METH_O must not be combined with the flags above. */
#define METH_NOARGS   0x0004
#define METH_O        0x0008
#define METH_FASTCALL  0x0080
#define METH_METHOD 0x0200

using namespace clang;
using namespace ast_matchers;

static int64_t getTypeEnum(ASTContext *Ctx, const InitListExpr *expr){
  const Expr *type_expr = expr->inits()[2];
  auto type_info = type_expr->getIntegerConstantExpr(*Ctx, NULL, false);
  if(!type_info.hasValue()){
    return -1;
  }
  return type_info.getValue().getExtValue();
}

static const FunctionDecl *
getFunctionDecl(const Expr *expr){
  const CastExpr *cast_expr = dyn_cast<CastExpr>(expr);
  if(!cast_expr){
    return NULL;
  }
  const DeclRefExpr *decl_ref = dyn_cast<DeclRefExpr>(cast_expr->getSubExprAsWritten());
  const FunctionDecl *decl = dyn_cast<FunctionDecl>(decl_ref->getDecl());
  return decl;
}

void handlePyMethodDef(Rewriter *LACRewriter, ASTContext *Ctx, const InitListExpr *expr){
  // expr->dump();
  if(expr->getNumInits() < 3){
    return;
  }
  int64_t ty = getTypeEnum(Ctx, expr);
  unsigned expected_args;
  switch(ty){
    case METH_O:
      expected_args = 2;
      break;
    case METH_NOARGS:
      expected_args = 2;
      break;
    case METH_VARARGS:
      expected_args = 2;
      break;
    case METH_VARARGS | METH_KEYWORDS:
      expected_args = 3;
      break;
    default:
      return;
  }


  const FunctionDecl *funcDecl = getFunctionDecl(expr->inits()[1]);
  // funcDecl->dump();
  if(funcDecl->getNumParams() == expected_args){
    return;
  }
  if(funcDecl->getNumParams() == expected_args - 1){
    FullSourceLoc paramLocation = Ctx->getFullLoc(funcDecl->getParametersSourceRange().getEnd());
    LACRewriter->InsertTextAfterToken(
      paramLocation,
      ", PyObject *ignored"
    );
  }
}

void handlePyGetSetDef(Rewriter *LACRewriter, ASTContext *Ctx, const InitListExpr *expr){
  int num_inits = expr->getNumInits();
  if(num_inits < 2){
    return;
  }
  
  const FunctionDecl *funcDecl; 
  printf("\nexpr->inits()[1]:\n");
  expr->inits()[1]->dump();
  funcDecl = getFunctionDecl(expr->inits()[1]);
  if(funcDecl && funcDecl->getNumParams() == 1){
    FullSourceLoc paramLocation = Ctx->getFullLoc(funcDecl->getParametersSourceRange().getEnd());
    LACRewriter->InsertTextAfterToken(
      paramLocation,
      ", void *ignored"
    );
  }

  if(num_inits < 3){
    return;
  }

  funcDecl = getFunctionDecl(expr->inits()[2]);
  if(funcDecl && funcDecl->getNumParams() == 2){
    FullSourceLoc paramLocation = Ctx->getFullLoc(funcDecl->getParametersSourceRange().getEnd());
    LACRewriter->InsertTextAfterToken(
      paramLocation,
      ", void *ignored"
    );
  }
  
}

//-----------------------------------------------------------------------------
// LACommenter - implementation
//-----------------------------------------------------------------------------
void LACommenterMatcher::run(const MatchFinder::MatchResult &Result) {
  ASTContext *Ctx = Result.Context;

  const InitListExpr *PyMethodDef =
      Result.Nodes.getNodeAs<clang::InitListExpr>("PyMethodDef");
  if(PyMethodDef){
    handlePyMethodDef(&LACRewriter, Ctx, PyMethodDef);
  }
  const InitListExpr *PyGetSetDef =
      Result.Nodes.getNodeAs<clang::InitListExpr>("PyGetSetDef");
  if(PyGetSetDef){
    handlePyGetSetDef(&LACRewriter, Ctx, PyGetSetDef);
  }

    
}

void LACommenterMatcher::onEndOfTranslationUnit() {
  // LACRewriter.getEditBuffer(LACRewriter.getSourceMgr().getMainFileID())
  //     .write(*os_);
  LACRewriter.getEditBuffer(LACRewriter.getSourceMgr().getMainFileID())
      .write(llvm::outs());
}

LACommenterASTConsumer::LACommenterASTConsumer(Rewriter &R, std::unique_ptr<llvm::raw_ostream>&& os) : 
  LACHandler(R, std::move(os)) {
  auto PyMethodDefMatcher = 	initListExpr(
    hasType(asString("PyMethodDef"))
  ).bind("PyMethodDef");
  
  Finder.addMatcher(PyMethodDefMatcher, &LACHandler);

  auto PyGetSetDefMatcher = initListExpr(
    hasType(asString("PyGetSetDef"))
  ).bind("PyGetSetDef");
  Finder.addMatcher(PyGetSetDefMatcher, &LACHandler);
}

//-----------------------------------------------------------------------------
// FrontendAction
//-----------------------------------------------------------------------------
class LACPluginAction : public PluginASTAction {
public:
  // Our plugin can alter behavior based on the command line options
  bool ParseArgs(const CompilerInstance &,
                 const std::vector<std::string> &) override {
    return true;
  }

  // Returns our ASTConsumer per translation unit.
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    RewriterForLAC.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    std::unique_ptr<raw_ostream> os = nullptr; //CI.createDefaultOutputFile(false, file, "cpp");
    // if (!os)
      // return nullptr;
    // *os << "//hi\n";
    return std::make_unique<LACommenterASTConsumer>(RewriterForLAC, std::move(os));
  }

private:
  Rewriter RewriterForLAC;
};

//-----------------------------------------------------------------------------
// Registration
//-----------------------------------------------------------------------------
static FrontendPluginRegistry::Add<LACPluginAction>
    X(/*Name=*/"pyfpcast",
      /*Desc=*/"Python function pointer cast remover");
