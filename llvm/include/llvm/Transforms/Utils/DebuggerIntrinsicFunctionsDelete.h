//===-- DebuggerIntrinsicFunctionsDelete.h - Example Transformations ------------------*- C++ -*-===//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_UTILS_DEBUGGERINTRINSICFUNCTIONSDELETE_H
#define LLVM_TRANSFORMS_UTILS_DEBUGGERINTRINSICFUNCTIONSDELETE_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class DebuggerIntrinsicFunctionsDeletePass : public PassInfoMixin<DebuggerIntrinsicFunctionsDeletePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_DEBUGGERINTRINSICFUNCTIONSDELETE_H
