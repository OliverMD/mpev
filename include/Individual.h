//
// Created by Oliver Downard on 18/12/2017.
//
#pragma once

#include <string>

class IndividualRep {
public:
  virtual std::string name() = 0;
};

class IntIndividualRep : public IndividualRep {
public:
  IntIndividualRep(int val) : value{val} {}
  std::string name() { return "int"; }
  int getValue() { return value; }

private:
  int value;
};

struct Individual {
  Individual(std::unique_ptr<IndividualRep> rep, float fit)
      : representation{std::move(rep)}, fitness{fit} {}

  std::unique_ptr<IndividualRep> representation;
  float fitness;
};

/**
 * Basic Int individual maker
 * @return A random Individual with an IntIndividualRep
 */
Individual makeRandomIntIndivdual();
