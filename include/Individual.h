//
// Created by Oliver Downard on 18/12/2017.
//
#pragma once

#include <string>
#include <vector>
class Context;

class IndividualRep {
public:
  virtual std::string name() = 0;
  virtual std::unique_ptr<IndividualRep> copy() = 0;
  virtual std::string toString() const = 0;
};

class IntIndividualRep : public IndividualRep {
public:
  IntIndividualRep(int val) : value{val} {}
  std::string name() override { return "int"; }
  int getValue() { return value; }
  std::unique_ptr<IndividualRep> copy() override {
    return std::make_unique<IntIndividualRep>(value);
  }
  std::string toString() const override { return std::to_string(value); }

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
Individual makeRandomIntIndivdual(Context &ctx);

std::vector<Individual> crossoverIntIndividuals(Context &ctx, Individual &b,
                                                Individual &a);

Individual mutateIntIndividual(Context &ctx, Individual &a);