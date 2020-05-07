/******************************************************************************
 * Copyright (c) 2018 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

#ifndef PHASAR_PHASARLLVM_IFDSIDE_PROBLEMS_TYPESTATEDESCRIPTIONS_TYPESTATEDESCRIPTION_H_
#define PHASAR_PHASARLLVM_IFDSIDE_PROBLEMS_TYPESTATEDESCRIPTIONS_TYPESTATEDESCRIPTION_H_

#include <set>
#include <string>

#include "llvm/IR/CallSite.h"

namespace psr {

/**
 * Interface for a type state problem to be used with the IDETypeStateAnalysis.
 * It needs to provide a finite state machine to handle state changes and a list
 * of functions that are relevant to the analyzed api.
 * Factory functions are those functions which generate objects/variables of
 * target type, e.g. file handles generated by fopen().
 * Consuming functions operate on objects/variables of target type, e.g.
 * fwrite(). Functions that are both factory and consumer, e.g. freopen(),
 * cannot be modeled by this interface, since analyzing such functions correctly
 * introduces dependencies that cannot be solved by a distributive framework
 * such as IDE.
 *
 * @see CSTDFILEIOTypeStateDescription as an example of type state description.
 */
struct TypeStateDescription {
  /// Type for states of the finite state machine
  typedef int State;
  virtual ~TypeStateDescription() = default;
  virtual bool isFactoryFunction(const std::string &F) const = 0;
  virtual bool isConsumingFunction(const std::string &F) const = 0;
  virtual bool isAPIFunction(const std::string &F) const = 0;

  /**
   * @brief For a given function name (as a string token) and a state, this
   * function returns the next state.
   */
  virtual State getNextState(std::string Tok, State S) const = 0;
  virtual State getNextState(const std::string &Tok, State S,
                             llvm::ImmutableCallSite CS) const {
    return getNextState(Tok, S);
  }
  virtual std::string getTypeNameOfInterest() const = 0;
  virtual std::set<int> getConsumerParamIdx(const std::string &F) const = 0;
  virtual std::set<int> getFactoryParamIdx(const std::string &F) const = 0;
  virtual std::string stateToString(State S) const = 0;
  virtual State bottom() const = 0;
  virtual State top() const = 0;

  /**
   * Represents the uninitialized state of an object, e.g. allocation of a file
   * handle
   */
  virtual State uninit() const = 0;

  /**
   * Represents the start/initial state of an object after creation, e.g. state
   * of a file handle after fopen()
   */
  virtual State start() const = 0;

  /**
   * Represents the error state of an object
   */
  virtual State error() const = 0;
};

} // namespace psr

#endif
