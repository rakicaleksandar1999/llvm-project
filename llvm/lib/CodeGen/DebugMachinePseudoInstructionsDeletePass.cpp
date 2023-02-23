//===-- DebugMachinePseudoInstructionsDeletePass.cpp
//------------------------------------===//
// DebugMachinePseudoInstructionsDeletePass implementation.
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

using namespace llvm;

namespace {
/// DebugMachinePseudoInstructionsDeletePass - This is a pass to delete the
/// debug machine pseudo-instructions.
///
struct DebugMachinePseudoInstructionsDeletePass : public MachineFunctionPass {
  static char ID;

  raw_ostream &OS;

  DebugMachinePseudoInstructionsDeletePass()
      : MachineFunctionPass(ID), OS(dbgs()) {}
  DebugMachinePseudoInstructionsDeletePass(raw_ostream &os,
                                           const std::string &banner)
      : MachineFunctionPass(ID), OS(os) {}

  StringRef getPassName() const override {
    return "Debug Machine Pseudo-Instructions Delete";
  }

  bool runOnMachineFunction(MachineFunction &MF) override {
    bool changed = false;
    for (MachineBasicBlock &MBB : MF) {
      for (MachineInstr &MI : make_early_inc_range(MBB)) {
        if (MI.isDebugInstr()) {
          MI.eraseFromParent();
          changed = true;
        }
      }
    }
    MF.print(OS, getAnalysisIfAvailable<SlotIndexes>());
    return changed;
  }
};

char DebugMachinePseudoInstructionsDeletePass::ID = 0;
} // namespace

char &llvm::DebugMachinePseudoInstructionsDeletePassID =
    DebugMachinePseudoInstructionsDeletePass::ID;
INITIALIZE_PASS(DebugMachinePseudoInstructionsDeletePass, "debug-mir-delete",
                "Debug Machine Pseudo-Instructions Delete", false, false)

namespace llvm {
/// Returns a newly-created MachineFunction Printer pass. The
/// default banner is empty.
///
MachineFunctionPass *
createDebugMachinePseudoInstructionsDeletePass(raw_ostream &OS,
                                               const std::string &Banner) {
  return new DebugMachinePseudoInstructionsDeletePass(OS, Banner);
}

} // namespace llvm
