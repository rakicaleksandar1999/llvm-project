//===-- DebuggerIntrinsicFunctionsCount.h - Example Transformations
//------------------*- C++ -*-===//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_UTILS_DEBUGGERINTRINSICFUNCTIONSCOUNT_H
#define LLVM_TRANSFORMS_UTILS_DEBUGGERINTRINSICFUNCTIONSCOUNT_H

#include "llvm/IR/PassManager.h"
#include <map>
#include <string>

namespace llvm {

class DebuggerIntrinsicFunctionsCountPass
    : public PassInfoMixin<DebuggerIntrinsicFunctionsCountPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);

private:
  std::map<std::string, int> opcodeCounter;
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_UTILS_DEBUGGERINTRINSICFUNCTIONSCOUNT_H
