//
// Created by Oliver Downard on 17/12/2017.
//

#pragma once

#include <memory>
#include <string>

class Population;

/*
 * Interface for population states
 * May need to become an abstract class in the future
 */
class PopulationState {
public:

    /**
     * void execute(Population& p)
     * execute the state.
     *
     * @return The new state, nullptr if no change
     */
    virtual std::unique_ptr<PopulationState> execute(Population& pop) = 0;

    /**
     *
     * @return the name of the state
     */
    virtual std::string name() const = 0;
};

class InitialPopState : public PopulationState {
public:
    std::unique_ptr<PopulationState> execute(Population& pop) override;
    std::string name() const override { return "InitialPopState"; }
};

class Population {
public:
    Population() : state{new InitialPopState()} {}
    Population(std::unique_ptr<PopulationState> startState) : state{std::move(startState)} {}
    void step() {
        auto newState = state->execute(*this);
        if (newState != nullptr) {
            state = std::move(newState);
        }
    }
    const PopulationState* getState() const { return state.get();}
private:
    std::unique_ptr<PopulationState> state;
};