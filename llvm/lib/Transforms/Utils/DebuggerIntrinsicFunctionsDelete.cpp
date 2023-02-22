//===-- DebuggerIntrinsicFunctionsDelete.cpp - Example Transformations
//--------------------------===//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/DebuggerIntrinsicFunctionsDelete.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

PreservedAnalyses
DebuggerIntrinsicFunctionsDeletePass::run(Function &F,
                                          FunctionAnalysisManager &AM) {
  for (BasicBlock &bb : F) {
    for (Instruction &i : make_early_inc_range(bb)) {
      if (CallBase::classof(&i)) {
        CallBase *callBase = (CallBase *)&i;
        StringRef funName = callBase->getCalledFunction()->getName();
        if (funName.equals("llvm.dbg.addr") ||
            funName.equals("llvm.dbg.declare") ||
            funName.equals("llvm.dbg.value") ||
            funName.equals("llvm.dbg.assign")) {
          i.eraseFromParent();
        }
      }
    }
  }
  errs() << F << "\n\n";
  return PreservedAnalyses::none();
}
