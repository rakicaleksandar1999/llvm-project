//===-- DebugMachinePseudoInstructionsCountPass.cpp
//------------------------------------===//
// DebugMachinePseudoInstructionsCountPass implementation.
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/IR/PrintPasses.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <map>

using namespace llvm;

namespace {
/// DebugMachinePseudoInstructionsCountPass - This is a pass to count the debug
/// machine pseudo-instructions.
///
struct DebugMachinePseudoInstructionsCountPass : public MachineFunctionPass {
  static char ID;

  raw_ostream &OS;

  DebugMachinePseudoInstructionsCountPass()
      : MachineFunctionPass(ID), OS(dbgs()) {}
  DebugMachinePseudoInstructionsCountPass(raw_ostream &os,
                                          const std::string &banner)
      : MachineFunctionPass(ID), OS(os) {}

  StringRef getPassName() const override {
    return "Debug Machine Pseudo-Instructions Count";
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addUsedIfAvailable<SlotIndexes>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &MF) override {
    OS << "Function: " << MF.getName() << "\n";
    std::map<int, int> opcodeCounter;
    for (MachineBasicBlock &MBB : MF) {
      for (MachineInstr &MI : MBB) {
        if (MI.isDebugInstr()) {
          if (opcodeCounter.find(MI.getOpcode()) == opcodeCounter.end()) {
            opcodeCounter[MI.getOpcode()] = 1;
          } else {
            opcodeCounter[MI.getOpcode()] += 1;
          }
        }
      }
    }
    for (std::map<int, int>::iterator i = opcodeCounter.begin(),
                                      e = opcodeCounter.end();
         i != e; i++) {
      switch (i->first) {
      case TargetOpcode::DBG_VALUE: {
        errs() << "DBG_VALUE";
        break;
      }
      case TargetOpcode::DBG_VALUE_LIST: {
        errs() << "DBG_VALUE_LIST";
        break;
      }
      case TargetOpcode::DBG_LABEL: {
        errs() << "DBG_LABEL";
        break;
      }
      case TargetOpcode::DBG_INSTR_REF: {
        errs() << "DBG_INSTR_REF";
        break;
      }
      case TargetOpcode::DBG_PHI: {
        errs() << "DBG_PHI";
        break;
      }
      }
      errs() << ": " << i->second << "\n";
    }
    opcodeCounter.clear();
    return false;
  }
};

char DebugMachinePseudoInstructionsCountPass::ID = 0;
} // namespace

char &llvm::DebugMachinePseudoInstructionsCountPassID =
    DebugMachinePseudoInstructionsCountPass::ID;
INITIALIZE_PASS(DebugMachinePseudoInstructionsCountPass, "debug-mir-count",
                "Debug Machine Pseudo-Instructions Count", false, false)

namespace llvm {
/// Returns a newly-created DebugMachinePseudoInstructionsCountPass. The
/// default banner is empty.
///
MachineFunctionPass *
createDebugMachinePseudoInstructionsCountPass(raw_ostream &OS,
                                              const std::string &Banner) {
  return new DebugMachinePseudoInstructionsCountPass(OS, Banner);
}

} // namespace llvm
