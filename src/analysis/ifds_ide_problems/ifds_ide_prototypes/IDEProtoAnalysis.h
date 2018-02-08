/******************************************************************************
 * Copyright (c) 2017 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

/*
 * IDEProtoAnalysis.h
 *
 *  Created on: 15.09.2017
 *      Author: philipp
 */

#ifndef SRC_ANALYSIS_IFDS_IDE_PROBLEMS_IDEPROTOANALYSIS_H_
#define SRC_ANALYSIS_IFDS_IDE_PROBLEMS_IDEPROTOANALYSIS_H_

#include "../../../lib/LLVMShorthands.h"
#include "../../control_flow/LLVMBasedICFG.h"
#include "../../ifds_ide/DefaultIDETabulationProblem.h"
#include "../../ifds_ide/DefaultSeeds.h"
#include "../../ifds_ide/FlowFunction.h"
#include "../../ifds_ide/ZeroValue.h"
#include "../../ifds_ide/edge_func/EdgeIdentity.h"
#include "../../ifds_ide/flow_func/Identity.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>
using namespace std;

class IDEProtoAnalysis
    : public DefaultIDETabulationProblem<
          const llvm::Instruction *, const llvm::Value *,
          const llvm::Function *, const llvm::Value *, LLVMBasedICFG &> {
private:
  vector<string> EntryPoints;

public:
  IDEProtoAnalysis(LLVMBasedICFG &icfg, vector<string> EntryPoints = {"main"});

  virtual ~IDEProtoAnalysis() = default;

  // start formulating our analysis by specifying the parts required for IFDS

  shared_ptr<FlowFunction<const llvm::Value *>>
  getNormalFlowFunction(const llvm::Instruction *curr,
                        const llvm::Instruction *succ) override;

  shared_ptr<FlowFunction<const llvm::Value *>>
  getCallFlowFunction(const llvm::Instruction *callStmt,
                      const llvm::Function *destMthd) override;

  shared_ptr<FlowFunction<const llvm::Value *>>
  getRetFlowFunction(const llvm::Instruction *callSite,
                     const llvm::Function *calleeMthd,
                     const llvm::Instruction *exitStmt,
                     const llvm::Instruction *retSite) override;

  shared_ptr<FlowFunction<const llvm::Value *>>
  getCallToRetFlowFunction(const llvm::Instruction *callSite,
                           const llvm::Instruction *retSite) override;

  shared_ptr<FlowFunction<const llvm::Value *>>
  getSummaryFlowFunction(const llvm::Instruction *callStmt,
                         const llvm::Function *destMthd) override;

  map<const llvm::Instruction *, set<const llvm::Value *>>
  initialSeeds() override;

  const llvm::Value *createZeroValue() override;

  bool isZeroValue(const llvm::Value *d) const override;

  // in addition provide specifications for the IDE parts

  shared_ptr<EdgeFunction<const llvm::Value *>> getNormalEdgeFunction(
      const llvm::Instruction *curr, const llvm::Value *currNode,
      const llvm::Instruction *succ, const llvm::Value *succNode) override;

  shared_ptr<EdgeFunction<const llvm::Value *>>
  getCallEdgeFunction(const llvm::Instruction *callStmt,
                      const llvm::Value *srcNode,
                      const llvm::Function *destiantionMethod,
                      const llvm::Value *destNode) override;

  shared_ptr<EdgeFunction<const llvm::Value *>> getReturnEdgeFunction(
      const llvm::Instruction *callSite, const llvm::Function *calleeMethod,
      const llvm::Instruction *exitStmt, const llvm::Value *exitNode,
      const llvm::Instruction *reSite, const llvm::Value *retNode) override;

  shared_ptr<EdgeFunction<const llvm::Value *>>
  getCallToReturnEdgeFunction(const llvm::Instruction *callSite,
                              const llvm::Value *callNode,
                              const llvm::Instruction *retSite,
                              const llvm::Value *retSiteNode) override;

  shared_ptr<EdgeFunction<const llvm::Value *>>
  getSummaryEdgeFunction(const llvm::Instruction *callSite,
                         const llvm::Value *callNode,
                         const llvm::Instruction *retSite,
                         const llvm::Value *retSiteNode) override;

  const llvm::Value *topElement() override;

  const llvm::Value *bottomElement() override;

  const llvm::Value *join(const llvm::Value *lhs,
                          const llvm::Value *rhs) override;

  shared_ptr<EdgeFunction<const llvm::Value *>> allTopFunction() override;

  class IDEProtoAnalysisAllTop
      : public EdgeFunction<const llvm::Value *>,
        public enable_shared_from_this<IDEProtoAnalysisAllTop> {
    const llvm::Value *computeTarget(const llvm::Value *source) override;

    shared_ptr<EdgeFunction<const llvm::Value *>> composeWith(
        shared_ptr<EdgeFunction<const llvm::Value *>> secondFunction) override;

    shared_ptr<EdgeFunction<const llvm::Value *>> joinWith(
        shared_ptr<EdgeFunction<const llvm::Value *>> otherFunction) override;

    bool equalTo(shared_ptr<EdgeFunction<const llvm::Value *>> other) override;
  };

  string DtoString(const llvm::Value *d) override;

  string VtoString(const llvm::Value *v) override;

  string NtoString(const llvm::Instruction *n) override;

  string MtoString(const llvm::Function *m) override;
};

#endif /* SRC_ANALYSIS_IFDS_IDE_PROBLEMS_IDEPROTOANALYSIS_HH_ */