//
// Created by Patryk Pilichowski (s0220383) on 23/02/2023.
//

#include "DFA.h"
#include "json.hpp"
#include <iomanip>
#include <fstream>
#include <cassert>

bool is_subset_of(const std::set<State*> &subset, const std::set<State*> &superset) {
    for(State* current_state1 : subset) {
        if(superset.find(current_state1) == superset.end()) return false;
    }
    return true;
}
bool is_element_of(State* const &element, const std::set<State *> &state_set) {
    return state_set.find(element) != state_set.end();
}

void DFA::validate() const {
    assert(is_subset_of(end_states, states));
    assert(is_element_of(start_state, states));

    TransitionMap transition_map_copy = transition_map;

    for(State* current_state : states) {
        auto found_state = transition_map_copy.find(current_state);
        for(char character : alphabet) {
            assert(found_state != transition_map_copy.end() && "Expected state to be in transition_map");
            auto found_output_state = found_state->second.find(character);
            assert(found_output_state != found_state->second.end() &&
                   "Expected character and state combination to be in transition map");
            found_state->second.erase(found_output_state);
        }
        assert(found_state->second.empty() && "Expected precisely one mapping per unique state and input combination");
        transition_map_copy.erase(found_state);
    }
    assert(transition_map_copy.empty() && "Expected each state in transition map to be legal");
}

DFA::~DFA() {
    for(State* current_state : states) {
        delete current_state;
    }
}
std::string get_product_name(const std::string &name1, const std::string &name2) {
    return name1+" , "+name2;
}
State* make_product_state(const State* const state1, const State* const state2, const bool &intersection) {
    const bool is_starting = state1->get_isStarting() && state2->get_isStarting();
    const bool is_accepting = (intersection) ?
            state1->get_isAccepting() && state2->get_isAccepting() :
            state1->get_isAccepting() || state2->get_isAccepting();
    auto result = new State(get_product_name(state1->get_naam(), state2->get_naam()),
                 is_starting, is_accepting);
    return result;
}
DFA::DFA(const DFA &dfa1, const DFA &dfa2, const bool &intersection) {
    if(dfa1.get_alphabet() != dfa2.get_alphabet())
        throw std::invalid_argument("The DFA's don't have the same alphabet!");
    const State* dfa1_currentState = dfa1.get_start_state();
    const State* dfa2_currentState = dfa2.get_start_state();
    std::set<State*> dfa1_reachableStates;
    std::set<State*> dfa2_reachableStates;
    /*TO DO while(true) {
        State* product_state = make_product_state(dfa1_currentState, dfa2_currentState, intersection);
            if(product_state->get_isStarting()) start_state = product_state;
            if(product_state->get_isAccepting()) end_states.insert(product_state);
    }*/
}

DFA::DFA(const std::string &relativeFile) {
    std::ifstream input(relativeFile);
    nlohmann::json data = nlohmann::json::parse(input);
    for(auto &elem : data["alphabet"]) {
        alphabet.insert(elem.get<std::string>()[0]);
    }
    for(auto &elem : data["states"]) {
        auto new_state = new State(elem["name"].get<std::string>());
        if (elem["starting"].get<bool>()) {
            new_state->set_isStarting(true);
            start_state=new_state;
        }
        if (elem["accepting"].get<bool>()) {
            new_state->set_isAccepting(true);
            end_states.insert(new_state);
        }
        states.insert(new_state);
    }
    for(auto &elem : data["transitions"]) {
        auto from_state = get_state_by_name(elem["from"].get<std::string>());
        auto to_state = get_state_by_name(elem["to"].get<std::string>());
        char curr_input = elem["input"].get<std::string>()[0];

        assert(from_state != nullptr);
        assert(to_state != nullptr);

        if(transition_map.find(from_state) == transition_map.end()) {
            std::map<char, State*> new_map = {{curr_input, to_state}};
            transition_map.insert({from_state, new_map});
        }
        else {
            transition_map.at(from_state).insert({curr_input, to_state});
        }
    }
    validate();
}

// getters implementation
std::set<State*> DFA::get_states() const {
    return states;
}
std::set<char> DFA::get_alphabet() const {
    return alphabet;
}
std::set<State*> DFA::get_end_states() const {
    return end_states;
}
State* DFA::get_start_state() const {
    return start_state;
}
TransitionMap DFA::get_transition_map() const {
    return transition_map;
}
State* DFA::get_state_by_name(const std::string &name) const {
    for(auto &current_state : states) {
        if(current_state->get_naam() == name) return current_state;
    }
    return nullptr;
}

// other
bool DFA::accepts(const std::string &str) const {
    State* current_state = start_state;
    for(char current_char : str) {
        current_state = transition_map.at(current_state).at(current_char);
    }
    return is_element_of(current_state, end_states);
}
void DFA::print() const {
    nlohmann::json j;
    j["type"] = "DFA";
    j["alphabet"] = {};
    j["states"] = {};
    j["transitions"] = {};
    for(const char currentChar: alphabet) {
        j["alphabet"].push_back(std::string(1,currentChar));
    }
    for(State* currentState : states) {
        j["states"].push_back({{"name", currentState->get_naam()}, {"starting", currentState->get_isStarting()},
                               {"accepting", currentState->get_isAccepting()}});
    }
    for(Transitions currentTransitions : transition_map) {
        for(auto currentTransition : currentTransitions.second) {
            j["transitions"].push_back({{"from", currentTransitions.first->get_naam()},
                                        {"to", currentTransition.second->get_naam()},
                                        {"input", std::string(1, currentTransition.first)}});
        }
    }
    std::cout << std::setw(4) << j << std::endl;
}