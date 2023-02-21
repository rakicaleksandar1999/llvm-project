//===-- DebuggerIntrinsicFunctionsCount.cpp - Example Transformations --------------------------===//
//===----------------------------------------------------------------------===//

#include "llvm/Transforms/Utils/DebuggerIntrinsicFunctionsCount.h"

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

PreservedAnalyses DebuggerIntrinsicFunctionsCountPass::run(Function &F,
                                      FunctionAnalysisManager &AM) {
  errs() << "Function: " << F.getName() << "\n";
  for(BasicBlock& bb : F){
    for(Instruction& i : bb){
      if(CallBase::classof(&i)){
        CallBase* callBase = (CallBase*) &i;
        StringRef funName = callBase->getCalledFunction()->getName();
        if(funName.equals("llvm.dbg.addr") || funName.equals("llvm.dbg.declare") || funName.equals("llvm.dbg.value") || funName.equals("llvm.dbg.assign")){
            std::string s;
          for(int i=0; i<funName.size(); i++){
            s.push_back(funName.data()[i]);
          }
          if(opcodeCounter.find(s) == opcodeCounter.end()) {
            opcodeCounter[s] = 1;
          } else {
            opcodeCounter[s] += 1;
          }
        }
      }
    }
  }
  for(std::map< std::string, int>::iterator i = opcodeCounter.begin(), e = opcodeCounter.end(); i!=e; i++){
    errs() << i->first << ": " << i->second << "\n";
  }
  opcodeCounter.clear();
  return PreservedAnalyses::all(); 
}
