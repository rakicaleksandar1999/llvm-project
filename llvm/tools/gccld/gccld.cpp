//===- gccld.cpp - LLVM 'ld' compatible linker ----------------------------===//
// 
//                     The LLVM Compiler Infrastructure
//
// This file was developed by the LLVM research group and is distributed under
// the University of Illinois Open Source License. See LICENSE.TXT for details.
// 
//===----------------------------------------------------------------------===//
//
// This utility is intended to be compatible with GCC, and follows standard
// system 'ld' conventions.  As such, the default output file is ./a.out.
// Additionally, this program outputs a shell script that is used to invoke LLI
// to execute the program.  In this manner, the generated executable (a.out for
// example), is directly executable, whereas the bytecode file actually lives in
// the a.out.bc file generated by this program.  Also, Force is on by default.
//
// Note that if someone (or a script) deletes the executable program generated,
// the .bc file will be left around.  Considering that this is a temporary hack,
// I'm not too worried about this.
//
//===----------------------------------------------------------------------===//

#include "gccld.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Bytecode/Reader.h"
#include "llvm/Bytecode/WriteBytecodePass.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Linker.h"
#include "Support/CommandLine.h"
#include "Support/FileUtilities.h"
#include "llvm/System/Signals.h"
#include "Support/SystemUtils.h"
#include <fstream>
#include <memory>

using namespace llvm;

namespace {
  cl::list<std::string> 
  InputFilenames(cl::Positional, cl::desc("<input bytecode files>"),
                 cl::OneOrMore);

  cl::opt<std::string> 
  OutputFilename("o", cl::desc("Override output filename"), cl::init("a.out"),
                 cl::value_desc("filename"));

  cl::opt<bool>    
  Verbose("v", cl::desc("Print information about actions taken"));
  
  cl::list<std::string> 
  LibPaths("L", cl::desc("Specify a library search path"), cl::Prefix,
           cl::value_desc("directory"));

  cl::list<std::string> 
  Libraries("l", cl::desc("Specify libraries to link to"), cl::Prefix,
            cl::value_desc("library prefix"));

  cl::opt<bool>
  Strip("s", cl::desc("Strip symbol info from executable"));

  cl::opt<bool>
  NoInternalize("disable-internalize",
                cl::desc("Do not mark all symbols as internal"));
  cl::alias
  ExportDynamic("export-dynamic", cl::desc("Alias for -disable-internalize"),
                cl::aliasopt(NoInternalize));

  cl::opt<bool>
  LinkAsLibrary("link-as-library", cl::desc("Link the .bc files together as a"
                                            " library, not an executable"));
  cl::alias
  Relink("r", cl::desc("Alias for -link-as-library"),
         cl::aliasopt(LinkAsLibrary));

  cl::opt<bool>    
  Native("native",
         cl::desc("Generate a native binary instead of a shell script"));
  cl::opt<bool>    
  NativeCBE("native-cbe",
            cl::desc("Generate a native binary with the C backend and GCC"));
  
  // Compatibility options that are ignored but supported by LD
  cl::opt<std::string>
  CO3("soname", cl::Hidden, cl::desc("Compatibility option: ignored"));
  cl::opt<std::string>
  CO4("version-script", cl::Hidden, cl::desc("Compatibility option: ignored"));
  cl::opt<bool>
  CO5("eh-frame-hdr", cl::Hidden, cl::desc("Compatibility option: ignored"));
  cl::opt<std::string>
  CO6("h", cl::Hidden, cl::desc("Compatibility option: ignored"));
}

/// PrintAndReturn - Prints a message to standard error and returns true.
///
/// Inputs:
///  progname - The name of the program (i.e. argv[0]).
///  Message  - The message to print to standard error.
///
static int PrintAndReturn(const char *progname, const std::string &Message) {
  std::cerr << progname << ": " << Message << "\n";
  return 1;
}

/// EmitShellScript - Output the wrapper file that invokes the JIT on the LLVM
/// bytecode file for the program.
static void EmitShellScript(char **argv) {
  // Output the script to start the program...
  std::ofstream Out2(OutputFilename.c_str());
  if (!Out2.good())
    exit(PrintAndReturn(argv[0], "error opening '" + OutputFilename +
                                 "' for writing!"));

  Out2 << "#!/bin/sh\n";
  // Allow user to setenv LLVMINTERP if lli is not in their PATH.
  Out2 << "lli=${LLVMINTERP-lli}\n";
  Out2 << "exec $lli \\\n";
  // gcc accepts -l<lib> and implicitly searches /lib and /usr/lib.
  LibPaths.push_back("/lib");
  LibPaths.push_back("/usr/lib");
  LibPaths.push_back("/usr/X11R6/lib");
  // We don't need to link in libc! In fact, /usr/lib/libc.so may not be a
  // shared object at all! See RH 8: plain text.
  std::vector<std::string>::iterator libc = 
    std::find(Libraries.begin(), Libraries.end(), "c");
  if (libc != Libraries.end()) Libraries.erase(libc);
  // List all the shared object (native) libraries this executable will need
  // on the command line, so that we don't have to do this manually!
  for (std::vector<std::string>::iterator i = Libraries.begin(), 
         e = Libraries.end(); i != e; ++i) {
    std::string FullLibraryPath = FindLib(*i, LibPaths, true);
    if (!FullLibraryPath.empty() && IsSharedObject(FullLibraryPath))
      Out2 << "    -load=" << FullLibraryPath << " \\\n";
  }
  Out2 << "    $0.bc ${1+\"$@\"}\n";
  Out2.close();
}

int main(int argc, char **argv, char **envp) {
  cl::ParseCommandLineOptions(argc, argv, " llvm linker for GCC\n");
  PrintStackTraceOnErrorSignal();

  std::string ModuleID("gccld-output");
  std::auto_ptr<Module> Composite(new Module(ModuleID));

  // We always look first in the current directory when searching for libraries.
  LibPaths.insert(LibPaths.begin(), ".");

  // If the user specified an extra search path in their environment, respect
  // it.
  if (char *SearchPath = getenv("LLVM_LIB_SEARCH_PATH"))
    LibPaths.push_back(SearchPath);

  // Remove any consecutive duplicates of the same library...
  Libraries.erase(std::unique(Libraries.begin(), Libraries.end()),
                  Libraries.end());

  // Link in all of the files
  if (LinkFiles(argv[0], Composite.get(), InputFilenames, Verbose))
    return 1; // Error already printed

  if (!LinkAsLibrary)
    LinkLibraries(argv[0], Composite.get(), Libraries, LibPaths,
                  Verbose, Native);

  // Link in all of the libraries next...

  // Create the output file.
  std::string RealBytecodeOutput = OutputFilename;
  if (!LinkAsLibrary) RealBytecodeOutput += ".bc";
  std::ofstream Out(RealBytecodeOutput.c_str());
  if (!Out.good())
    return PrintAndReturn(argv[0], "error opening '" + RealBytecodeOutput +
                                   "' for writing!");

  // Ensure that the bytecode file gets removed from the disk if we get a
  // SIGINT signal.
  RemoveFileOnSignal(RealBytecodeOutput);

  // Generate the bytecode file.
  if (GenerateBytecode(Composite.get(), Strip, !NoInternalize, &Out)) {
    Out.close();
    return PrintAndReturn(argv[0], "error generating bytecode");
  }

  // Close the bytecode file.
  Out.close();

  // If we are not linking a library, generate either a native executable
  // or a JIT shell script, depending upon what the user wants.
  if (!LinkAsLibrary) {
    // If the user wants to generate a native executable, compile it from the
    // bytecode file.
    //
    // Otherwise, create a script that will run the bytecode through the JIT.
    if (Native) {
      // Name of the Assembly Language output file
      std::string AssemblyFile = OutputFilename + ".s";

      // Mark the output files for removal if we get an interrupt.
      RemoveFileOnSignal(AssemblyFile);
      RemoveFileOnSignal(OutputFilename);

      // Determine the locations of the llc and gcc programs.
      std::string llc = FindExecutable("llc", argv[0]);
      std::string gcc = FindExecutable("gcc", argv[0]);
      if (llc.empty())
        return PrintAndReturn(argv[0], "Failed to find llc");

      if (gcc.empty())
        return PrintAndReturn(argv[0], "Failed to find gcc");

      // Generate an assembly language file for the bytecode.
      if (Verbose) std::cout << "Generating Assembly Code\n";
      GenerateAssembly(AssemblyFile, RealBytecodeOutput, llc, envp);
      if (Verbose) std::cout << "Generating Native Code\n";
      GenerateNative(OutputFilename, AssemblyFile, Libraries, LibPaths,
                     gcc, envp);

      // Remove the assembly language file.
      removeFile (AssemblyFile);
    } else if (NativeCBE) {
      std::string CFile = OutputFilename + ".cbe.c";

      // Mark the output files for removal if we get an interrupt.
      RemoveFileOnSignal(CFile);
      RemoveFileOnSignal(OutputFilename);

      // Determine the locations of the llc and gcc programs.
      std::string llc = FindExecutable("llc", argv[0]);
      std::string gcc = FindExecutable("gcc", argv[0]);
      if (llc.empty())
        return PrintAndReturn(argv[0], "Failed to find llc");
      if (gcc.empty())
        return PrintAndReturn(argv[0], "Failed to find gcc");

      // Generate an assembly language file for the bytecode.
      if (Verbose) std::cout << "Generating Assembly Code\n";
      GenerateCFile(CFile, RealBytecodeOutput, llc, envp);
      if (Verbose) std::cout << "Generating Native Code\n";
      GenerateNative(OutputFilename, CFile, Libraries, LibPaths, gcc, envp);

      // Remove the assembly language file.
      removeFile(CFile);

    } else {
      EmitShellScript(argv);
    }
  
    // Make the script executable...
    MakeFileExecutable(OutputFilename);

    // Make the bytecode file readable and directly executable in LLEE as well
    MakeFileExecutable(RealBytecodeOutput);
    MakeFileReadable(RealBytecodeOutput);
  }

  return 0;
}
