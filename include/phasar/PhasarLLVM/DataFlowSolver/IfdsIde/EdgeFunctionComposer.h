/******************************************************************************
 * Copyright (c) 2017 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

#ifndef PHASAR_PHASARLLVM_IFDSIDE_EDGEFUNCTIONCOMPOSER_H
#define PHASAR_PHASARLLVM_IFDSIDE_EDGEFUNCTIONCOMPOSER_H

#include "phasar/PhasarLLVM/DataFlowSolver/IfdsIde/EdgeFunctions.h"

#include <memory>
#include <set>

namespace psr {

/**
 * This abstract class models edge function composition. It holds two edge
 * functions. The edge function computation order is implemented as
 *  F -> G -> otherFunction
 * i.e. F is computed before G, G is computed before otherFunction.
 *
 * Note that an own implementation for the join function is required, since
 * this varies between different analyses, and is not implemented by this
 * class.
 * It is also advised to provide a more precise compose function, which is able
 * to reduce the result of the composition, rather than using the default
 * implementation. By default, an explicit composition is used. Such a function
 * definition can grow unduly large.
 */
template <typename AnalysisDomainTy,
          typename Container = std::set<typename AnalysisDomainTy::d_t>>
class EdgeFunctionComposer
    : public EdgeFunction<AnalysisDomainTy, Container> {
public:
  using typename EdgeFunction<AnalysisDomainTy, Container>::EdgeFunctionPtrType;
  using L = typename AnalysisDomainTy::l_t;

private:
  // For debug purpose only
  const unsigned EFComposer_Id;
  static inline unsigned CurrEFComposer_Id = 0;

protected:
  /// First edge function
  EdgeFunctionPtrType F;
  /// Second edge function
  EdgeFunctionPtrType G;

public:
  EdgeFunctionComposer(EdgeFunctionPtrType F, EdgeFunctionPtrType G)
      : EFComposer_Id(++CurrEFComposer_Id), F(F), G(G) {}

  ~EdgeFunctionComposer() override = default;

  /**
   * Target value computation is implemented as
   *     G(F(source))
   */
  L computeTarget(L source) override {
    return G->computeTarget(F->computeTarget(source));
  }

  /**
   * Function composition is implemented as an explicit composition, i.e.
   *     (secondFunction * G) * F = EFC(F, EFC(G , otherFunction))
   *
   * However, it is advised to immediately reduce the resulting edge function
   * by providing an own implementation of this function.
   */
  EdgeFunctionPtrType composeWith(EdgeFunctionPtrType secondFunction, MemoryManager<AnalysisDomainTy, Container> memManager) override {
    if (auto *EI = dynamic_cast<EdgeIdentity<AnalysisDomainTy, Container> *>(secondFunction)) {
      return this;
    }
    if (auto *AB = dynamic_cast<AllBottom<AnalysisDomainTy, Container> *>(secondFunction)) {
      return this;
    }
    return F->composeWith(G->composeWith(secondFunction, memManager),memManager);
  }

  // virtual EdgeFunctionPtrType
  // joinWith(EdgeFunctionPtrType otherFunction) = 0;

  bool equal_to(EdgeFunctionPtrType other) const override {
    if (auto EFC = dynamic_cast<EdgeFunctionComposer<AnalysisDomainTy, Container> *>(other)) {
      return F->equal_to(EFC->F) && G->equal_to(EFC->G);
    }
    return false;
  }

  void print(std::ostream &OS, bool isForDebug = false) const override {
    OS << "COMP[ " << F->str() << " , " << G->str()
       << " ] (EF:" << EFComposer_Id << ')';
  }
};

} // namespace psr

#endif
