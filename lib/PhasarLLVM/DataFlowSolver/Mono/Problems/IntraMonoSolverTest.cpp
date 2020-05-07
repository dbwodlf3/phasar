/******************************************************************************
 * Copyright (c) 2017 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

/*
 * MonoSolverTest.cpp
 *
 *  Created on: 06.06.2017
 *      Author: philipp
 */

#include <algorithm>
#include <iostream>
#include <utility>

#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

#include "phasar/PhasarLLVM/ControlFlow/LLVMBasedCFG.h"
#include "phasar/PhasarLLVM/Pointer/LLVMPointsToInfo.h"
#include "phasar/PhasarLLVM/TypeHierarchy/LLVMTypeHierarchy.h"
#include "phasar/Utils/LLVMShorthands.h"

#include "phasar/PhasarLLVM/DataFlowSolver/Mono/Problems/IntraMonoSolverTest.h"
using namespace std;
using namespace psr;

namespace psr {

IntraMonoSolverTest::IntraMonoSolverTest(const ProjectIRDB *IRDB,
                                         const LLVMTypeHierarchy *TH,
                                         const LLVMBasedCFG *CF,
                                         const LLVMPointsToInfo *PT,
                                         std::set<std::string> EntryPoints)
    : IntraMonoProblem<IntraMonoSolverTest::n_t, IntraMonoSolverTest::d_t,
                       IntraMonoSolverTest::f_t, IntraMonoSolverTest::t_t,
                       IntraMonoSolverTest::v_t, IntraMonoSolverTest::i_t>(
          IRDB, TH, CF, PT, std::move(EntryPoints)) {}

BitVectorSet<const llvm::Value *>
IntraMonoSolverTest::merge(const BitVectorSet<const llvm::Value *> &Lhs,
                           const BitVectorSet<const llvm::Value *> &Rhs) {
  cout << "IntraMonoSolverTest::join()\n";
  return Lhs.setUnion(Rhs);
}

bool IntraMonoSolverTest::equal_to(
    const BitVectorSet<const llvm::Value *> &Lhs,
    const BitVectorSet<const llvm::Value *> &Rhs) {
  cout << "IntraMonoSolverTest::sqSubSetEqual()\n";
  return Lhs == Rhs;
}

BitVectorSet<const llvm::Value *>
IntraMonoSolverTest::normalFlow(const llvm::Instruction *S,
                                const BitVectorSet<const llvm::Value *> &In) {
  cout << "IntraMonoSolverTest::normalFlow()\n";
  BitVectorSet<const llvm::Value *> Result;
  Result.insert(In);
  if (const auto *const Store = llvm::dyn_cast<llvm::StoreInst>(S)) {
    Result.insert(Store);
  }
  return Result;
}

unordered_map<const llvm::Instruction *, BitVectorSet<const llvm::Value *>>
IntraMonoSolverTest::initialSeeds() {
  cout << "IntraMonoSolverTest::initialSeeds()\n";
  return {};
}

void IntraMonoSolverTest::printNode(ostream &OS,
                                    const llvm::Instruction *N) const {
  OS << llvmIRToString(N);
}

void IntraMonoSolverTest::printDataFlowFact(ostream &OS,
                                            const llvm::Value *D) const {
  OS << llvmIRToString(D);
}

void IntraMonoSolverTest::printFunction(ostream &OS,
                                        const llvm::Function *M) const {
  OS << M->getName().str();
}

} // namespace psr
