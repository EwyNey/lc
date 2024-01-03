#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

int main() {
  llvm::LLVMContext ctx;

  llvm::IRBuilder<> builder(ctx);

  llvm::FunctionType *fnty = llvm::FunctionType::get(
      llvm::Type::getInt8Ty(ctx),
      {llvm::Type::getInt8PtrTy(ctx, 1), llvm::Type::getInt8PtrTy(ctx, 1)},
      false);

  llvm::Module m("mod", ctx);

  llvm::Function *f = llvm::Function::Create(
      fnty, llvm::GlobalValue::LinkageTypes::ExternalLinkage, "f", &m);

  llvm::BasicBlock *body = llvm::BasicBlock::Create(ctx, "body", f);

  builder.SetInsertPoint(body);

  llvm::Value *arg0 =
      builder.CreateLoad(llvm::Type::getInt8Ty(ctx), f->getArg(0));
  llvm::Value *arg1 =
      builder.CreateLoad(llvm::Type::getInt8Ty(ctx), f->getArg(1));

  builder.CreateRet(builder.CreateAdd(arg0, arg1));

  m.dump();
}